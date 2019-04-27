wordsearch: main.o hide.o seek.o util.o position.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

puzzle:
	./wordsearch -d 2 -s 8 < words
