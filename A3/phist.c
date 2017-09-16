#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/time.h>
#include "text_count.h"
#include "dir_search.h"
#define MAX_WORD_LEN 45

char *optarg;

int main(int argc, char **argv) {

	struct timeval starttime, endtime;
	double timediff;
	
	// Get start time for code run.
	if ((gettimeofday(&starttime, NULL)) == -1) {
		perror("gettimeofday");
		exit(1);
	}
	
	int num_procs = 1;
	int dflag = 0;
	char opt;
	char *indirname;

	// Process command line arguments.
	while ((opt = getopt(argc, argv, "d:n:o:")) != -1) {
		switch (opt) {
		// Input directory flag.
		case 'd':
			// Retrieve -d argument.
			indirname = optarg;
			// Raise flag: input directory provided.
			dflag = 1;
			break;
		// Number of processes flag.
		case 'n':;
			// Retrieve -n argument.
			char *numstring = optarg;
			char *endptr = NULL;
			// Parse out numeric value for number of processes.
			num_procs = strtol(numstring, &endptr, 10);
			// Ensure value is a positive integer and does not have extra
			// characters after it.
			if (*endptr != '\0' || num_procs < 1) {
				fprintf(stderr, "Argument after -n flag must be a positive integer.\n");
				exit(1);
			}
			break;
		// Redirection flag.
		case 'o':;
			// Retrieve -o argument.
			char *outfilename = optarg;
			// Open output file stream for writing.
			FILE* outfile = fopen(outfilename, "w");
			if (outfile == NULL) {
				perror("fopen");
				exit(1);
			}
			// Redirect stdout to the output file.
			if (dup2(fileno(outfile), fileno(stdout)) == -1) {
				perror("dup2");
				exit(1);
			}
			// Close output file stream.
			if (fclose(outfile) != 0) {
				perror("close");
				exit(1);
			}
			break;
		// For invalid flags.
		default:
			fprintf(stderr, "Usage: phist -d <input file name> [-n <number of processes>] [-o <output file name>]\n");
			exit(1);
		}
	}
	// Error if no input directory was provided.
	if (!dflag) {
		fprintf(stderr, "phist requires -d <input file name> option.\n");
		exit(1);
	}

	int i, j, size;
	// Declare 2 file descriptors per child process.
	int fd[num_procs][2];
	// Declare the word length count array, initialize all values to 0.
	int word_len[MAX_WORD_LEN];
	for (i = 0; i < MAX_WORD_LEN; i++)
		word_len[i] = 0;

	char **filenames = filename_array(indirname, &size);

	int quotient, r_remainder;
	if (num_procs < 2) {
		quotient = size;
		r_remainder = 0;
	}
	else if (num_procs >= size) {
		num_procs = size;
		quotient = 1;
		r_remainder = 0;
	}
	else {
		quotient = size/num_procs;
		r_remainder = size % num_procs;
	}

	for (i = 0; i < num_procs; i++) {
		// Create pipe.
		if (pipe(fd[i]) == -1) {
			perror("pipe");
			exit(1);
		}

		// Fork out new child.
		int pid = fork();
		if (pid < 0) {
			perror("fork");
			exit(1);
		}

		// Child process branch.
		if (pid == 0) {
			// Close child's read end.
			if (close(fd[i][0]) == -1) {
				perror("close");
				exit(1);
			}
			// Close all copied read file descriptors from parent (from earlier
			// child processes.) Parent has already closed its write ends,
			// therefore no write ends are open in these previous pipes.
			for(j = 0; j < i; j++)
				if (close(fd[j][0]) == -1) {
					perror("close");
					exit(1);
				}

			int start_ind, end_ind;
			if (i < r_remainder) {
				start_ind = i * (quotient + 1);
				end_ind = start_ind + (quotient + 1);
			}
			else {
				start_ind = r_remainder * (quotient + 1) + (i - r_remainder) * quotient;
				end_ind = start_ind + quotient;
			}

			for (j = start_ind; j < end_ind; j++)
				count_words(word_len, filenames[j], indirname);

			if (write(fd[i][1], word_len, sizeof(int)*MAX_WORD_LEN)
				!= sizeof(int)*MAX_WORD_LEN) {
				perror("write");
				exit(1);
			}

			free_filename_array(filenames, size);

			// Close child's remaining write end.
			if (close(fd[i][1]) == -1) {
				perror("close");
				exit(1);
			}

			// Terminate child process.
			exit(0);
		}
		// Parent process branch.
		else {
			// Close parent's write end.
			if (close(fd[i][1]) == -1) {
				perror("close");
				exit(1);
			}

			
		}
	}

	free_filename_array(filenames, size);

	// Only the parent reaches this part of the code.
	// Create temporary array to hold word length count.
	int temp[MAX_WORD_LEN];
	for (i = 0; i < MAX_WORD_LEN; i++)
		temp[i] = 0;

	// Add the word length counts of each child to the parent.
	for (i = 0; i < num_procs; i++) {

		// Read in the word length count array of each child through its pipe
		// and store in the temp array (overwriting the previous entries.)
		if (read(fd[i][0], temp, sizeof(int)*MAX_WORD_LEN)
			!= sizeof(int)*MAX_WORD_LEN) {
			perror("read");
			exit(1);
		}

		// Add each word length count in the temp array to the parent's word
		// length count at the appropriate index.
		for (j = 0; j < MAX_WORD_LEN; j++)
			word_len[j] += temp[j];

		// Close parent's reading end (entire pipe closed.)
		if (close(fd[i][0]) == -1) {
			perror("close");
			exit(1);
		}
	}

	// Print array of counts to stdout.
	for (i = 0; i < MAX_WORD_LEN; i++)
		fprintf(stdout, "%d, %d\n", i+1, word_len[i]);

	// Get end time for code run.
	if ((gettimeofday(&endtime, NULL)) == -1) {
		perror("gettimeofday");
		exit(1);
	}
	// Calculate running time (different in end and start time) and print.
	timediff = (endtime.tv_sec - starttime.tv_sec) +
		(endtime.tv_usec - starttime.tv_usec) / 1000000.0;
	fprintf(stderr, "%.4f\n", timediff);

	return 0;
}

