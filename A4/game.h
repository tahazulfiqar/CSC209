#ifndef GAME_H
#define GAME_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>

#define BUFFER_SIZE 256
#define MAX_NAME 10     // Max playername 

char **board;

//Node for linked list which stores all players
typedef struct player {
	char name[MAX_NAME];  
	int max_score;
	int total_games;
	int total_score; 
	struct player *next;
} Player;


/*
 * Create a new player with the given name.  Insert it at the tail of the list
 * of players whose head is pointed to by *player_ptr_add.
 *
 * Return:
 *   0 if successful
 *   1 if a player by this name already exists in this list
 *   2 if the given name cannot fit in the 'name' array
 *       (don't forget about the null terminator)
 */
int add_player(Player **p_ptr, const char *name, Player **user_ptr_add);


/*
 * Return a pointer to the player with this name in
 * the list starting with head. Return NULL if no such user exists.
 */
Player *find_player(const char *name, const Player *head);


/*
* Writes the player names and stats of all players in the linked list.
*/
void all_players(int fd, const Player *curr);



/*
 * Writes the game board to the client whose socket fd is given.
 */

void write_board (int fd);

/*
*  Write a player's stats to client.*
*   - 0 on success.
*   - 1 if the player is NULL.
*/
int write_player(int fd, const Player *player);


/*
 * Finds and deletes a player from the list
 *
 * Return:
 *   - 0 on success
 *   - 1 if player is not in the list
 */
int delete_player(char *name);

/*
 * Finds the player with the given name and updates the player's record.
 * Reorders the linked list to preserve decreasing order of max_score.
 *
 * Return:
 *   - 0 on success
 *   - 1 if player is not in the list
 */
int add_score(char *name, int score, Player **player_list_ptr);

/*
*	Writes the max_score and names of the 3 players with the greatest
*	max_score to the client.
*/
void top_3(int fd, Player *player_list);
#endif
