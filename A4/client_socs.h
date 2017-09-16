#ifndef CLIENT_SOCS_H
#define CLIENT_SOCS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "read_line.h"
#include "game.h"
#define LISTEN_QUEUE_LEN 5
#define MESSAGE_MAX_LEN 256

// Client struct taken from Alan Rosenthal's sample server.
typedef struct client {
	int fd;
	struct in_addr ipaddr;
	struct client *next;
	struct player *user;

	// gamestate:
	// 0 -> Need new_game, cannot add_score.
	// 1 -> Need add_score, cannot new_game.
	// 2 -> Writing username.
	char gamestate;
} Client;

int socsetup(int port);
int newconnection(int listenfd, Client **clist_ptr);
int whatsup(Client *p, char *cmd_buf, int buf_size, Client **clist_ptr);
void addclient(int fd, struct in_addr addr, Client **clist_ptr);
void removeclient(int fd, Client **clist_ptr);

#endif
