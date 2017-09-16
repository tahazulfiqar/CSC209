#ifndef WORD_CHECKER_H
#define WORD_CHECKER_H

int guess_in_board(char **board, char *guess);
int recursive_check(char **board, char *guess, char prev_vacant[4][4],
	int i, int j, int ind);

#endif
