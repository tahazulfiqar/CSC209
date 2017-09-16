#ifndef SCORE_BOARD_H
#define SCORE_BOARD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SCOREFILE_NAME "score_file"
#define NAME_MAX_LEN 20

typedef struct user {
	char user[NAME_MAX_LEN];
	int max_score;
	int total_score;
	int total_games;
	struct user *next;
} User;

User * create_score_board();
void print_score_board(User *list);
void update_score_board(User **list_p, int score, const char *name);
void save_score_board(User *list);
User *create_user_node(User u);
void update_user(User *u_ptr, int score);
void free_score_board(User *list);

#endif
