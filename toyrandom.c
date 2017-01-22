#include "toyrandom.h"
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <math.h>
#include <stdint.h>

static gsl_rng *r;
void   random_init(uint64_t seed)
{
    r = gsl_rng_alloc( gsl_rng_mt19937 );
    if ( seed )
        gsl_rng_set( r, seed);
}

double random_horse()
{
    return fmax(0, 5.0 + gsl_ran_gaussian( r, 2.0 ));
}

double random_ball()
{
    return fmax( 0, 2.0 + gsl_ran_gaussian( r, 0.3 ));
}

double random_bike()
{
    return fmax( 0, 20.0 + gsl_ran_gaussian( r, 10.0 ));
}

double random_train()
{
    return fmax( 0, 10.0 + gsl_ran_gaussian( r, 5.0 ));
}

double random_coal()
{
    return 47 * gsl_ran_beta( r, 0.5, 0.5 );
}

double random_book()
{
    return gsl_ran_chisq( r, 2.0 );
}

double random_doll()
{
    return gsl_ran_gamma( r, 5.0, 1.0 );
}

double random_blocks()
{
    const double a=5;
    const double b=20;
    const double c=10;

    const double fc = (c-a)/(b-a);
    double u = gsl_rng_uniform_pos( r );
    if( u < fc )
        return a + sqrt(u*(b-a)*(c-a));
    else
        return b - sqrt((1.0-u)*(b-a)*(b-c));
}

double random_gloves()
{
    double u = gsl_rng_uniform_pos( r );    
    if( gsl_rng_uniform_pos( r ) < 0.3 )
        return 3.0 + u;
    else
        return u;
}

double (*random_func[n_toy_types])(void) = {
        [horse]  = random_horse,
        [ball]   = random_ball,
        [bike]   = random_bike, 
        [train]  = random_train, 
        [coal]   = random_coal,
        [book]   = random_book,
        [doll]   = random_doll,
        [blocks] = random_blocks,
        [gloves] = random_gloves
};
