#include <ctype.h>
#include <stdio.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "wordsearch.h"

bool garbage = false;
bool no_fit = false;
int dimensions = 2;
int size = 20;
int variablesizesize;
int *variablesize = NULL;
int verbose = 0;

void usage(char *progname)
{
	fprintf(stderr, "Usage: %s [options] wordlist\n", progname);
	fprintf(stderr, "\n");
	fprintf(stderr, "-d dim\tSpecify the number of dimensions in the puzzle\n");
	fprintf(stderr, "-g\tFill the unused space with garbage letters\n");
	fprintf(stderr, "-h\tHelp\n");
	fprintf(stderr, "-m message\tMessage to hide in the unused letters\n");
	fprintf(stderr, "-s size\tSpecify the size of each dimension. This can either be a single integer the size\n");
  fprintf(stderr, "       \tof each dimension, or a set of integers separated by a single letter x for each\n");
  fprintf(stderr, "       \tdimension. In this case, the number of parameters must match the dimension.\n");
	fprintf(stderr, "-n\tDon't fit words, just fill the grid using a message (useful for import)\n");
	fprintf(stderr, "-v\tEnable additional prints\n");
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
	char *message = NULL;
	word_search_t w;

	setlocale(LC_ALL, "");

	// Default to 2 dimensions, 20x20
	while ((c = getopt(argc, argv, "rd:s:hvm:gn")) != -1) {
		switch(c) {
			case 'd':
				dimensions = atoi(optarg);
				break;
			case 's':
        // Size can be either a single integer which specifies all of the dimensions, or it can
        // be a series of integers separated by x's to specify all of the dimensions.
        {
          const char *nextdim = strchr(optarg, 'x'); 
          if(nextdim) {
            // First, count the number of dimensions.
            variablesizesize = 1;
            do {
              nextdim = strchr(nextdim + 1, 'x'); 
              variablesizesize++;
            } while(nextdim != NULL);
            // Allocate the array.
            variablesize = malloc(sizeof(int) * variablesizesize);
            // Fill the array.
            nextdim = optarg;
            int *cursize = variablesize;
            *cursize = 0;
            char *curchar = optarg;
            while(*curchar) {
              if(*curchar=='x') {
                cursize ++;
                *cursize = 0;
              } else if(*curchar >= '0' && *curchar <= '9') {
                *cursize = *cursize * 10 + (*curchar) - '0';
              } else {
                fprintf(stderr, "Invalid size argument: %s\n", optarg);
                usage(argv[0]);
              }
              curchar++;
            }
          } else {
            size = atoi(optarg);
          }
        }
				break;
			case 'h':
				usage(argv[0]);
				break;
			case 'm':
				if( garbage ) {
					fprintf(stderr, "Only one of -g and -m may be specified\n");
					exit(1);
				}
				message = optarg;
				break;
			case 'g':
				if( message != NULL ) {
					fprintf(stderr, "Only one of -g and -m may be specified\n");
					exit(1);
				}
				garbage = true;
				break;
			case 'v':
				verbose = 1;
				break;
			case 'n':
				no_fit = true;
				break;
			default:
				break;
		}
	}

  if(variablesize) {
    if(variablesizesize != dimensions) {
      fprintf(stderr, "Number of sizes does not match number of dimensions: %d vs %d\n", variablesizesize, dimensions);
      usage(argv[0]);
    }
    wordsearch_alloc_variable( &w, dimensions, variablesize);
  } else {
    wordsearch_alloc( &w, dimensions, size );
  }

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

	if( !no_fit ) {
		wordsearch_fit( &w );
	}

	if( message != NULL ) {
		if( !wordsearch_fill_message( &w, message ) ) {
			message = NULL;
		}
	} else if( garbage ) {
		wordsearch_fill_garbage( &w );
	}

	wordsearch_display( &w );
	if( verbose ) {
		wordsearch_display_stats( &w );
	}
	wordsearch_solve( &w, message != NULL );
	wordsearch_free( &w );
  free(variablesize);
	return 0;
}
