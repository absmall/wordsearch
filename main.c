#include <stdio.h>
#include "wordsearch.h"


int main(int argc, char *argv[])
{
	word_search w;

	wordsearch_alloc( &w, 4, 5 );
	wordsearch_add( &w, "hello" );
	wordsearch_fit( &w );
	wordsearch_display( &w );
	wordsearch_free( &w );
	return 0;
}
