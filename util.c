#include <stdio.h>
#include "wordsearch.h"

static void display_dimension( void *letters, int dimension, int *dims )
{
	int i;

	if( dimension == 1 ) {
		char *c = (char *)letters;
		printf("\t|");
		for( i = 0; i < *dims; i ++ ) {
			printf("%c", c[i]);
		}
		printf("|\n");
	} else if( dimension == 2 ) {
		int i;
		void **l = (void **)letters;
		printf("\t+");
		for( i = 0; i < dims[ 1 ]; i ++ ) {
			printf("-");
		}
		printf("+\n");
		for( i = 0; i < *dims; i ++ ) {
			display_dimension( l[ i ], dimension - 1, dims + 1 );
		}
		printf("\t+");
		for( i = 0; i < dims[ 1 ]; i ++ ) {
			printf("-");
		}
		printf("+\n");
	} else {
		void **l = (void **)letters;
		for( i = 0; i < *dims; i ++ ) {
			display_dimension( l[ i ], dimension - 1, dims + 1 );
		}
		printf("\n");
	}
}

void wordsearch_display( word_search_t *ws )
{
	display_dimension( ws->letters, ws->num_dimensions, ws->dimensions );
}
