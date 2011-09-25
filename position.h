#ifndef __POSITION_H__
#define __POSITION_H_

#include <stdbool.h>
#include "wordsearch.h"

typedef struct {
	int *start_pos;
	int start_direction;
	int *pos;
	int direction;
	word_search *ws;
} position_t;

void position_create_random( word_search *ws, position_t *position );
bool position_iterate( position_t *position );
void position_free( position_t *position );
int position_dimension_direction( position_t *position, int dimension );

#endif /* __POSITION_H__ */
