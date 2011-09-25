#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "wordsearch.h"
#include "position.h"

static void letters_alloc(void **letters, int dimensions, int *maxes)
{
	int i;
	if( dimensions == 1 ) {
		*letters = malloc( sizeof(char) * *maxes );
		for( i = 0; i < *maxes; i ++ ) {
			((char *)*letters)[ i ] = ' ';
		}
	} else {
		*letters = malloc( sizeof(void *) * *maxes );
		for( i = 0; i < *maxes; i ++ ) {
			letters_alloc( ((void **)*letters) + i, dimensions - 1, maxes + 1 );
		}
	}
}

void wordsearch_alloc( word_search_t *ws, int dimensions, int max)
{
	int i;

	ws->num_dimensions = dimensions;
	ws->dimensions = malloc(dimensions * sizeof(int));
	for(i = 0; i < dimensions; i ++ ) {
		ws->dimensions[ i ] = max;
	}
	letters_alloc(&ws->letters, dimensions, ws->dimensions);
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

void wordsearch_add( word_search_t *ws, char *new_word )
{
	ws->word_count ++;

	if( ws->word_count == 1 ) {
		ws->words = malloc( sizeof( char * ) );
	} else {
		ws->words = realloc( ws->words, sizeof( char * ) * ws->word_count );
	}
	ws->words[ ws->word_count - 1 ] = strdup( new_word );
}

static compare( const void *p1, const void *p2 )
{
	return strlen(*(char **)p2)-strlen(*(char **)p1);
}

static int fit_word_score( word_search_t *ws, char *word, position_t *position )
{
	char c;
	void *index;
	int i, j, length;
	int last_pos;
	int score;
	int offset;

	score = 0;
	length = strlen(word);

	// Make sure the word fits at all. See if the last letter is on the grid
	for( i = 0; i < ws->num_dimensions; i ++ ) {
		offset = position->pos[ i ] + length * position_dimension_direction( position, i );
		if( offset < 0 || offset > ws->dimensions[ i ] ) {
			return -1;
		}
	}

	for( i = 0; i < length; i ++ ) {
		index = ws->letters;
		for( j = 0; j < ws->num_dimensions - 1; j ++ ) {
			index = ((void **)index)[ position->pos[ j ] + i * position_dimension_direction( position, j )];
		}
		c = ((char *)index)[ position->pos[ j ] + i * position_dimension_direction( position, j )];

		if( c == word[i] ) {
			score ++;
		} else if( c != ' ' ) {
			return -1;
		}
	}
}

static void insert_word( word_search_t *ws, char *word, position_t *position )
{
	void *index;
	int i, j;
	int length;

	length = strlen(word);

	for( i = 0; i < length; i ++ ) {
		index = ws->letters;
		for( j = 0; j < ws->num_dimensions - 1; j ++ ) {
			index = ((void **)index)[ position->pos[ j ] + i * position_dimension_direction( position, j )];
		}
		((char *)index)[ position->pos[ j ] + i * position_dimension_direction( position, j )] = word[i];
	}
}

bool fit_word( word_search_t *ws, char *word )
{
	bool fitted = false;
	int fit, best_fit;
	int length;
	int count;
	position_t position, best_position;

	length = strlen(word);

	// Start at a random location looking for places to fit the word
	position_create_random( ws, &position);

	best_fit = -1;
	do {
		fit = fit_word_score( ws, word, &position );
		if( fit > best_fit ) {
			fitted = true;
			best_fit = fit;
			best_position = position;
			if( best_fit == length ) {
				break;
			}
		}
	} while(position_iterate( &position ) );

	if( fitted ) {
		insert_word( ws, word, &position );
	}
	
	position_free( &position );

	return fitted;
}

void wordsearch_fit( word_search_t *ws )
{
	int i;

	// We don't need strong randomness, reproducibility is better
	srand(0);

	// Sort words by length to fit longest first
	qsort( ws->words, ws->word_count, sizeof(char *), compare );

	// Fit each word
	for( i = 0; i < ws->word_count; i ++ ) {
		fit_word( ws, ws->words[i] );
	}
}
