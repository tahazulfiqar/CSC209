#include "dictionary.h"

//form hash value for string s
//this produces a starting value in the dictionary array
unsigned hash(const char *s) {
	unsigned hashval;
	for (hashval = 0; *s != '\0'; s++)
		hashval = *s + 31 * hashval;
	return hashval ;
}

//Performs search for a key in the hashtable.
//if the string s is in the dictionary, it is in the list of blocks 
//beginning in array entry hash(s).
//if lookup finds entry for s, it returns a pointer to the node
//if not - it returns NULL
DNode * lookup (DNode ** dictionary, int hash_size, const char *key) {
	DNode * np;
	unsigned int hashval = hash(key);
	for (np = dictionary [hashval % hash_size]; np !=NULL; np = np->next)
		if (strcmp (key, np->key) == 0)
			return np; //found
	return NULL; //not found
}

//insert uses lookup to detemine whether key is already in the dictionary
//if not, a new entry is created
DNode * insert (DNode ** dictionary, int hash_size,  const char * key) {
	unsigned int hashval;
	DNode *np;

	if ((np = lookup (dictionary, hash_size, key)) == NULL ) { //
		np = (DNode *) malloc (sizeof (*np));

		if (np == NULL || (np->key = copystr (key)) == NULL)
			return NULL;

		hashval = hash (key) % hash_size;

		//now links itself on top of array entry
		np->next = dictionary [hashval];
		dictionary [hashval] = np;
	}
	return np;
}

// Reads in the strings in a dictionary file and hashes them into a big heap
// memory hash table as DNodes. Returns a pointer to the hash table of DNodes.
DNode ** load_dictionary() {
	// Open dictionary file: Read as text.
	FILE *dictionary_file = fopen(DICTIONARY_FILE, "r");
	if (dictionary_file == NULL) {
		perror(DICTIONARY_FILE);
		exit(1);
	}

	int i;
	// Allocate memory for DNode hash table. Initialize to NULL.
	DNode **dictionary = malloc(BIG_HASH_SIZE * sizeof(DNode *));
	for (i = 0; i < BIG_HASH_SIZE; i++)
		dictionary[i] = NULL;

	char line[MAX_LINE];

	// Read in each line of the dictionary file and hash them in (via insert.)
	while (fgets(line, MAX_LINE, dictionary_file) != NULL) {
		// Strip the newlines and convert to uppercase.
		line[strcspn(line, "\r\n")] = '\0';
		for (i = 0; i < strlen(line); i++)
			line[i] = toupper(line[i]);
		insert(dictionary, BIG_HASH_SIZE, line);
	}
	// Trigger if fgets produces an error.
	if (ferror(dictionary_file)) {
		perror(DICTIONARY_FILE);
		exit(1);
	}

	if (fclose(dictionary_file) != 0) {
		perror(DICTIONARY_FILE);
		exit(1);
	}

	return dictionary;
}

void free_dictionary (DNode ** dictionary, int hash_size) {
	int i;
	for (i=0; i<hash_size; i++) { //iterate over hash array
		if (dictionary [i]!=NULL) { //if there is an entry at position i
			DNode *head = dictionary[i]; //find the head of the linked list
			DNode *current = head;
			while (current != NULL) {
				DNode * temp = current;
				current = current->next;
				free (temp);
			}
			dictionary[i] = NULL;  //BUG fix
		}
	}	 
}

char *copystr(const char *s) { /* make a duplicate of s */
	char *p;
	int len = strlen(s);

	p = (char *) malloc(len+1); /* +1 for ’\0’ */
	if (p != NULL)
		strncpy(p, s, len);
	p[len] = '\0';

	return p;
}
