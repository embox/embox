#include "mqtt_test.h"

int main(int argc, char *argv[]) {
	/* connection and socket listening */
	struct addrinfo hints;
	int sockfd;
	/* message recieving */
    struct sockaddr_storage peer_addr;
    socklen_t peer_addr_len = sizeof(struct sockaddr_storage);
    ssize_t nread;
    char buf[BUF_SIZE], host[NI_MAXHOST], service[NI_MAXSERV];
	int r;
	/* logging */
	FILE *fd = fopen("log/server.log", "a");
	char timestamp[BUF_SIZE];
	time_t timer;
	struct tm *cur_time;
	// char hostbuffer[BUF_SIZE];
	// char *this_ip;
	// struct hostent *host_entry;

	// gethostname(hostbuffer, BUF_SIZE);
	// host_entry = gethostbyname(hostbuffer);
	// // To convert an Internet network
	// // address into ASCII string
	// this_ip = inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0]));
	
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;		/* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_DGRAM;	/* Datagram socket */
	hints.ai_flags = AI_PASSIVE;	/* For wildcard IP address */
	hints.ai_protocol = 0;			/* Any protocol */

	sockfd = sock_connect(NULL, TEST_PORT, &hints);
	if (sockfd < 0) {
		timer = time(NULL);
		cur_time = localtime(&timer);
		strftime(timestamp, BUF_SIZE, "%d-%m-%Y %T", cur_time);

		printf("[%s] Failed to start server: %s\n",
				timestamp, strerror(sockfd));
		fprintf(fd, "[%s] Failed to start server: %s\n",
				timestamp, strerror(sockfd));
		return sockfd;
	}

	while (1) {
		nread = recvfrom(sockfd, buf, BUF_SIZE, 0,
						(struct sockaddr *) &peer_addr, &peer_addr_len);

		timer = time(NULL);
		cur_time = localtime(&timer);
		strftime(timestamp, BUF_SIZE, "%d-%m-%Y %T", cur_time);
		
		if (nread == -1) {
			printf("[%s] Failed request.", timestamp);
			fprintf(fd, "[%s] Failed request.", timestamp);
			continue;
		}

		r = getnameinfo((struct sockaddr *) &peer_addr,
						peer_addr_len, host, NI_MAXHOST,
						service, NI_MAXSERV, NI_NUMERICSERV);

		if (r == 0) {
			printf("[%s] Received %ld bytes from %s:%s. Message: %s\n",
						timestamp, (long) nread, host, service, buf);
			fprintf(fd, "[%s] Received %ld bytes from %s:%s. Message: %s\n",
						timestamp, (long) nread, host, service, buf);
		} else {
			printf("[%s] getnameinfo: %s\n", timestamp, gai_strerror(r));
			fprintf(fd, "[%s] getnameinfo: %s\n", timestamp, gai_strerror(r));
		}

		if (nread != sendto(sockfd, buf, nread, 0, 
							(struct sockaddr *) &peer_addr, peer_addr_len))
		{
			printf("[%s] Failed to send response.", timestamp);
			fprintf(fd, "[%s] Failed to send response.", timestamp);
		}
	}
}
