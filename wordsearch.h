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
void wordsearch_fit( word_search_t *ws );
void wordsearch_solve( word_search_t *ws );
void wordsearch_free( word_search_t *ws );

#endif /* __WORDSEARCH_H__ */
