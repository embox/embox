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
	/* connection and socket listening */
    struct addrinfo hints;
    struct addrinfo *result, *res_it;
    int sockfd;
	/* message recieving */
    struct sockaddr_storage peer_addr;
    socklen_t peer_addr_len = sizeof(struct sockaddr_storage);
    ssize_t nread;
    char buf[BUF_SIZE], host[NI_MAXHOST], service[NI_MAXSERV];
	/* function return check */
	int r;
	/* for logging */
	FILE *fd = fopen("log/server.log", "a");
	char timestamp[BUF_SIZE];
	time_t timer;
	struct tm *cur_time;
	// char hostbuffer[BUF_SIZE];
	// char *this_ip;
	// struct hostent *host_entry;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;		/* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_DGRAM;	/* Datagram socket */
	hints.ai_flags = AI_PASSIVE;	/* For wildcard IP address */
	hints.ai_protocol = 0;			/* Any protocol */

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
			break;		/* Success */

		close(sockfd);
	}

	if (res_it == NULL) {		/* No address succeeded */
		fprintf(stderr, "Could not bind\n");
		exit(EXIT_FAILURE);
	}

	freeaddrinfo(result);

	// gethostname(hostbuffer, BUF_SIZE);
	// host_entry = gethostbyname(hostbuffer);
	// // To convert an Internet network
	// // address into ASCII string
	// this_ip = inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0]));

	while (1) {
		nread = recvfrom(sockfd, buf, BUF_SIZE, 0,
						 (struct sockaddr *) &peer_addr, &peer_addr_len);

		timer = time(NULL);
		cur_time = localtime(&timer);
		strftime(timestamp, BUF_SIZE, "%d-%m-%Y %I:%M:%S %p", cur_time);
		
		if (nread == -1) {
			fprintf(fd, "[%s] Failed request.", timestamp);
			continue;
		}

		r = getnameinfo((struct sockaddr *) &peer_addr,
						peer_addr_len, host, NI_MAXHOST,
						service, NI_MAXSERV, NI_NUMERICSERV);

		if (r == 0) {
			printf("Received %ld bytes from %s:%s\n",
				   (long) nread, host, service);
			
			fprintf(fd, "[%s] Received %ld bytes from %s:%s. Message: %s\n",
						timestamp, (long) nread, host, service, buf);
		} else {
			fprintf(stderr, "getnameinfo: %s\n", gai_strerror(r));

			fprintf(fd, "[%s] getnameinfo: %s\n", timestamp, gai_strerror(r));
		}

		if (nread != sendto(sockfd, buf, nread, 0, 
							(struct sockaddr *) &peer_addr, peer_addr_len))
		{
			fprintf(stderr, "Error sending response\n");

			fprintf(fd, "[%s] Failed to send response.", timestamp);
		}
	}
}
