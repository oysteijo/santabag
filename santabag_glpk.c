#include "glpk.h"
#include "toys.h"
#include "bagutils.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define LINE_LEN 256
#define MAX_N_BAGS 1000

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
        assert( n == 5 );
    }
    fclose(fp);

    /* create problem */
    lp = glp_create_prob(); assert(lp);
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
//            static const char *ascii_str = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
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
        if( x > 0.0 ){
            char var[10];
            strncpy(var, glp_get_col_name(lp, i+1), 10); 
            printf("%s x %3g\n", var, x);
        }
    } 

    /* housekeeping */
    glp_delete_prob(lp);
    glp_free_env();
    free(ia);
    free(ja);
    free(ar);
    return 0;
}
