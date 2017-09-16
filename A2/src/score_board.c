#include "score_board.h"

// Creates a linked list from data on file.
User * create_score_board() {
	// Open score file: reading.
	FILE *scorefile = fopen(SCOREFILE_NAME, "rb");
	if (scorefile == NULL) {
		perror(SCOREFILE_NAME);
		exit(1);
	}
	
	User u;
	User *temp, *listhead = NULL;
	// Read data in from file and create a linked list to store the data.
	while (fread(&u, sizeof(User) - sizeof(User *), 1, scorefile) == 1) {
		// Create the first node.
		if (listhead == NULL)
			listhead = create_user_node(u);
		// Add any other node into the list.
		else {
			// Add new nodes to the back of list to preserve order.
			temp = listhead;
			while (temp->next != NULL)
				temp = temp->next;
			temp->next = create_user_node(u);
		}
	}
	// Trigger if fread produces an error. Skip if hit EOF.
	if (ferror(scorefile)) {
		perror(SCOREFILE_NAME);
		exit(1);
	}

	if (fclose(scorefile) != 0) {
		perror(SCOREFILE_NAME);
		exit(1);
	}

	return listhead;
}

// Prints the data of each user from linked link.
void print_score_board(User *list) {
	User *u_ptr = list;

	if (u_ptr == NULL)
		fprintf(stdout, "Score board is empty.\n\n");

	// Iterate through linked list and print the stored info.
	while(u_ptr != NULL) {
		fprintf(stdout, "Username: %s\n", u_ptr->user);
		fprintf(stdout, "Max Score: %d\n", u_ptr->max_score);
		fprintf(stdout, "Total Score: %d\n", u_ptr->total_score);
		fprintf(stdout, "Total # of Games Played: %d\n\n", u_ptr->total_games);
		u_ptr = u_ptr->next;
	}
}

// Updates the info on a given user with the input username. If the user does
// not exist on the list, create a new user node.
void update_score_board(User **list_p, int score, const char *name) {
	User *u_ptr = *list_p, *prev_ptr = *list_p;
	// Iterate through linked list to find a user who has the input username.
	// If found, update the user info and return.
	while (u_ptr != NULL) {
		if (strcmp(name, u_ptr->user) == 0) {
			update_user(u_ptr, score);
			return;
		}
		prev_ptr = u_ptr;
		u_ptr = u_ptr->next;
	}

	// If user not found, create new user node.
	User u;
	strncpy(u.user, name, NAME_MAX_LEN);
	(u.user)[NAME_MAX_LEN - 1] = '\0';
	u.max_score = score;
	u.total_score = score;
	u.total_games = 1;
	u.next = NULL;
	// If linked list is empty, create the first node.
	if (*list_p == NULL)
		*list_p = create_user_node(u);
	// Else, add new node to the end of list.
	else
		prev_ptr->next = create_user_node(u);
}

// Saves data on linked list to file.
void save_score_board(User *list){
	// Open score file: overwriting.
	FILE *scorefile = fopen(SCOREFILE_NAME, "wb");
	if (scorefile == NULL) {
		perror(SCOREFILE_NAME);
		exit(1);
	}

	User *u_ptr = list;
	// Iterate through linked list and write the info in each node to file.
	while (u_ptr != NULL) {
		if (fwrite(u_ptr, sizeof(User) - sizeof(User *), 1, scorefile) != 1) {
			perror(SCOREFILE_NAME);
			exit(1);
		}
		u_ptr = u_ptr->next;
	}

	if (fclose(scorefile) != 0) {
		perror(SCOREFILE_NAME);
		exit(1);
	}
}

// Creates a new user node stored in the heap. The 'next' node pointer points
// to NULL.
User * create_user_node(User u) {
	User *node_ptr = malloc(sizeof(User));
	strncpy(node_ptr->user, u.user, NAME_MAX_LEN);
	node_ptr->user[NAME_MAX_LEN - 1] = '\0';
	node_ptr->max_score = u.max_score;
	node_ptr->total_score = u.total_score;
	node_ptr->total_games = u.total_games;
	node_ptr->next = NULL;
	return node_ptr;
}

// Updates an existing node with the given name.
void update_user(User *u_ptr, int score) {
	u_ptr->total_games++;
	u_ptr->total_score += score;
	if (score > u_ptr->max_score)
		u_ptr->max_score = score;
}

// Frees the whole linked list pointed to by the given pointer.
void free_score_board(User *list) {
	User *u_ptr = list, *next_ptr;
	while (u_ptr != NULL) {
		next_ptr = u_ptr->next;
		free(u_ptr);
		u_ptr = next_ptr;
	}
}
