#include "word_checker.h"

// Calls recursive_check on each entry on the input game board.
int guess_in_board(char **board, char *guess) {

	int row, col, length;

	// Create 2D vacancy array (all entries vacant.)
	char vacant[4][4];
	for (row = 0; row < 4; row++)
		for (col = 0; col < 4; col++)
			vacant[row][col] = 1;

	// Call recursive_check on each entry on the input game board. If the
	// guessed string on the board, returns a non-0 length.
	for (row = 0; row < 4; row++)
		for (col = 0; col < 4; col++) {
			// recursive_check counts the null terminator as a char, therefore
			// the returned length is off by 1.
			length = recursive_check(board, guess, vacant, row, col, 0) - 1;
			if (length > 0)
				return length;
		}
	return 0;
}

// Checks that the current entry (board[i][j]) is the same as 'guess[ind]',
// then calls itself on the adjacent entries on the board.
int recursive_check(char **board, char *guess, char prev_vacant[4][4],
	int i, int j, int ind) {

	// Base case: end of guessed string reach. Therefore whole guessed string
	// was found on the board. Return success (1.)
	if (guess[ind] == '\0')
		return 1;

	// Check that 'board[i][j]' is the same char as 'guess[ind].'
	if (guess[ind] == board[i][j]) {

		int row, col;

		// Copy the input vacancy matrix and set vacant[i][j] to 0 (cannot
		// reuse same entry in same word.)
		char vacant[4][4];
		for (row = 0; row < 4; row++)
			for (col = 0; col < 4; col++)
				vacant[row][col] = prev_vacant[row][col];
		vacant[i][j] = 0;

		// Call recursive_check on each neighbouring entry.
		int p, q, length;
		for (p = -1; p < 2; p++)
			for (q = -1; q < 2; q++) {
				row = i+p;
				col = j+q;
				// Prevents index OOB.
				if (row > -1 && row < 4 && col > -1 && col < 4
					// Ensures that the neighbouring entry can be used (is
					// vacant.)
					&& vacant[row][col] == 1) {
					// Each successful return from recursive_check increments
					// the return length by 1.
					length = recursive_check(board, guess, vacant, row, col,
						ind + 1);
					if (length)
						return length + 1;
					else if (guess[ind] == 'Q' && guess[ind + 1] == 'U') {
						length = recursive_check(board, guess, vacant, row, col,
							ind + 2);
						if (length)
							return length + 1;
					}
				}
			}
	}

	return 0;
}
