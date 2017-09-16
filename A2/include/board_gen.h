#ifndef BOARD_GEN_H
#define BOARD_GEN_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

char die_roll(char *die);
void randomize(char *a);
char** gameplay();
char** generate_2d_array(char *a);
void print_2d_array(char **a);
void free_2d_array(char **a);

#endif
