#include "toys.h"
#include "strtools.h"
STRSPLIT_INIT;
STRSPLIT_LENGTH_INIT;
STRSPLIT_FREE_INIT;
#include <stdio.h>

void bag_set_from_line( char bag[n_toy_types+1], char *line )
{
	memset( bag, 0, (n_toy_types + 1) * sizeof(char));
	char **spl = strsplit(strstrip(line), ' ');
	int n_items = strsplit_length( spl );
	for( int i=0; i < n_items; i++ ){
		char toy[10]; /* No toy description is longer than 10 charaters */
		int  id;
		int  ck = sscanf( strstrip(spl[i]), "%10[^_]_%d", toy, &id );
		if(ck!=2) {fprintf(stderr,"%s, Could not parse: %s\n", toy, spl[i]); exit(-1); }
		bag[string_to_toy(toy)]++;
	}
	strsplit_free( spl );
	return;
}

