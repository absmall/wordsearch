#ifndef __POSITION_H__
#define __POSITION_H_

#include <stdbool.h>
#include "wordsearch_internal.h"

typedef struct position_t {
	int *start_pos;
	int start_direction;
	int *pos;
	int direction;
	word_search_t *ws;
} position_t;

void position_create_random( word_search_t *ws, position_t *position );
void position_copy( position_t *dest, position_t *src );
bool position_iterate( position_t *position );
void position_free( position_t *position );
int position_dimension_direction( const position_t *position, int dimension );

#endif /* __POSITION_H__ */
