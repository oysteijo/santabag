#include "toys.h"
#include "bagutils.h"
#include <glpk.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define LINE_LEN 256
#define MAX_N_BAGS 1000
#define SUBMISSIONFILE "submission.csv"
static const int toy_maxcount[n_toy_types] = {
        [horse]  =  1000,
        [ball]   =  1100,
        [bike]   =   500, 
        [train]  =  1000, 
        [coal]   =   166,
        [book]   =  1200,
        [doll]   =  1000,
        [blocks] =  1000,
        [gloves] =   200
};

/* Plain Fisher-Yates */
static void shuffle_array( int *array, int n )
{
    for( int i = n-1; i > 0; i-- ){
        int idx = rand() % (i+1);
        int tmp = array[i];
        array[i] = array[idx];
        array[idx] = tmp;
    }
}

int main(int argc, char *argv[])
{
    glp_prob *lp;
    double alpha = 0.0;
    
    if( argc == 3 ) alpha = atof(argv[2]);  /* FIXME: strtof and check */

    if( (argc < 2) || (argc > 3)){
        fprintf( stderr, "Usage: %s <filename> <alpha>\n", argv[0]);
        exit(1);
    }

    /* Read the comma separated value file */
    FILE *fp;
    if(NULL == (fp=fopen(argv[1], "r"))){
        perror(argv[1]);
        exit(1);
    }

    int linecount = 0;
    char line[LINE_LEN];
    while(fgets( line, sizeof(line), fp)){
        if(line[0]=='#') continue;
        linecount++;
    }
    rewind(fp);
    int n_columns = linecount - 1;  /* subtract one for the header line */
    
    /* allocate memory for the file */
    /* Do I need that? Well ... on the stack then! */
    char varnames[n_columns][n_toy_types+1];
    double means[n_columns];
    double stds[n_columns];

    /* Aaargh! This is buggy... Do not run this on a manually modified csv file! */
    for(int i = 0; fgets( line, sizeof(line), fp); i++ ){
        if(!strncmp( line, "bag", 3)) continue; /* skip header line */
        double mean_dummy, std_dummy;
        int n = sscanf( line, "%[^,],%lf,%lf,%lf,%lf", varnames[i-1], &mean_dummy, &std_dummy, &means[i-1], &stds[i-1] );
        if( n != 5 ){
            fprintf(stderr, "Cannot read line %d in '%s'\n -> %s\n", i, argv[1], line );
            exit(1);
        }
    }
    fclose(fp);

    /* create problem */
    if(NULL == (lp = glp_create_prob())){
        fprintf(stderr, "Cannot create GLPK solver");
        exit(1);
    }
    glp_set_prob_name(lp, "Santa's Uncertain Bags");
    glp_set_obj_dir(lp, GLP_MAX);
    
    int    *ia = malloc( n_columns * (n_toy_types+1) * sizeof(int));
    int    *ja = malloc( n_columns * (n_toy_types+1) * sizeof(int)); 
    double *ar = malloc( n_columns * (n_toy_types+1) * sizeof(double)); 
    
    if( !ia || !ja || !ar ) {
        fprintf( stderr, "Cannot allocate mamory for sparse matrix.\n");
        exit(1);
    }

    /* fill problem */
    glp_add_rows(lp, n_toy_types + 1);
    for( int i = 0; i < n_toy_types; i++){
        glp_set_row_name(lp, i+1, toy_to_string(i));
        glp_set_row_bnds(lp, i+1, GLP_UP, 0.0, toy_maxcount[i]);
    }
    /* Total bags bound */
    glp_set_row_name(lp, 10, "Total");
    glp_set_row_bnds(lp, 10, GLP_UP, 0.0, 1000);  /* Max 1000 bags */

    glp_add_cols(lp, n_columns);
    for( int i = 0; i < n_columns; i++){
        glp_set_col_name(lp, i+1, varnames[i]);
        glp_set_col_bnds(lp, i+1, GLP_LO, 0.0, 0.0);  /* ??? */
        glp_set_col_kind(lp, i+1, GLP_IV); 
        glp_set_obj_coef(lp, i+1, means[i] + alpha*stds[i] );
    }

    int c = 0;
    for( int i = 0; i < n_toy_types; i++ )
        for( int j = 0; j < n_columns; j++ ){
            const char n = (strchr(ascii_str,varnames[j][i]) - ascii_str);
            if(!n) continue;
            c++;
            ia[c] = i+1, ja[c] = j+1, ar[c] = (double) n;
        }

    for( int j = 0; j < n_columns; j++ ){
        c++;
        ia[c] = 10, ja[c] = j+1, ar[c] = 1.0;
    }
    ia = realloc( ia, c * sizeof(int));
    ja = realloc( ja, c * sizeof(int));
    ar = realloc( ar, c * sizeof(double));
    if( !ia || !ja || !ar ) {
        fprintf( stderr, "Cannot reallocate memory for sparse matrix.\n");
        exit(1);
    }

    glp_load_matrix(lp, c, ia, ja, ar);
    
    /* solve problem */
    glp_iocp parm;
    glp_init_iocp(&parm);
    parm.presolve = GLP_ON;
    int err = glp_intopt(lp, &parm);
    if( err ){
        fprintf( stderr, "No success. Check return code: (%d)\n", err);
        exit(err);
    }
    
    /* recover and display results and write submisison file */
    printf("Objective at solution: %g\n", glp_mip_obj_val(lp));
    for (int i = 0; i < n_columns; i++ ){
        double x = glp_mip_col_val(lp, i+1);
        if( x > 0.0 )
            printf("%s x %3g\n", glp_get_col_name(lp,i+1), x);
    } 

    /* print to file */
    /* First step for randomizing the toys is to generate an array of yoys to pick from,
     * Then we random shuffle this array and the we pick sequencially.
     * A shuffle code is so simple that make our own instead of using GSL */

    srand(time(NULL)); /* Make your own seed */
    int *number_array[n_toy_types];
    for ( int i = 0 ; i < n_toy_types; i++ ) {
        number_array[i] = malloc( toy_maxcount[i] * sizeof(int));
        if(!number_array[i]){fprintf(stderr,"Fuck it! Let's go bowling!\n"); exit(1);}
        for ( int j = 0 ; j < toy_maxcount[i] ; j++ )
            number_array[i][j] = j;
        shuffle_array( number_array[i], toy_maxcount[i] );
    }
    
    int toy_counter[n_toy_types];
    memcpy( toy_counter, toy_maxcount, n_toy_types * sizeof(int));

    if( NULL == ( fp = fopen(SUBMISSIONFILE, "w" ))){
        perror(SUBMISSIONFILE);
        exit(1);  /* FIXME: cleanup first */
    }
    fprintf( fp, "Gifts\n");
    /* Loop solutions */
    for ( int col = 0; col < n_columns; col++ ){
        double x = glp_mip_col_val(lp, col+1);
        if ( x > 0.0 ){
            char bag[10];
            strncpy( bag, glp_get_col_name( lp, col+1 ), 10);
            string_to_bag( bag );
            int n_lines = (int) lround(x);
            while ( n_lines-- ){
                for ( int tt = 0 ; tt < n_toy_types; tt++ ){
                    int ntoy = (int) bag[tt];
                    while( ntoy-- )
                        fprintf( fp, "%s_%d ", toy_to_string(tt), number_array[tt][--toy_counter[tt]]);
                    /* Will I ever understand this code when I read it again? */
                }
                fprintf(fp,"\n");
            }
        }
    }
    fclose(fp);
    /* Free the number_arrays */
    for(int i=0; i < n_toy_types; i++ )
        free( number_array[i] );

    /* housekeeping */
    glp_delete_prob(lp);
    glp_free_env();
    free(ia);
    free(ja);
    free(ar);
    return 0;
}
