PORT = 57474
CFLAGS = -DPORT=\$(PORT) -Wall -g -std=c99 -Werror -D_XOPEN_SOURCE

game_local: game_local.o game.o read_line.o client_socs.o board_gen.o
	gcc $(CFLAGS) -o game_local game_local.o game.o read_line.o client_socs.o board_gen.o

game_local.o: game_local.c game.h read_line.h client_socs.h board_gen.h
	gcc $(CFLAGS) -c game_local.c

game.o: game.c game.h
	gcc $(CFLAGS) -c game.c

read_line.o: read_line.c read_line.h
	gcc $(CFLAGS) -c read_line.c

client_socs.o: client_socs.c client_socs.h
	gcc $(CFLAGS) -c client_socs.c

board_gen.o: board_gen.c board_gen.h
	gcc $(CFLAGS) -c board_gen.c

clean: 
	rm game_local *.o
