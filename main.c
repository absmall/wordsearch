#include <stdio.h>
#include <string.h>
#include "wordsearch.h"


int main(int argc, char *argv[])
{
	char buf[200];
	word_search w;

	// Default to 2 dimensions, 20x20
	wordsearch_alloc( &w, 2, 20 );
	while(1) {
		// Read from stdin
		fgets(buf, 200, stdin);
		if( feof(stdin) ) break;
		// Chomp newline
		while( isspace( buf[ strlen(buf) - 1 ] ) ) buf[ strlen(buf) - 1 ] = '\0';
		wordsearch_add( &w, buf);
	}
	wordsearch_fit( &w );
	wordsearch_display( &w );
	wordsearch_solve( &w );
	wordsearch_free( &w );
	return 0;
}
