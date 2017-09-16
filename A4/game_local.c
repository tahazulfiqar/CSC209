#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include "game.h"
#include "client_socs.h"
#include "board_gen.h"

#ifndef PORT
#define PORT 57473
#endif

#define GAME_TIMER 120
#define INPUT_ARG_MAX_NUM 12
#define DELIM " \r\n"

extern char **board;
Client *client_list;

/*
 *	SIGALRM handler that generates a new game board every 2 mins.
 */
void gen_new_board(int sig) {
	fprintf(stdout, "> Interrupting execution to generate new board.\n");
	Client *c_ptr = client_list;
	char game_over_str[] = "****Current game is over****\r\n> ";

	// Print game over message to each connected client.
	while (c_ptr != NULL) {
		if (write(c_ptr->fd, game_over_str, strlen(game_over_str) + 1) != strlen(game_over_str) + 1) {
			perror("write");
			exit(1);
		}
		c_ptr = c_ptr->next;
	}

	// Free previous board.
	if(board != NULL)
		free_2d_array(board);

	// Generate new board.
	board = gameplay();

	// Set timer for next board change.
	alarm(GAME_TIMER);
}

/*
 *	Writes an error message to the client with the given fd.
 */
void error(int fd, char *msg) {
	char buf[MESSAGE_MAX_LEN];
	sprintf(buf, "- Error: %s\r\n", msg);
	if (write(fd, buf, strlen(buf) + 1) != strlen(buf) + 1) {
		perror("write");
		exit(1);
	}
}

/*
* Read and process commands
* Return:  -1 for quit command
*          0 otherwise
*/
int process_args(Client *c_ptr, int cmd_argc, char **cmd_argv, Player **player_list_ptr) {

	Player *player_list = *player_list_ptr;

	// Code adjusted for commands on the interactive server.
	if (cmd_argc <= 0)
		return 0;
	else if (strcmp(cmd_argv[0], "all_players") == 0 && cmd_argc == 1)
		all_players(c_ptr->fd, player_list);
	else if (strcmp(cmd_argv[0], "top_3") == 0 && cmd_argc == 1)
		top_3(c_ptr->fd, player_list);
	else if (strcmp(cmd_argv[0], "add_score") == 0 && cmd_argc == 2) {
		if(c_ptr->gamestate == 0)
			error(c_ptr->fd, "Must start new game before submitting score.");
		else {
			if (add_score(c_ptr->user->name, atoi(cmd_argv[1]), player_list_ptr) == 1)
				error(c_ptr->fd, "Player not found.");
			else
				c_ptr->gamestate = 0;
		}
	}
	else if (strcmp(cmd_argv[0], "new_game") == 0 && cmd_argc == 1) {
		if(c_ptr->gamestate == 1)
			error(c_ptr->fd, "Must enter score before starting new game.");
		else {
			write_board(c_ptr->fd);
			c_ptr->gamestate = 1;
		}
	}
	else if (strcmp(cmd_argv[0], "quit") == 0 && cmd_argc == 1)
		return -1;
	else
		error(c_ptr->fd, "Incorrect syntax or unknown command.");

	/* GIVEN CODE: Batch mode.
	Player *player_list = *player_list_ptr;

	if (cmd_argc <= 0) {
	return 0;
	} else if (strcmp(cmd_argv[0], "quit") == 0 && cmd_argc == 1) {
	return -1;
	} else if (strcmp(cmd_argv[0], "add_player") == 0 && cmd_argc == 2) {
	switch (add_player(cmd_argv[1], player_list_ptr)) {
	case 1:
	error("player by this name already exists");
	break;
	case 2:
	error("playername is too long");
	break;
	}
	} else if (strcmp(cmd_argv[0], "all_players") == 0 && cmd_argc == 1) {
	list_players(player_list);
	} else if (strcmp(cmd_argv[0], "print_player") == 0 && cmd_argc == 2) {
	Player * player = find_player (cmd_argv[1], player_list );
	if (print_player(player) == 1) {
	error("player not found");
	}
	} else if (strcmp(cmd_argv[0], "add_score") == 0 && cmd_argc >= 3) {
	if (add_score(cmd_argv[1], atoi(cmd_argv[2]),player_list) == 1) {
	error("player not found");
	}
	} else if (strcmp(cmd_argv[0], "new_game") == 0 && cmd_argc == 2) {

	print_board ();
	} else {
	error("Incorrect syntax");
	}
	*/

	return 0;
}


