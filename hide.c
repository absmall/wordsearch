#include <wchar.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "wordsearch_internal.h"
#include "position.h"

typedef struct message_data_t {
	wchar_t *msg;
	int index;
} message_data_t;

void wordsearch_add( word_search_t *ws, char *new_word )
{
	size_t s;
	ws->word_count ++;

	if( ws->word_count == 1 ) {
		ws->words = malloc( sizeof( wchar_t * ) );
	} else {
		ws->words = realloc( ws->words, sizeof( wchar_t * ) * ws->word_count );
	}
	s = mbstowcs(NULL, new_word, 0) + 1;
	ws->words[ ws->word_count - 1 ] = malloc( sizeof(wchar_t) * s );
	mbstowcs(ws->words[ ws->word_count - 1 ], new_word, s);
}

static int compare( const void *p1, const void *p2 )
{
	return wcslen(*(wchar_t **)p2)-wcslen(*(wchar_t **)p1);
}

static int fit_word_score( word_search_t *ws, wchar_t *word, position_t *position )
{
	wchar_t c;
	void *index;
	int i, j, length;
	int score;
	int offset;

	score = 0;
	length = wcslen(word);

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
		c = ((wchar_t *)index)[ position->pos[ j ] + i * position_dimension_direction( position, j )];

		if( c == word[i] ) {
			score ++;
		} else if( c != ' ' ) {
			return -1;
		}
	}

	return score;
}

static void insert_word( word_search_t *ws, wchar_t *word, position_t *position )
{
	void *index;
	int i, j;
	int length;

	length = wcslen(word);

	for( i = 0; i < length; i ++ ) {
		index = ws->letters;
		for( j = 0; j < ws->num_dimensions - 1; j ++ ) {
			index = ((void **)index)[ position->pos[ j ] + i * position_dimension_direction( position, j )];
		}
		((wchar_t *)index)[ position->pos[ j ] + i * position_dimension_direction( position, j )] = word[i];
	}
}

bool fit_word( word_search_t *ws, wchar_t *word )
{
	int fit, best_fit;
	int length;
	position_t position, best_position;

	length = wcslen(word);

	// Start at a random location looking for places to fit the word
	position_create_random( ws, &position);

	best_fit = -1;
	do {
		fit = fit_word_score( ws, word, &position );
		if( fit > best_fit ) {
			if( best_fit >= 0 ) {
				position_free( &best_position );
			}
			best_fit = fit;
			position_copy( &best_position, &position );
			if( best_fit == length ) {
				break;
			}
		}
	} while(position_iterate( &position ) );

	if( best_fit >= 0 ) {
		insert_word( ws, word, &best_position );
		position_free( &best_position );
	}
	
	position_free( &position );

	return best_fit >= 0;
}

void wordsearch_fit( word_search_t *ws )
{
	int i;

	// We don't need strong randomness, reproducibility is better
	srand(0);

	// Sort words by length to fit longest first
	qsort( ws->words, ws->word_count, sizeof(wchar_t *), compare );

	// Fit each word
	for( i = 0; i < ws->word_count; i ++ ) {
		fit_word( ws, ws->words[i] );
	}
}

static void wordsearch_fill(wchar_t *c, void *data)
{
	message_data_t *msg_data = (message_data_t *)data;

	if( *c == ' ' ) {
		*c = msg_data->msg[ msg_data->index ++ ];
	}
}

bool wordsearch_fill_message( word_search_t *ws, char *residue )
{
	int length;
	int filled=0, empty=0;
	wchar_t *wresidue;
	bool ret = true;

	if( residue != NULL ) {
		length = mbstowcs( NULL, residue, strlen( residue ) );
		wresidue = malloc( sizeof( wchar_t ) * (length + 1) );
		if( wresidue != NULL ) {
			mbstowcs( wresidue, residue, strlen( residue ) );
			wordsearch_gather_stats( ws->letters, ws->num_dimensions, ws->dimensions, &filled, &empty );
			if( length == empty ) {
				message_data_t data;
				data.msg = wresidue;
				data.index = 0;
				wordsearch_iterate( ws, wordsearch_fill, NULL, NULL, &data );
			} else {
				printf("Incorrect message length: length is %d but %d is needed.\n", length, empty);
				ret = false;
			}
			free( wresidue );
		}
	}

	return ret;
}

static void fill_garbage(wchar_t *c, void *data)
{
	wchar_t w;
	position_t p;

	word_search_t *ws = (word_search_t *)data;
	position_create_random( ws, &p );

	if( *c == ' ' ) {
		w = *position_at( &p );
		// If we hit an unfilled position, then iterate until we find something
		while( w == ' ' ) {
			if( !position_iterate( &p ) ) {
				// We couldn't find any character? The puzzle is empty!
				w = rand() % sizeof(wchar_t);
				break;
			}
			w = *position_at( &p );
		}
		*c = w;
	}

	position_free( &p );
}

void wordsearch_fill_garbage( word_search_t *ws )
{
	wordsearch_iterate( ws, fill_garbage, NULL, NULL, ws );
}
