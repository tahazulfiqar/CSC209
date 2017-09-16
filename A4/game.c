#include "game.h"



/*
* Create a new player with the given name.  Insert it at the tail of the list
* of players whose head is pointed to by *player_ptr_add.
*
* Return:
*   - 0 on success.
*   - 1 if a player by this name already exists in this list.
*   - 2 if the given name cannot fit in the 'name' array
*       (don't forget about the null terminator).
*/
int add_player(Player **p_ptr, const char *name, Player **player_ptr_add) {
	if (strlen(name) > MAX_NAME - 1) {

		char new_name[MAX_NAME+1];

		strncpy(new_name, name, MAX_NAME);

		add_player(p_ptr, new_name, player_ptr_add);

		return 2;
	}

	Player *new_player = malloc(sizeof(Player));
	if (new_player == NULL) {
		perror("malloc");
		exit(1);
	}
	strncpy(new_player->name, name, MAX_NAME - 1);
	new_player->total_games = 0;
	new_player->total_score = 0;
	new_player->max_score = 0;
	new_player->next = NULL;

	// Add player to the end of the list
	Player *prev = NULL;
	Player *curr = *player_ptr_add;
	while (curr != NULL && strcmp(curr->name, name) != 0) {
		prev = curr;
		curr = curr->next;
	}

	if (prev == NULL) {
		*player_ptr_add = new_player;
		*p_ptr = new_player;
		return 0;
	}
	else if (curr != NULL) { //already exists
		*p_ptr = curr;
		free(new_player);
		return 1;
	}
	else {
		prev->next = new_player;
		*p_ptr = new_player;
		return 0;
	}
}


/*
* Return a pointer to the player with this name in
* the list starting with head. Return NULL if no such player exists.
*
* NOTE: You'll likely need to cast a (const Player *) to a (Player *)
* to satisfy the prototype without warnings.
*/
Player *find_player(const char *name, const Player *head) {
	/*    const Player *curr = head;
	while (curr != NULL && strcmp(name, curr->name) != 0) {
	curr = curr->next;
	}

	return (Player *)curr;
	*/
	while (head != NULL && strcmp(name, head->name) != 0) {
		head = head->next;
	}

	return (Player *)head;
}


/*
* Writes the player names and stats of all players in the linked list.
*/
void all_players(int fd, const Player *curr) {
	char buf[BUFFER_SIZE];
	strcpy(buf, "- Listing all players:\r\n");
	if (write(fd, buf, strlen(buf) + 1) != strlen(buf) + 1) {
		perror("write");
		exit(1);
	}
	while (curr != NULL) {
		strcpy(buf, "---------------------------------------------\r\n");
		if (write(fd, buf, strlen(buf) + 1) != strlen(buf) + 1) {
			perror("write");
			exit(1);
		}
		write_player(fd, curr);
		curr = curr->next;
	}
}



/*
* Write a player's stats to client.*
*   - 0 on success.
*   - 1 if the player is NULL.
*/
int write_player(int fd, const Player *p) {
	if (p == NULL) {
		return 1;
	}

	// Write name to client.
	char buf[BUFFER_SIZE];
	sprintf(buf, "- Name: %s\r\n", p->name);
	if (write(fd, buf, strlen(buf) + 1) != strlen(buf) + 1) {
		perror("write");
		exit(1);
	}
	// Write player stats to client.
	sprintf(buf, "- Total games: %d, Total points: %d, Best score: %d\r\n", p->total_games, p->total_score, p->max_score);
	if (write(fd, buf, strlen(buf) + 1) != strlen(buf) + 1) {
		perror("write");
		exit(1);
	}

	return 0;
}

/*
* Finds the player with the given name and updates the player's record.
* Reorders the linked list to preserve decreasing order of max_score.
*
* Return:
*   - 0 on success
*   - 1 if player is not in the list
*/
int add_score(char *name, int score, Player **player_list_ptr) {
	Player *p = find_player(name, *player_list_ptr);
	// Player not found.
	if (p == NULL) {
		return 1;
	}
	// Update p's record.
	p->total_games++;
	p->total_score += score;
	if (p->max_score < score)
		p->max_score = score;
	// Find the player preceding p.
	Player *prev = *player_list_ptr;
	while (prev->next != NULL && prev->next != p)
		prev = prev->next;

	Player *before = *player_list_ptr;
	Player *after = *player_list_ptr;
	// Store the player with the greatest max_score <= p's max_score in after.
	// Store the player with the least max_score > p's max_score in before.
	while (after != NULL && after->max_score > p->max_score) {
		before = after;
		after = after->next;
	}

	// p is in the correct place already. Do nothing.
	if (after == p)
		;
	// p must be moved to the list head.
	else if (after == before) {
		prev->next = p->next;
		p->next = after;
		*player_list_ptr = p;
	}
	else {
		prev->next = p->next;
		p->next = after;
		before->next = p;
	}

	return 0;
}

/*
* Writes the game board to the client whose socket fd is given.
*/
void write_board(int fd) {
	int i = 0, j = 0;
	char buf[3];
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			sprintf(buf, "%c ", board[i][j]);
			if (write(fd, buf, strlen(buf) + 1) != strlen(buf) + 1) {
				perror("write");
				exit(1);
			}
		}
		strcpy(buf, "\r\n");
		if (write(fd, buf, strlen(buf) + 1) != strlen(buf) + 1) {
			perror("write");
			exit(1);
		}
	}
}

/*
 *	Writes the max_score and names of the 3 players with the greatest
 *	max_score to the client.
 */
void top_3(int fd, Player *player_list) {
	int i;
	Player *p = player_list;
	char buf[BUFFER_SIZE];

	for (i = 0; i < 3; i++) {
		if (p == NULL)
			break;
		sprintf(buf, "Name: %s, Max score: %d\r\n", p->name, p->max_score);
		if (write(fd, buf, strlen(buf) + 1) != strlen(buf) + 1) {
			perror("write");
			exit(1);
		}
		p = p->next;
	}
}