/*
* Tokenize the string stored in cmd.
* Return the number of tokens, and store the tokens in cmd_argv.
*/
int tokenize(int fd, char *cmd, char **cmd_argv) {
	int cmd_argc = 0;
	char *next_token = strtok(cmd, DELIM);
	while (next_token != NULL) {
		if (cmd_argc >= INPUT_ARG_MAX_NUM - 1) {
			error(fd, "- Too many arguments!");
			cmd_argc = 0;
			break;
		}
		cmd_argv[cmd_argc] = next_token;
		cmd_argc++;
		next_token = strtok(NULL, DELIM);
	}

	return cmd_argc;
}

void user_message(Client *c_ptr, int n, char* user) {

	if (n == 0) {

		char user_name[256];

		strcpy(user_name, user);

		char final_message[256];

		strcpy(final_message, "- Welcome to Boggle, ");

		char endline[] = "!\r\n";

		strcat(user_name, endline);

		strcat(final_message, user_name);

		//Write back to the client that they've welcomed a new user.
		if (write(c_ptr->fd, final_message, strlen(final_message) + 1) != strlen(final_message) + 1) {
			perror("write");
			exit(1);
		}
	}

	//Case when the entered username is too long. 
	//TODO: FIND A WAY TO RE-PROMPT THE FIRST ARGUMENT -> Set first_prompt back to 1 perhaps?

	else if (n == 2) {

		char user_name[256];

		strcpy(user_name, user);

		char final_message[256];

		sprintf(final_message, "- You can only have a username with %d characters.\r\n Your new username will be: ", MAX_NAME);

		strncpy(final_message, user_name, MAX_NAME);

		//Write back to the client so they receive the message
		if (write(c_ptr->fd, final_message, strlen(final_message) + 1) != strlen(final_message) + 1) {
			perror("write");
			exit(1);
		}
	}

	//Case when user already exists	
	else {

		char user_name[256];

		strcpy(user_name, user);

		char final_message[256];

		strcpy(final_message, "- Welcome back to Boggle, ");

		char endline[] = "!\r\n";

		strcat(user_name, endline);

		strcat(final_message, user_name);

		//Write back to the returning user client. 
		if (write(c_ptr->fd, final_message, strlen(final_message) + 1) != strlen(final_message) + 1) {
			perror("write");
			exit(1);
		}
	}
}


