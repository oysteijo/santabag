#ifndef __TOYRANDOM_H__
#define __TOYRANDOM_H__
#include "toys.h"
#include <stdint.h>
void   random_init(uint64_t seed);
double random_horse();
double random_ball();
double random_bike();
double random_train();
double random_coal();
double random_book();
double random_doll();
double random_blocks();
double random_gloves();

double (*random_func[n_toy_types])(void);
#endif  /* __TOYRANDOM_H__ */
