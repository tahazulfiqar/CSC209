#ifndef DIR_SEARCH_H
#define DIR_SEARCH_H

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

int count_files(char *path);
char ** filename_array(char *path, int *size_ptr);
void free_filename_array(char **arr, int size);

#endif
