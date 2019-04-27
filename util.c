#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "wordsearch_internal.h"

void wordsearch_letters_alloc(void **letters, int dimensions, int *maxes)
{
	int i;
	if( dimensions == 1 ) {
		*letters = malloc( sizeof(wchar_t) * *maxes );
		for( i = 0; i < *maxes; i ++ ) {
			((wchar_t *)*letters)[ i ] = ' ';
		}
	} else {
		*letters = malloc( sizeof(void *) * *maxes );
		for( i = 0; i < *maxes; i ++ ) {
			wordsearch_letters_alloc( ((void **)*letters) + i, dimensions - 1, maxes + 1 );
		}
	}
}

static void wordsearch_letters_copy_helper(void **letters, int dimensions, int *maxes, void *old_letters )
{
	int i;
	if( dimensions == 1 ) {
		*letters = malloc( sizeof(wchar_t) * *maxes );
		for( i = 0; i < *maxes; i ++ ) {
			((wchar_t *)*letters)[ i ] = ((wchar_t *)old_letters)[ i ];
		}
	} else {
		*letters = malloc( sizeof(void *) * *maxes );
		for( i = 0; i < *maxes; i ++ ) {
			wordsearch_letters_copy_helper( ((void **)*letters) + i, dimensions - 1, maxes + 1, ((void **)old_letters)[ i ] );
		}
	}
}

void wordsearch_copy_letters(word_search_t *dest, word_search_t *src)
{
	memset( dest, 0, sizeof(word_search_t) );
	dest->num_dimensions = src->num_dimensions;
	dest->dimensions = malloc( sizeof(int) * src->num_dimensions );
	memcpy( dest->dimensions, src->dimensions, sizeof(int) * src->num_dimensions );
	wordsearch_letters_copy_helper( &dest->letters, src->num_dimensions, src->dimensions, src->letters );
}

void wordsearch_alloc( word_search_t *ws, int dimensions, int max)
{
	int i;

	ws->num_dimensions = dimensions;
	ws->dimensions = malloc(dimensions * sizeof(int));
	for(i = 0; i < dimensions; i ++ ) {
		ws->dimensions[ i ] = max;
	}
	wordsearch_letters_alloc(&ws->letters, dimensions, ws->dimensions);
	ws->word_count = 0;

	// Find the number of possible directions - up, down or no movement in each dimension, but
	// they can't all be no movement
	//
	ws->max_directions = 1;
	for(i = 0; i < ws->num_dimensions; i ++ ) {
		ws->max_directions *= 3;
	}
	ws->max_directions -= 1;
}

void wordsearch_alloc_variable( word_search_t *ws, int dimensions, int *max)
{
	int i;

	ws->num_dimensions = dimensions;
	ws->dimensions = malloc(dimensions * sizeof(int));
	for(i = 0; i < dimensions; i ++ ) {
		ws->dimensions[ i ] = max[i];
	}
	wordsearch_letters_alloc(&ws->letters, dimensions, ws->dimensions);
	ws->word_count = 0;

	// Find the number of possible directions - up, down or no movement in each dimension, but
	// they can't all be no movement
	//
	ws->max_directions = 1;
	for(i = 0; i < ws->num_dimensions; i ++ ) {
		ws->max_directions *= 3;
	}
	ws->max_directions -= 1;
}

static void letters_free( void *letters, int dimensions, int *maxes)
{
	int i;

	if( dimensions == 1 ) {
		free( letters );
	} else {
		for( i = 0; i < *maxes; i ++ ) {
			letters_free( ((void **)letters)[ i ], dimensions - 1, maxes + 1 );
		}
		free( letters );
	}
}

void wordsearch_free( word_search_t *ws )
{
	int i;
	letters_free( ws->letters, ws->num_dimensions, ws->dimensions );
	free( ws->dimensions );

	for( i = 0; i < ws->word_count; i ++ ) {
		free( ws->words[i] );
	}
	if( ws->word_count != 0 ) {
		free( ws->words );
	}
}

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
