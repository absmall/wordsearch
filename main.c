#include <stdio.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "wordsearch.h"

int dimensions = 2;
int size = 20;

void usage(char *progname)
{
	fprintf(stderr, "Usage: %s [options] wordlist\n", progname);
	fprintf(stderr, "\n");
	fprintf(stderr, "-d dim\tSpecify the number of dimensions in the puzzle\n");
	fprintf(stderr, "-s size\tSpecify the size of each dimension\n");
	exit(1);
}

void read_words(FILE *f, word_search_t *w)
{
	char buf[200];

	while(1) {
		// Read from stdin
		fgets(buf, 200, f);
		if( feof(f) ) break;
		// Chomp newline
		while( isspace( buf[ strlen(buf) - 1 ] ) ) buf[ strlen(buf) - 1 ] = '\0';
		wordsearch_add( w, buf);
	}
}

int main(int argc, char *argv[])
{
	int i;
	int c;
	FILE *f;
	word_search_t w;

	setlocale(LC_ALL, "");

	// Default to 2 dimensions, 20x20
	while ((c = getopt(argc, argv, "d:s:h")) != -1) {
		switch(c) {
			case 'd':
				dimensions = atoi(optarg);
				break;
			case 's':
				size = atoi(optarg);
				break;
			case 'h':
				usage(argv[0]);
				break;
			default:
				break;
		}
	}

	wordsearch_alloc( &w, dimensions, size );

	if( optind < argc ) {
		for( i = optind; i < argc; i ++ ) {
			f = fopen(argv[i], "r");
			if( f == NULL ) {
				fprintf(stderr, "Failed to open %s\n", argv[i]);
				exit(1);
			}
			read_words(f, &w);
			fclose(f);
		}
	} else {
		read_words(stdin, &w);
	}

	wordsearch_fit( &w );
	wordsearch_display( &w );
	wordsearch_solve( &w );
	wordsearch_free( &w );
	return 0;
}
