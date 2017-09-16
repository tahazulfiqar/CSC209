#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "dictionary.h"
#include "word_checker.h"
#include "score_board.h"
#include "board_gen.h"
#define BUFFER_SIZE 100
#define GUESS_STR_SIZE 17

// Stores a string entered from stdin into s as uppercase chars.
void stdin_upper(char *s, int num) {
	int i, end_index;
	fgets(s, num, stdin);
	end_index = strcspn(s, "\r\n");
	s[end_index] = '\0';
	for (i = 0; i < strlen(s); i++)
		s[i] = toupper(s[i]);

	// Clear out remaining chars in stdin if more then 16 chars are entered.
	if (end_index >= num - 1) {
		char c;
		while ((c = getchar()) != '\n' && c != EOF);
	}
}

int main(int argc, char **argv) {

	srand(time(NULL));

	FILE *i_file = NULL, *o_file = NULL;
	int i;

	// Check command-line arguments.
	if (argc > 2) {
		// Too many parameters
		fprintf(stderr, "Too many parameters.\n Usage: boggle [optional test file]\n");
		return 1;
	}
	else  if (argc == 2) {
		// Test file provided.
		i_file = fopen(argv[1], "r");
		if (i_file == NULL) {
			perror(argv[1]);
			return 1;
		}
		// Open output file.
		o_file = fopen("result.txt", "w");
		if (o_file == NULL) {
			perror("result.txt");
			return 1;
		}
	}

	int score = 0, length = 0;
	DNode **dictionary = load_dictionary();
	static DNode *used_dictionary[SMALL_HASH_SIZE];
	DNode *entry_ptr, *used_ptr;

	// Interactive mode.
	if (i_file == NULL) {
		system("clear");

		// Read in, display, then free score board.
		User *score_list = create_score_board();
		print_score_board(score_list);
		free_score_board(score_list);

		char guess[GUESS_STR_SIZE];
		// Startup prompt.
		fprintf(stdout, "Enter 'N' to begin new session or 'Q' to quit: ");
		stdin_upper(guess, GUESS_STR_SIZE);
		while ((guess[0] != 'Q' && guess[0] != 'N') || guess[1] != '\0') {
			fprintf(stdout, "Please enter either 'N' or 'Q': ");
			stdin_upper(guess, GUESS_STR_SIZE);
		}

		// Loop of one game session.
		while (guess[0] != 'Q') {

			// Reset input buffer 'guess', generate game board, and reset score.
			strcpy(guess, "");
			char **random_board = gameplay();
			score = 0;

			// Loop of each guessed word.
			while (!((guess[0] == 'N' || guess[0] == 'Q') && guess[1] == '\0')) {

				// Clear screen, then display board.
				system("clear");
				print_2d_array(random_board);

				// Check if word in input buffer is valid (ignore empty string.)
				if (strlen(guess) < 3 && strlen(guess) != 0)
					fprintf(stdout, "Word must be at least 3 chars long. Entered '%s'\n", guess);
				else if (strlen(guess) != 0) {
					entry_ptr = lookup(dictionary, BIG_HASH_SIZE, guess);
					used_ptr = lookup(used_dictionary, SMALL_HASH_SIZE,
						guess);
					length = guess_in_board(random_board, guess);
					// Check if word is in dictionary.
					if (entry_ptr == NULL)
						fprintf(stdout, "'%s' not found in dictionary.\n", guess);
					// Check if word is on board.
					else if (length < 3)
						fprintf(stdout, "'%s' not found on game board.\n", guess);
					// Check if word has been entered before.
					else if (used_ptr != NULL)
						fprintf(stdout, "'%s' has already been entered in the current game session.\n", guess);
					// Increment score for a valid word.
					else {
						insert(used_dictionary, SMALL_HASH_SIZE, guess);
						if (length == 3 || length == 4)
							score++;
						else if (length == 5)
							score += 2;
						else if (length == 6)
							score += 3;
						else if (length == 7)
							score += 5;
						else
							score += 11;
						fprintf(stdout, "'%s' is a valid word. Current score: %d\n", guess, score);
					}
				}

				// Prompt for next guess or for 'N'/'Q'.
				fprintf(stdout, "Enter a word (16 chars max). Type 'N' or 'Q' to end session: ");
				stdin_upper(guess, GUESS_STR_SIZE);
			}
			// Free dictionary of used words.
			free_dictionary(used_dictionary, SMALL_HASH_SIZE);

			// Free game board.
			free_2d_array(random_board);

			// Show score, ask for user name.
			system("clear");
			char username[NAME_MAX_LEN];
			fprintf(stdout, "Final score: %d\n", score);
			fprintf(stdout, "Enter user name (19 chars max): ");
			stdin_upper(username, NAME_MAX_LEN);

			// Update score board, save, display, then free.
			score_list = create_score_board();
			update_score_board(&score_list, score, username);
			save_score_board(score_list);
			fprintf(stdout, "\nUpdated score board:\n");
			print_score_board(score_list);
			free_score_board(score_list);

			// Ask for next game 'N' or quit 'Q'.
			fprintf(stdout, "Enter 'N' to begin new session or 'Q' to quit: ");
			stdin_upper(guess, GUESS_STR_SIZE);
			while ((guess[0] != 'Q' && guess[0] != 'N') || guess[1] != '\0') {
				fprintf(stdout, "Please enter either 'N' or 'Q': ");
				stdin_upper(guess, GUESS_STR_SIZE);
			}
		}
		// Game end message.
		fprintf(stdout, "=========  GAME END  =========\n");
	}
	// Test file mode.
	else {
		int comma = 0;

		// Read in whole first line, transfer chars into 16 char array.
		char board_str[18], board_data[16];
		fgets(board_str, 18, i_file);
		for (i = 0; i < 16; i++)
			board_data[i] = board_str[i];

		// Convert 16 char array into 2D array (game board.)
		char **converted_board = generate_2d_array(board_data);

		// Store string of inputs to be entered.
		char guess_buffer[BUFFER_SIZE];
		fgets(guess_buffer, BUFFER_SIZE, i_file);
		guess_buffer[strcspn(guess_buffer, "\r\n")] = '\0';

		char *token = strtok(guess_buffer, ",");
		while (token != NULL) {
			entry_ptr = lookup(dictionary, BIG_HASH_SIZE, token);
			used_ptr = lookup(used_dictionary, SMALL_HASH_SIZE, token);
			length = guess_in_board(converted_board, token);

			// Write invalid entries into output file, increment score if
			// valid.
			if (strlen(token) < 3 || strlen(token) > 16 || entry_ptr == NULL ||
				used_ptr != NULL || length  < 3) {
				// Do not put a comma before the first invalid entry.
				if (comma)
					fprintf(o_file, ",");
				fprintf(o_file, "%s", token);
				comma = 1;
			}
			else {
				insert(used_dictionary, SMALL_HASH_SIZE, token);
				if (length == 3 || length == 4)
					score++;
				else if (length == 5)
					score += 2;
				else if (length == 6)
					score += 3;
				else if (length == 7)
					score += 5;
				else
					score += 11;
			}
			token = strtok(NULL, ",");
		}
		// Print accumulated score.
		fprintf(o_file, "\n%d\n", score);

		// Closes both input and output files.
		if (fclose(i_file) != 0) {
			perror(argv[1]);
			return 1;
		}
		if (fclose(o_file) != 0) {
			perror("result.txt");
			return 1;
		}
	}
	return 0;
}
