#ifndef __WORDSEARCH_H__
#define __WORDSEARCH_H__

typedef struct word_search
{
	int num_dimensions;
	int *dimensions;
	void *letters; 
	char **words;
	int word_count;
	int max_directions;
} word_search;

void wordsearch_alloc( word_search *ws, int dimensions, int max);
void wordsearch_add( word_search *ws, char *new_word);
void wordsearch_fit( word_search *ws );
void wordsearch_free( word_search *ws );

#endif /* __WORDSEARCH_H__ */
