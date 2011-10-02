#include <string.h>
#include <stdio.h>

#include "wordsearch.h"
#include "position.h"

static bool check_word( word_search_t *ws, wchar_t *word, position_t *position )
{
	wchar_t c;
	void *index;
	int i, j, length;
	int offset;

	length = wcslen(word);

	// Make sure the word fits at all. See if the last letter is on the grid
	for( i = 0; i < ws->num_dimensions; i ++ ) {
		offset = position->pos[ i ] + length * position_dimension_direction( position, i );
		if( offset < 0 || offset > ws->dimensions[ i ] ) {
			return false;
		}
	}

	for( i = 0; i < length; i ++ ) {
		index = ws->letters;
		for( j = 0; j < ws->num_dimensions - 1; j ++ ) {
			index = ((void **)index)[ position->pos[ j ] + i * position_dimension_direction( position, j )];
		}
		c = ((wchar_t *)index)[ position->pos[ j ] + i * position_dimension_direction( position, j )];

		if( c != word[i] ) {
			return false;
		}
	}

	return true;
}

void wordsearch_solve(word_search_t *ws)
{
	position_t pos;
	int i, j;

	position_create_random( ws, &pos );

	for( i = 0; i < ws->word_count; i ++ ) {
		do {
			if( check_word( ws, ws->words[i], &pos ) ) {
				printf("Found '%ls' at (", ws->words[i]);
				for(j = 0; j < ws->num_dimensions - 1; j ++ ) {
					printf("%d,", pos.pos[ j ] );
				}
				printf("%d)\n", pos.pos[ ws->num_dimensions - 1 ]);
			}
		} while( position_iterate( &pos ) );
	}

	position_free( &pos );
}
