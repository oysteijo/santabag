#ifndef __TOYS_H__
#define __TOYS_H__
#include <string.h>
typedef enum toy_t_ { horse, ball, bike, train, coal, book, doll, blocks, gloves, n_toy_types} toy_t;

static char *toy_name[n_toy_types] = {
    [horse]  =  "horse",
    [ball]   =  "ball",
    [bike]   =  "bike", 
    [train]  =  "train", 
    [coal]   =  "coal",
    [book]   =  "book",
    [doll]   =  "doll",
    [blocks] =  "blocks",
    [gloves] =  "gloves"
};

inline static int string_to_toy(const char *s)
{
    for(int i=0; i < n_toy_types; i++)
        if(!strcmp(s, toy_name[i]))
            return i;
    /* Whoooops */
    return -1;
}

inline static char * toy_to_string( int i ) { return toy_name[i]; }
#endif /* __TOYS_H__ */
