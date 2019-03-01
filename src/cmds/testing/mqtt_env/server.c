#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>

#define BUF_SIZE 	500
#define TEST_PORT	1337

int main(int argc, char *argv[]) {
	/* connection and socket listening */
    struct addrinfo hints;
    struct addrinfo *result, *res_it;
    int sockfd;
	/* message recieving */
    struct sockaddr_storage peer_addr;
    socklen_t peer_addr_len;
    ssize_t nread;
    char buf[BUF_SIZE];
	/* function return check */
	int r;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;    /* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
	hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
	hints.ai_protocol = 0;          /* Any protocol */

	r = getaddrinfo(NULL, TEST_PORT, &hints, &result);
	if (r != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(r));
		exit(EXIT_FAILURE);
	}

	for (res_it = result; res_it != NULL; res_it = res_it->ai_next) {
		sockfd = socket(res_it->ai_family, res_it->ai_socktype,
				res_it->ai_protocol);
		if (sockfd == -1)
			continue;

		if (bind(sockfd, res_it->ai_addr, res_it->ai_addrlen) == 0)
			break;                  /* Success */

		close(sockfd);
	}

	if (res_it == NULL) {               /* No address succeeded */
		fprintf(stderr, "Could not bind\n");
		exit(EXIT_FAILURE);
	}

	freeaddrinfo(result);           /* No longer needed */

	/* Read datagrams and echo them back to sender */

	for (;;) {
		peer_addr_len = sizeof(struct sockaddr_storage);
		nread = recvfrom(sockfd, buf, BUF_SIZE, 0,
				(struct sockaddr *) &peer_addr, &peer_addr_len);
		if (nread == -1)
			continue;               /* Ignore failed request */

		char host[NI_MAXHOST], service[NI_MAXSERV];

		r = getnameinfo((struct sockaddr *) &peer_addr,
						peer_addr_len, host, NI_MAXHOST,
						service, NI_MAXSERV, NI_NUMERICSERV);
		if (r == 0)
			printf("Received %ld bytes from %s:%s\n",
					(long) nread, host, service);
		else
			fprintf(stderr, "getnameinfo: %s\n", gai_strerror(r));

		if (sendto(sockfd, buf, nread, 0,
					(struct sockaddr *) &peer_addr,
					peer_addr_len) != nread)
			fprintf(stderr, "Error sending response\n");
	}
}