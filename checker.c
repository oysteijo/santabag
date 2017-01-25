#include "strtools.h"
#include "toys.h"
#include "toyrandom.h"
STRSPLIT_INIT;
STRSPLIT_FREE_INIT;
STRSPLIT_LENGTH_INIT;
#include <omp.h>
#include <stdio.h>
#include <math.h>
#include <gsl/gsl_statistics.h>

#define N_BAGS 1000

double simulate_one_bag( char *bag )
{
    double sum = 0.0;
    for( int toytype = 0; toytype < n_toy_types; toytype++ ){
        const int n = bag[toytype];
        for ( int j = 0; j < n; j++)
            sum += random_func[toytype]();
    }
    return sum;
}

double simulate_bag_set( char submission[N_BAGS][n_toy_types+1], double *reject_rate )
{
    double tot_sum = 0.0;
    double reject = 0.0;
    for ( int i = 0; i < N_BAGS; i++ ){
        char *bag = submission[i];
        double bag_weight = simulate_one_bag( bag );
        if( bag_weight <= 50.0 )
            tot_sum += bag_weight;
        else
            reject += 1.0;
    }
    if( reject_rate ) *reject_rate = reject / N_BAGS;
    return tot_sum;
}
double gauss_cdf( double x )
{
    return 0.5 + 0.5*erf(x / M_SQRT2);
}
int main(int argc, char *argv[] )
{
    unsigned int n;
    double reference_score = -1.0;
    int n_submissions = 1;

    /* Parse command line arguments */
    if( argc == 2)
        n = 100;
    if (argc >= 3)
        n = atoi(argv[2]); /* FIXME: check error */
    if (argc >= 4)
        reference_score = strtod( argv[3], NULL );
    if (argc == 5)
        n_submissions = atoi( argv[4] );
    
    if(argc < 2 || argc > 5) {
        fprintf(stderr, "Usage: %s <filename> <n_trials> <reference_score> <n_submissions>\n", argv[0]);
        return 0;
    }
    /* Open file */
    FILE *fp;
    if( !(fp =fopen(argv[1], "r") )){
        perror(argv[1]);
        return -1;
    }

    /* read and parse file */
    int linecounter = 0;
    char buffer[256];
    char submission[N_BAGS][n_toy_types+1] = {{0}}; 
    while ( fgets( buffer, sizeof buffer, fp ) ){
        if(!strncmp("Gifts", buffer, 5)) continue; 
        char **spl = strsplit(strstrip(buffer), ' ');
        int n_items = strsplit_length( spl );
        for( int i=0; i < n_items; i++ ){
            char toy[10];
            int  id;
            int  ck = sscanf( strstrip(spl[i]), "%10[^_]_%d", toy, &id );
            if(ck!=2) {fprintf(stderr,"%s, Could not parse: %s\n", toy, spl[i]); exit(-1); }
            submission[linecounter][string_to_toy(toy)]++;
        }
        strsplit_free( spl );
        linecounter++;
    }

    /* Start simulation */
    random_init(2016);  /* If threading is enabled, setting the sames seed still won't make
                           the simulations reproducable. That's the cost of threading */
    double all[n];
    double reject = 0.0;    
#pragma omp parallel for reduction(+:reject)
    for (unsigned int i = 0; i < n; i++ ){
        double r;
        all[i]= simulate_bag_set( submission, &r );
        reject += r;
    }

    /* Calculate mean and standard deviation */
    double mean = gsl_stats_mean( all, 1, n);
    double std  = gsl_stats_variance_m( all, 1, n, mean);
    std = sqrt(std);

    /* Report the results */
    printf("Total weight: %5.2f  Standard dev.: %5.2f\n", mean, std);
    printf("Mean reject rate: %5.6f\n", reject / n);

    if( reference_score > 0 ){
        double not_prob = gauss_cdf( (reference_score - mean) / std );
        printf("probability of surpassing %5.2lf (ref) in one submission  = %5.5lf\n",
                reference_score, 1.0 - not_prob );
        if( n_submissions > 1 )
            printf("probability of surpassing %5.2lf (ref) in %3d submissions = %5.5lf\n",
                reference_score, n_submissions, 1.0 - pow(not_prob, n_submissions));
    }
    return 0;
}
