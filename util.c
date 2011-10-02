#include <stdio.h>
#include <stdlib.h>
#include "wordsearch.h"

static void wordsearch_iterate_helper( void *letters, int dimension, int *dims, void (*char_callback)(wchar_t *c, void *data), void (*dim_callback_entry)(int dim, void *data), void (*dim_callback_exit)(int dim, void *data), void *data )
{
	int i;

	if( dim_callback_entry ) {
		dim_callback_entry( dimension, data );
	}

	if( dimension == 1 ) {
		wchar_t *c = (wchar_t *)letters;

		for( i = 0; i < *dims; i ++ ) {
			char_callback( c + i, data );
		}

	} else {
		void **l = (void **)letters;

		for( i = 0; i < *dims; i ++ ) {
			wordsearch_iterate_helper( l[ i ], dimension - 1, dims + 1, char_callback, dim_callback_entry, dim_callback_exit, data );
		}
	}
	if( dim_callback_exit ) {
		dim_callback_exit( dimension, data );
	}
}

static void display_dimension( void *letters, int dimension, int *dims )
{
	int i;

	if( dimension == 1 ) {
		wchar_t *c = (wchar_t *)letters;
		printf("\t|");
		for( i = 0; i < *dims; i ++ ) {
			printf("%lc", c[i]);
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

void wordsearch_iterate( word_search_t *ws, void (*char_callback)(wchar_t *c, void *data), void (*dim_callback_entry)(int dim, void *data), void (*dim_callback_exit)(int dim, void *data), void *data )
{
	wordsearch_iterate_helper( ws->letters, ws->num_dimensions, ws->dimensions, char_callback, dim_callback_entry, dim_callback_exit, data );
}

void wordsearch_gather_stats( void *letters, int dimension, int *dims, int *filled, int *empty )
{
	int i;

	if( dimension == 1 ) {
		wchar_t *c = (wchar_t *)letters;
		for( i = 0; i < *dims; i ++ ) {
			if( c[i] == ' ' ) {
				(*empty) ++;
			} else {
				(*filled) ++;
			}
		}
	} else {
		void **l = (void **)letters;
		for( i = 0; i < *dims; i ++ ) {
			wordsearch_gather_stats( l[ i ], dimension - 1, dims + 1, filled, empty );
		}
	}
}

void wordsearch_display_stats( word_search_t *ws )
{
	int filled, empty;

	filled = 0;
	empty = 0;

	wordsearch_gather_stats( ws->letters, ws->num_dimensions, ws->dimensions, &filled, &empty );

	printf("Filled %d, empty %d (Density %0.2f%%)\n\n", filled, empty, 100.0 * filled / (filled + empty));
}
