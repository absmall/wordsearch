wordsearch: main.o hide.o util.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
