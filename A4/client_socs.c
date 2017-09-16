#include "client_socs.h"

/*
	Creates and binds a listening socket.
*/
int socsetup(int port) {
	struct sockaddr_in saddr;
	int listenfd;

	// Get the listening socket file descriptor.
	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}

	// Use setsockopt to release the port immediately after termination.
	int on = 1;
	int status = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
		(const char *)&on, sizeof(on));
	if (status == -1) {
		perror("setsockopt -- REUSEADDR");
	}

	// Set up address that the socket binds to.
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = INADDR_ANY;
	saddr.sin_port = htons(port);
	memset(&saddr.sin_zero, 0, sizeof(saddr.sin_zero));

	// Bind the socket.
	if (bind(listenfd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0) {
		perror("bind");
		exit(1);
	}

	// Listen for incoming connections.
	if (listen(listenfd, LISTEN_QUEUE_LEN) < 0) {
		perror("listen");
		exit(1);
	}

	fprintf(stdout, "> Listening on %d\n", port);

	return listenfd;
}

/*
	Accept an incoming client connection.
*/
int newconnection(int listenfd, Client **clist_ptr) {
	int cfd;
	struct sockaddr_in r;
	socklen_t socklen = sizeof r;

	// Accept connection, retrieve file descriptor of socket to client.
	if ((cfd = accept(listenfd, (struct sockaddr *)&r, &socklen)) < 0)
		perror("accept");
	else {
		// Server-side prompt.
		printf("> Connection from %s\n", inet_ntoa(r.sin_addr));

		// Add incoming client to the linked list of all active clients.
		addclient(cfd, r.sin_addr, clist_ptr);

		// Client-side prompt (must write to client.)
		char str[MESSAGE_MAX_LEN];
		sprintf(str, "- Client (%s) connected.\r\n", inet_ntoa(r.sin_addr));
		if (write(cfd, str, strlen(str) + 1) != strlen(str) + 1) {
			perror("write");
			exit(1);
		}
	}

	return cfd;
}

/*
	Read from selected client. Returns 0 if the line was read successfully.
	Returns -1 if the selected client has disconnected or on some read error.
*/
int whatsup(Client *c_ptr, char *cmd_buf, int buf_size, Client **clist_ptr) {
	// Read commands from the client and store into cmd_buf (cmd_buf is passed
	// in as a parameter.) If len == 0, the client has disconnected.
	int len = readLine(c_ptr->fd, cmd_buf, buf_size);

	if (len > 0)
		return 0;
	// May delete fprintf below.
	// fprintf(stdout, "Received command \"%s\" from %s\n", cmd_buf, inet_ntoa(c_ptr->ipaddr));
	else if (len == 0) {

		fprintf(stdout, "> Disconnection from %s\n", inet_ntoa(c_ptr->ipaddr));
		// Close socket to disconnected client.
		close(c_ptr->fd);
		fflush(stdout);

		// Remove disconnecting client from linked list of all active clients.
		removeclient(c_ptr->fd, clist_ptr);
		return -1;
	}
	else {
		// Should not happen.
		perror("readLine");
		return -1;
	}
}

/*
	Add a client to the linked list of active clients.
*/
void addclient(int fd, struct in_addr addr, Client **clist_ptr) {
	// Allocate heap memory to store the Client struct.
	Client *c_ptr = malloc(sizeof(Client));
	if (c_ptr == NULL) {
		perror("malloc");
		exit(1);
	}

	fprintf(stdout, "> Adding client %s\n", inet_ntoa(addr));
	fflush(stdout);

	// Adds the client to the head of the linked list.
	c_ptr->fd = fd;
	c_ptr->ipaddr = addr;
	c_ptr->next = *clist_ptr;
	c_ptr->gamestate = 2;
	*clist_ptr = c_ptr;
}

/*
	Removes a client from the linked list of active clients.
*/
void removeclient(int fd, Client **clist_ptr) {
	Client *c_ptr = *clist_ptr;
	Client *prev_ptr = *clist_ptr;

	// Find client's struct in the list by iterating from the list head.
	while (c_ptr != NULL && c_ptr->fd != fd) {
		prev_ptr = c_ptr;
		c_ptr = c_ptr->next;
	}

	if (c_ptr == NULL) {
		fprintf(stderr, "> Cannot find client with the given file descriptor %d\n", fd);
		fflush(stderr);
	}
	else {
		// If the list head is the client to be removed...
		if (c_ptr == prev_ptr)
			*clist_ptr = c_ptr->next;
		// For all other clients...
		else
			prev_ptr->next = c_ptr->next;

		fprintf(stdout, "> Removing client %s\n", inet_ntoa((c_ptr)->ipaddr));
		// Free the memory occupied by the removed Client struct.
		free(c_ptr);
		fflush(stdout);
	}
}
