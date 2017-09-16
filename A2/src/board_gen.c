#include "board_gen.h"

char** gameplay() {

	//Declare all the valid dice with respective chars
	char die0[] = "RIFOBX";
	char die1[] = "IFEHEY";
	char die2[] = "DENOWS";
	char die3[] = "UTOKND";
	char die4[] = "HMSRAO";
	char die5[] = "LUPETS";
	char die6[] = "ACITOA";
	char die7[] = "YLGKUE";
	char die8[] = "QBMJOA";
	char die9[] = "EHISPN";
	char die10[] = "VETIGN";
	char die11[] = "BALIYT";
	char die12[] = "EZAVND";
	char die13[] = "RALESC";
	char die14[] = "UWILRG";
	char die15[] = "PACEMD";

	char die_rolls[] = { die_roll(die0), die_roll(die1), die_roll(die2), die_roll(die3), die_roll(die4),
		die_roll(die5), die_roll(die6), die_roll(die7), die_roll(die8),
		die_roll(die9), die_roll(die10), die_roll(die11), die_roll(die12),
		die_roll(die13), die_roll(die14), die_roll(die15) };


	randomize(die_rolls);

	char** game_board;
	game_board = generate_2d_array(die_rolls);

	return game_board;
}



//Function to simulate a roll for a single die.
char die_roll(char *die) {
	int r6 = rand() % 6;
	return die[r6];
}

void randomize(char *a) {
	int i, j, rand_ind;
	char temp;

	for (i = 0; i < 15; i++) {
		// Choose a random index in the right portion of the char array.
		rand_ind = i + (rand() % (16 - i));
		// Store the value at the chosen index in a temporary variable.
		temp = a[rand_ind];
		// From right to left, starting from the element immediately left of
		// the chosen element, shift all values one position to the right.
		for (j = rand_ind - 1; j >= i; j--) {
			a[j + 1] = a[j];
		}
		a[i] = temp;

	}
}

//Function to convert to 2-d array.
char** generate_2d_array(char *a) {

	//Allocate memory for the 4x4 grid.
	char ** game_board;

	game_board = (char **)malloc(4 * sizeof(char *));

	int i;

	for (i = 0; i<4; i++)
		game_board[i] = (char *)malloc(4 * sizeof(char));

	//Transfer from 1-D to 2-D, 4x4 grid
	int b = 0;
	int j;
	int k;

	for (j = 0; j < 4; j++) {
		for (k = 0; k < 4; k++) {
			game_board[j][k] = a[b];
			b++;
		}
	}

	return game_board;

}

void print_2d_array(char **a) {
	int c, d;

	for (c = 0; c < 4; c++) {
		for (d = 0; d < 4; d++) {
			printf("%c ", a[c][d]);
		}
		printf("\n");
	}

}

//Free memory for the 4x4 grid.
void free_2d_array(char **a) {

	int i;

	for (i = 0; i < 4; i++)
		free(a[i]);

	free(a);

}
