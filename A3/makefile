CC = gcc
CFLAGS = -I -Wall -std=gnu99 -g
DEPS = dir_search.h text_count.h
OBJ = dir_search.o text_count.o phist.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

phist: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f *.o
