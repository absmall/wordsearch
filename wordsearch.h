#ifndef __WORDSEARCH_H__
#define __WORDSEARCH_H__

#include <stdlib.h>

typedef struct word_search_t
{
	int num_dimensions;
	int *dimensions;
	void *letters; 
	wchar_t **words;
	int word_count;
	int max_directions;
} word_search_t;

void wordsearch_alloc( word_search_t *ws, int dimensions, int max);
void wordsearch_add( word_search_t *ws, char *new_word);
void wordsearch_fit( word_search_t *ws, char *residue );
void wordsearch_solve( word_search_t *ws );
void wordsearch_free( word_search_t *ws );
void wordsearch_iterate( word_search_t *ws, void (*char_callback)(wchar_t *c, void *data), void (*dim_callback_entry)(int dim, void *data), void (*dim_callback_exit)(int dim, void *data), void *data );
void wordsearch_display_stats( word_search_t *ws );

#endif /* __WORDSEARCH_H__ */
