#include <stdlib.h>
#include "wordsearch.h"
#include "position.h"

void position_create_random( word_search *ws, position_t *position )
{
	int i;

	position->pos = malloc( sizeof(int) * ws->num_dimensions);
	position->start_pos = malloc( sizeof(int) * ws->num_dimensions);
	for( i = 0; i < ws->num_dimensions; i ++ ) {
		position->start_pos[i]
			= position->pos[i]
			= rand() % ws->dimensions[ i ];
	}
	position->start_direction
		= position->direction
		= rand() % ws->max_directions;
	position->ws = ws;
}

bool position_iterate( position_t *position )
{
	int i;

	position->direction = (position->direction + 1) % position->ws->max_directions;
	if( position->direction != position->start_direction ) return true;

	for( i = 0; i < position->ws->num_dimensions; i ++ ) {
		position->pos[ i ] = (position->pos[ i ] + 1) % position->ws->dimensions[ i ];
		if( position->pos[ i ] != position->start_pos[ i ] ) return true;
	}

	return false;
}

void position_free( position_t *position )
{
	free( position->pos );
	free( position->start_pos );
}

int position_dimension_direction( position_t *position, int dimension )
{
	int i;
	int divisor;

	// To see which direction to look, add 1 (because no movement in all
	// dimensions in not valid), then get the number in base 3. Look at the
	// particular digit to get direction, then map 2 to -1 to move 0,1,2 to
	// -1,0,1
	divisor = 1;
	for( i = 0; i < dimension; i ++ ) {
		divisor *= 3;
	}
	i = ((position->direction + 1) / divisor) % 3;
	if( i == 2 ) {
		return -1;
	} else {
		return i;
	}
}