int main(int argc, char* argv[]) {

	board = NULL;
	srand(time(NULL));

	// Set up alarm handler for SIGALRM.
	struct sigaction sa;
	sa.sa_handler = gen_new_board;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGALRM, &sa, NULL) < 0) {
		perror("sigaction");
		exit(1);
	}

	// Initialize board change.
	raise(SIGALRM);

	// Initialize empty linked list of all active clients.
	client_list = NULL;

	// Head of linked list of all users that have connected to this server. 
	Player *player_list = NULL;

	// Setup the listening socket and retrieve its file descriptor.
	int listenfd = socsetup(PORT);

	while (1) {
		// Empty file descriptor set, then add the listening socket fd.
		fd_set fdlist;
		int maxfd = listenfd;
		FD_ZERO(&fdlist);
		FD_SET(listenfd, &fdlist);

		// For each client in the linked list of active clients, add their
		// socket fd to the set.
		Client *c_ptr = client_list;
		while (c_ptr != NULL) {
			FD_SET(c_ptr->fd, &fdlist);
			if (c_ptr->fd > maxfd)
				maxfd = c_ptr->fd;
			c_ptr = c_ptr->next;
		}

		int disc_flag;

		// Use select to extract a set of socket fds that may be read from.
		if (select(maxfd + 1, &fdlist, NULL, NULL, NULL) < 0)
			perror("select");
		else {
			// Find the first socket that may be read from.
			c_ptr = client_list;
			while (c_ptr != NULL && !FD_ISSET(c_ptr->fd, &fdlist))
				c_ptr = c_ptr->next;

			if (c_ptr != NULL && c_ptr->gamestate != 2) {

				// Read from the socket, store the read line into cmd_buf.
				char cmd_buf[BUFFER_SIZE];
				disc_flag = whatsup(c_ptr, cmd_buf, BUFFER_SIZE, &client_list);

				// Divide the read line into string tokens (arguments); store
				// into cmd_argv. Store the number of args into cmd_argc.
				char *cmd_argv[INPUT_ARG_MAX_NUM];
				int cmd_argc = tokenize(c_ptr->fd, cmd_buf, cmd_argv);

				// If the select was not triggered by a client disconnection...
				if (disc_flag == 0) {

					fprintf(stdout, "> Received from client %s: ", inet_ntoa((c_ptr)->ipaddr));
					fprintf(stdout, "%s\n", cmd_buf);

					// Trigger if client quits.
					if (cmd_argc > 0 && process_args(c_ptr, cmd_argc, cmd_argv, &player_list) == -1) {
						// Close client socket, then free client struct.
						fprintf(stdout, "> Disconnection from %s\n", inet_ntoa(c_ptr->ipaddr));
						close(c_ptr->fd);
						removeclient(c_ptr->fd, &client_list);
					}
					// Client-side prompt.
					else {
						char prompt[] = "> ";
						if (write(c_ptr->fd, prompt, strlen(prompt) + 1) != strlen(prompt) + 1) {
							perror("write");
							exit(1);
						}
					}
				}
			}
			// TO DO: Add comments.
			else if (c_ptr != NULL && c_ptr->gamestate == 2) {
				char buf[BUFFER_SIZE];
				disc_flag = whatsup(c_ptr, buf, BUFFER_SIZE, &client_list);
				char *cmd_argv[INPUT_ARG_MAX_NUM];
				tokenize(c_ptr->fd, buf, cmd_argv);

				if (disc_flag == 0) {
					Player *p;
					int user_result;
					user_result = add_player(&p, cmd_argv[0], &player_list);
					c_ptr->user = p;
					c_ptr->gamestate = 0;
					user_message(c_ptr, user_result, cmd_argv[0]);

					// Client-side prompt.
					char prompt[] = "> ";
					if (write(c_ptr->fd, prompt, strlen(prompt) + 1) != strlen(prompt) + 1) {
						perror("write");
						exit(1);
					}
				}
			}

			// Establish new connection with a queued client.
			if (FD_ISSET(listenfd, &fdlist)) {
				int cfd = newconnection(listenfd, &client_list);

				char str[] = "- Please enter your username: ";
				if (write(cfd, str, strlen(str) + 1) != strlen(str) + 1) {
					perror("write");
					exit(1);
				}
			}
			/*
				c_ptr = client_list;
				while (c_ptr != NULL && c_ptr->fd != cfd)
					c_ptr = c_ptr->next;

				char buf[BUFFER_SIZE];
				disc_flag = whatsup(c_ptr, buf, BUFFER_SIZE, NULL);
				char *cmd_argv[INPUT_ARG_MAX_NUM];
				tokenize(c_ptr->fd, buf, cmd_argv);

				if (disc_flag == 0) {
					Player *p;
					int user_result;
					user_result = add_player(&p, cmd_argv[0], &player_list);
					c_ptr->user = p;
					user_message(user_result, cmd_argv[0]);

					// Client-side prompt.
					char prompt[] = "> ";
					if (write(c_ptr->fd, prompt, strlen(prompt) + 1) != strlen(prompt) + 1) {
						perror("write");
						exit(1);
					}
				}*/
			
		}
	}

	/*	GIVEN CODE: Batch mode.
	int batch_mode = (argc == 2);
	char input[INPUT_BUFFER_SIZE];
	FILE *input_stream;

	// Create the heads of the empty data structure
	Player *player_list = NULL;

	if (batch_mode) {
	input_stream = fopen(argv[1], "r");
	if (input_stream == NULL) {
	perror("Error opening file");
	exit(1);
	}
	} else {
	// interactive mode
	input_stream = stdin;
	}

	printf("Welcome to Game server! (Local version)\nPlease type a command: <add_player>, <all_players>, <print_player name>, <add_score name score> or <quit>\n> ");

	while (fgets(input, INPUT_BUFFER_SIZE, input_stream) != NULL) {
	// only echo the line in batch mode since in interactive mode the player
	// just typed the line
	if (batch_mode) {
	printf("%s", input);
	}

	char *cmd_argv[INPUT_ARG_MAX_NUM];
	int cmd_argc = tokenize(input, cmd_argv);

	if (cmd_argc > 0 && process_args(cmd_argc, cmd_argv, &player_list) == -1) {
	break; // can only reach if quit command was entered
	}

	printf("> ");
	}

	if (batch_mode) {
	if (fclose(input_stream) != 0) {
	perror("fclose");
	exit(1);
	}
	}
	*/

	return 0;
}
