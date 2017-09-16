CC = gcc
CFLAGS = -I$(IDIR) -Wall -std=c99 -g

IDIR = ../include
ODIR = obj

_DEPS = board_gen.h dictionary.h score_board.h word_checker.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = board_gen.o dictionary.o score_board.o word_checker.o boggle.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

boggle: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
