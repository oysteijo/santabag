#include "toys.h"
#include "toyrandom.h"
#include "bagutils.h"
#include <gsl/gsl_statistics.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#define BAG_LIMIT 50.0
#define RESULTFILE "simulations.csv"

int main( int argc, char *argv[])
{
    int n = 100;  /* Default number of trials/samples */
    if( argc == 2) n = atoi(argv[1]);
    
    double toytype_mean[n_toy_types];
    double toytype_var[n_toy_types];
    int    toytype_max[n_toy_types];

    double *work_array = malloc( n * sizeof(double)); 
    if(!work_array){
        fprintf(stderr,"Cannot allocate memory.\n");
        exit(1);
    }

    random_init(2017);
    for( int toy = 0; toy < n_toy_types; toy++ ){
        int count = n;
        double *ptr = work_array;
        while(count--){
            *ptr++ = random_func[toy]();
        }
        toytype_mean[toy] = gsl_stats_mean( work_array, 1, n );
        toytype_var[toy]  = gsl_stats_variance_m( work_array, 1, n, toytype_mean[toy] );
        toytype_max[toy]  = lround(BAG_LIMIT / toytype_mean[toy]);
        printf("%-10s: %7.3lf %7.3lf  %4d\n", toy_to_string(toy), toytype_mean[toy], toytype_var[toy], toytype_max[toy]); 
    }
    free(work_array);

    /* Step 2 */
	FILE *fp;
	if( NULL == (fp = fopen( RESULTFILE, "w"))){
		perror( RESULTFILE );
		exit(1);
	}
	fprintf(fp,"bag, mean, std, mean_exp, std_exp\n");

    char workbag[n_toy_types+1] = { 0 };
    unsigned int n_combinations = 0;
   	for( char n_horse  = 0; n_horse  < toytype_max[horse] ; n_horse++  )
    for( char n_ball   = 0; n_ball   < toytype_max[ball]  ; n_ball++   )
    for( char n_bike   = 0; n_bike   < toytype_max[bike]  ; n_bike++   )
    for( char n_train  = 0; n_train  < toytype_max[train] ; n_train++  )
    for( char n_coal   = 0; n_coal   < toytype_max[coal]  ; n_coal++   )
    for( char n_book   = 0; n_book   < toytype_max[book]  ; n_book++   )
    for( char n_doll   = 0; n_doll   < toytype_max[doll]  ; n_doll++   )
    for( char n_blocks = 0; n_blocks < toytype_max[blocks]; n_blocks++ )
    for( char n_gloves = 0; n_gloves < toytype_max[gloves]; n_gloves++ )
	{
		workbag[horse]   = n_horse;		
		workbag[ball]    = n_ball;		
		workbag[bike]    = n_bike;		
		workbag[train]   = n_train;		
		workbag[coal]    = n_coal;		
		workbag[book]    = n_book;		
		workbag[doll]    = n_doll;		
		workbag[blocks]  = n_blocks;		
		workbag[gloves]  = n_gloves;		
		/* check at least 3 */
		/* check max 36 */
		/* calc expectation:
		 *   reject bags of expectation > 50 
		 *   reject bags of expectation < 20
		 */
		double estimate = 0.0;
		int n_toys = 0;
		for (int t = 0; t < n_toy_types; t++ ){
			n_toys   += workbag[t];
			estimate += workbag[t] * toytype_mean[t];
		}
	    if( n_toys < 3 ) continue;
	    if( estimate < 20.0 ) continue;   /* FIXME: These limits should  */ 
	    if( estimate > 50.0 ) continue;   /*   configurable */

	    n_combinations++;
		if( !(n_combinations % 200) ){
            time_t rawtime;
            struct tm * timeinfo;
            time ( &rawtime );
            timeinfo = localtime ( &rawtime );
            printf ( "%5d: %s", n_combinations, asctime (timeinfo) );
        }

		/* Do simulation (thread here? Yes!) */
		double *work_array = malloc( n * sizeof(double)); 
        if(!work_array){
            fprintf(stderr,"Cannot allocate memory.\n");
            exit(1);
        }

#pragma omp parallel for 
		for( int i = 0; i < n; i++ ){
			double sum = 0.0;
			for( int t = 0; t < n_toy_types; t++ ){
				int count = workbag[t];				
				while(count--)
					sum += random_func[t]();
			}
			work_array[i] = sum;
        }

        double mean = gsl_stats_mean( work_array, 1, n );
        double var  = gsl_stats_variance_m( work_array, 1, n, mean );
		
		for( int i = 0; i < n; i++ ){
			if( work_array[i] > 50.0 ) work_array[i] = 0.0;
		}

        double mean_exp = gsl_stats_mean( work_array, 1, n );
        double var_exp  = gsl_stats_variance_m( work_array, 1, n, mean_exp );
		free( work_array );
		
        fprintf(fp, "%s, %5.7lf, %5.7lf, %5.7lf, %5.7lf\n",
				bag_to_string(workbag), mean, sqrt(var), mean_exp, sqrt(var_exp));
	}
	fclose(fp);
    random_free();
	printf("Number of bag combinations simulated: %d\n", n_combinations);

    return 0;
}
