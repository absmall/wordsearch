wordsearch: main.o hide.o seek.o util.o position.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
