#include "dir_search.h"

// Count the number of files in the directory with the given path. Excludes
// .. and . inodes.
int count_files(char *path) {

	int count = 0;
	DIR *d = opendir(path);
	struct dirent *dir;

	// If the directory at the path is found, increment count for each file
	// found within it.
	if (d) {
		while ((dir = readdir(d)) != NULL)
			// Do not count .. and . as files.
			if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0
				&& strcmp(dir->d_name, ".svn") != 0)
				count++;
		closedir(d);
	}
	else {
		perror("opendir");
		exit(1);
	}

	return count;
}

// Allocates an array of strings to hold the names of all files in the
// directory at the given path. Returns a pointer to the array and records the
// array's size in the int pointed to by size_ptr.
char ** filename_array(char *path, int *size_ptr) {
	*size_ptr = count_files(path);
	char **arr = malloc(*size_ptr * sizeof(char *));
	if (arr == NULL) {
		perror("malloc");
		exit(1);
	}

	int i = 0;
	DIR *d = opendir(path);
	struct dirent *dir;

	if (d) {
		while ((dir = readdir(d)) != NULL)
			if (strcmp(dir->d_name, ".") != 0 &&
				strcmp(dir->d_name, "..") != 0 &&
				strcmp(dir->d_name, ".svn") != 0) {
				char *name = malloc((strlen(dir->d_name) + 1) * sizeof(char));
				if (name == NULL) {
					perror("malloc");
					exit(1);
				}
				strcpy(name, dir->d_name);
				arr[i] = name;
				i++;
			}

		closedir(d);
	}
	else {
		perror("opendir");
		exit(1);
	}

	return arr;
}

// Frees the input array of file names. Requires the array's size.
void free_filename_array(char **arr, int size) {
	int i = 0;
	while (i < size) {
		free(arr[i]);
		i++;
	}
	free(arr);
}
