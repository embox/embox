// #include <arpa/inet.h>
// #include <errno.h>
#include <netdb.h> 
// #include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define BUF_SIZE 	500
#define TEST_PORT	"1337"

int main(int argc, char *argv[]) {
    struct addrinfo hints;
    struct addrinfo *result, *res_it;
    int sfd, s, i;
    size_t len;
    ssize_t nread;
    char buf[BUF_SIZE], msg[BUF_SIZE];
	/* for logging */
	FILE *fd = fopen("log/client.log", "a");
	char timestamp[BUF_SIZE];
	time_t timer;
	struct tm *cur_time;

	/* Obtain address(es) matching host/port */

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
	hints.ai_flags = 0;
	hints.ai_protocol = 0;          /* Any protocol */

	s = getaddrinfo("10.0.2.16", TEST_PORT, &hints, &result);
	if (s != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
		exit(EXIT_FAILURE);
	}

	/* getaddrinfo() returns a list of address structures.
		Try each address until we successfully connect(2).
		If socket(2) (or connect(2)) fails, we (close the socket
		and) try the next address. */

	for (res_it = result; res_it != NULL; res_it = res_it->ai_next) {
		sfd = socket(res_it->ai_family, res_it->ai_socktype,
					 res_it->ai_protocol);
		if (sfd == -1)
			continue;

		if (connect(sfd, res_it->ai_addr, res_it->ai_addrlen) != -1)
			break;                  /* Success */

		close(sfd);
	}

	if (res_it == NULL) {               /* No address succeeded */
		fprintf(stderr, "Could not connect\n");
		exit(EXIT_FAILURE);
	}

	freeaddrinfo(result);           /* No longer needed */

	/* Send remaining command-line arguments as separate
		datagrams, and read responses from server */

	for (i = atoi(argv[1]); i <= 20; i+=2) {
		sprintf(msg, "ping%i", i);
		len = strlen(msg) + 1;
		timer = time(NULL);
		cur_time = localtime(&timer);
		strftime(timestamp, BUF_SIZE, "%d-%m-%Y %I:%M:%S %p", cur_time);

		if (len + 1 > BUF_SIZE) {
			fprintf(stderr, "Message too long: %d\n", i);
			continue;
		}
		nread = write(sfd, msg, len);
		if (nread != len) {
			fprintf(stderr, "partial/failed write\n");
			exit(EXIT_FAILURE);
		}
		fprintf(fd, "[%s] Sent %ld bytes to server. Message: %s\n",
					 timestamp, (long) nread, msg);

		nread = read(sfd, buf, BUF_SIZE);
		if (nread == -1) {
			perror("read");
			exit(EXIT_FAILURE);
		}

		printf("Received %ld bytes: %s\n", (long) nread, buf);

		fprintf(fd, "[%s] Recieved %ld bytes from server. Message: %s\n",
					 timestamp, (long) nread, msg);
	}

	exit(EXIT_SUCCESS);
}
