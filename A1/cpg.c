	#include <stdio.h>
	#include <stdlib.h>

	//function declarations
	int valid_chars_count (char *input, int size);
	int valid_char (char c);
	int valid_cg_count (int c_count, int g_count);
	int valid_cg_ratio (int c_count, int g_count, int cg_ratio);	

	int main (int argc, char *argv[]) {
		char * inputfile_name=argv[1]; 
		FILE * input_fp;
		char *input_buffer; //stores all bytes in the input file
		char *sequence; //char array holding the final cleaned DNA sequence
		
		int *valid_cpg_positions; //array of all positions in sequence
		int input_len, seq_len;
		
		int i, j;
		int r;

		//open the file
		if ( !(input_fp= fopen ( inputfile_name , "rb" )) ) {
			printf("Could not open file \"%s\" for reading input lines \n", inputfile_name);
			return (-1);
		}
		
		//compute total number of characters in the input file
		fseek (input_fp, 0, SEEK_END);
		input_len=ftell (input_fp);
		rewind (input_fp); 
		//printf ("total bytes in file: %d\n", input_len);
		
		//allocate an array to hold these characters
		input_buffer = (char*) malloc (input_len+1);
		
		//read file content into the input buffer
		r = fread (input_buffer, 1, input_len,  input_fp);
		//printf ("read characters %d\n", r);
		if (r != input_len) {
			printf("Reading error \n");
			return (-1);
		}
		//add terminating character
		input_buffer [input_len] ='\0';

		//determine total number of valid DNA characters
		//and allocate array of chars to hold them
		seq_len = valid_chars_count (input_buffer, input_len);
		//printf ("total characters %d total valid characters %d \n", input_len, seq_len);
		sequence = (char*) malloc (seq_len+1); 
		
		//transfer valid characters from raw buffer
		for (i=0, j=0; i < input_len; i++) {
			if (valid_char (input_buffer [i])) {
				sequence [j++] = input_buffer [i];
			}
		}
		sequence [seq_len] = '\0';
		
		//allocate int array for all the positions
		valid_cpg_positions = (int*) malloc (seq_len*sizeof(int));
		for (i=0; i<seq_len; i++)
			valid_cpg_positions[i] = 0;
			
		/* YOUR CpG ISLANDS DISCOVERY CODE HERE */
	
		//boundaries for the window 
		int upper_bound, lower_bound;
		
		//loop through all possible windows of size 200 within sequence 
		for (upper_bound = 200, lower_bound = 0; upper_bound < seq_len+1; upper_bound++, lower_bound++){
			
			//x is for iterating through each element of a 200 element window 	
			int x;

			//p is for iterating through all elements of valid_cpg_position
			int p;
			
			//initialize the respective counters
			int c_sum = 0;
			int g_sum = 0;
			int cg_sum = 0;
			
			//As long as x is within the 200 window boundary	
			for (x=lower_bound; x < upper_bound; x++){
			
				//if the character is c, increment c_sum.
				if (sequence[x] == 'c'){
					c_sum++;
				}
				
				//if the character is g, increment g_sum.
				if (sequence[x] == 'g'){
					g_sum++;
				}

				//As long as x isn't the last character of the window, compare it with the next array element.
				if(x < upper_bound-1){

					//if the current letter is c and the letter after is g, increment cg_sum
					if (sequence[x] == 'c' && sequence[x+1] == 'g'){
						cg_sum++;
					}
				}
			}
			
			//Once you reach the 200th element, check to see if window satisfies the two CpG island conditions
			if (valid_cg_ratio(c_sum, g_sum, cg_sum) && valid_cg_count(c_sum, g_sum)){
				
				//if it does, set the corresponding elements in valid_cpg_position to 1
				for(p=lower_bound; p < upper_bound; p++){
					valid_cpg_positions[p] = 1;
				}
			}
		}

	// initialize elements, set counter to an initial value of 0. 		
	int d;
	int counter = 0;	
	int starting_index;
	int ending_index;
	int island_count;
	
	// loop through the entire sequence
	for (d=0; d < seq_len; d++)
		
		//Case for the first element
		if (d == 0){
			//if the first element is 1, set the starting index to 0 and increment the counter
			if (valid_cpg_positions[0] == 1)
				counter++;
				starting_index = 0;
				
		}

		//For cases not including the first element
		else{
			//if the current value at the index is 1
			if(valid_cpg_positions[d] ==1){
				//increment the counter
				counter++;
				//if the previous element was 0, this is the starting index of a CpG island.   
				if(valid_cpg_positions[d-1] == 0)
					starting_index = d;
			}

			//if the current value at the index is 0
			else{
				//if the previous element was 1, this indicates the end of the CpG island. Print statement follows 
				if(valid_cpg_positions[d-1] == 1){ 
					ending_index = d-1;
					printf("CpG Island\t%d..%d\tsize=%d\n", starting_index, ending_index, counter);
					//the number of CpG islands is incremented 
					island_count++;
				}
			//size is reset
			counter = 0;
			}	
		}			
		
		//add in the printing statement for numislands
		printf("numislands\t%d\n", island_count);	

		return 1;
	}

	int valid_chars_count (char *input, int size) {
		int i, count = 0;
		for (i=0; i<size; i++) {
			if (valid_char (input [i]) )
				count++;
		}
		return count;
	}

	int valid_char (char c) {
		if (c == 'a' || c == 'c' || c == 'g' || c == 't' ) 
			return 1;
		return 0;
	}

	int valid_cg_count (int c_count, int g_count) { 
		int cg_count = c_count + g_count;
		double cg_percentage = (double)cg_count / 200;
		//printf("\t%f", cg_percentage);
		if (cg_percentage > 0.5) 
			return 1;
		return 0;
	}

	int valid_cg_ratio(int c_count, int g_count, int cg_count){
		double expected_cg = (double)c_count * g_count /200;
		double ratio = cg_count /expected_cg;
		//printf("\t%f", ratio);
		if (ratio >= 0.6)
			return 1; 
		return 0;
	}
