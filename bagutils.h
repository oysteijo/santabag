#ifndef __BAGUTILS_H__
#define __BAGUTILS_H__
#include "toys.h"
#include <assert.h>
#include <string.h>
/* we want printable string to represent the bags. We take 9 bytes, one for each
 * toy type (and a terminating \0) and we assign a charater the number of each
 * specific toy. We an not just add 'A', as we then nedd 37 consecutive printable
 * characters which does not have a special meaning in any lp software. (important) */
/*                                 1         2         3         4         5         6  */
/*                       0         0123456789012345678901234567890123456789012345678901 */
const char *ascii_str = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

static inline void string_to_bag( char str[n_toy_types + 1])
{
	assert(str);
	for( int tt; tt < n_toy_types; tt++ )
		str[tt] = (char) (strchr( ascii_str, str[tt] ) - ascii_str);
	return;
}

static inline char *bag_to_string( char bag[n_toy_types + 1])
{
	assert(bag);
	for( int tt = 0; tt < n_toy_types; tt++ )
		bag[tt] = ascii_str[(int)bag[tt]];
	return bag;
}

void bag_set_from_line( char bag[n_toy_types+1], char *line );
#endif /* __BAGUTILS_H__ */
