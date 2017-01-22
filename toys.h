#ifndef __TOYS_H__
#define __TOYS_H__ 1
#include <string.h>
typedef enum toy_t_ { horse, ball, bike, train, coal, book, doll, blocks, gloves, n_toy_types} toy_t;

#if 0
/* This is not in use ... */
int toy_prime[n_toy_types] = {
		[horse]  =  2,
		[ball]   =  3,
		[bike]   =  5, 
		[train]  =  7, 
		[coal]   = 11,
		[book]   = 13,
		[doll]   = 17,
		[blocks] = 19,
		[gloves] = 23
};
#endif

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

#if 0
/* Move this to LP part */
static int toy_maxcount[n_toy_types] = {
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
#endif

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

