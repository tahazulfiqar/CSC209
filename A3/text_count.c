#include "text_count.h"

// Test on words.txt file.
void count_words(int *word_len, char *filename, char *dirname) {
	char fullpath[LINE_MAX_LEN];

	// Concatenate to find full path of the file to be processed.
	if (sprintf(fullpath, "./%s/%s", dirname, filename) < 0) {
		fprintf(stderr, "sprint: Error.");
		exit(1);
	}
	FILE *i_file = fopen(fullpath, "r");
	if (i_file == NULL) {
		perror("fopen");
		exit(1);
	}

	int len;
	char line[LINE_MAX_LEN];
	char *word;
	// Chars that will serve as our word boundaries.
	char delimiter[] = " ,.-!?\"|':;+";
	while (fgets(line, LINE_MAX_LEN, i_file) != NULL) {
		word = strtok(line, delimiter);
		while (word != NULL) {
			len = strlen(word);
			// Ignore words of length 0 or longer than 45 chars.
			if (len > 0 && len < 46)
				word_len[len - 1]++;
			word = strtok(NULL, delimiter);
		}
	}

	if (fclose(i_file) != 0) {
		perror("fclose");
		exit(1);
	}
}
