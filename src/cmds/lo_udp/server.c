/* Server side implementation of UDP client-server model */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <net/net_namespace.h>

#define PORT	 8080
#define MAXLINE 1024

#define SERV_ADDR "10.0.0.1"

int main(int argc, char *argv[]) {
	int sockfd;
	char buffer[MAXLINE];
	struct sockaddr_in servaddr, cliaddr;
	int len = sizeof(cliaddr), n;
	char *hello = "Hello from server";

	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

	memset(&servaddr, 0, sizeof(servaddr));
	memset(&cliaddr, 0, sizeof(cliaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	if (inet_aton(SERV_ADDR, &servaddr.sin_addr) == 0) {
		printf("Fail\n");
		return 0;
	}

	if ( bind(sockfd, (const struct sockaddr *)&servaddr,
			sizeof(servaddr)) < 0 ) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	while (1) {
		n = recvfrom(sockfd, (char *)buffer, MAXLINE,
					0, ( struct sockaddr *) &cliaddr,
					&len);
		buffer[n] = '\0';
		if (n > 0) {
			printf("Client msg from port %d: %s; server net namespace %s\n",
				ntohs(cliaddr.sin_port), buffer,
				(get_net_ns()).p->name);

			sendto(sockfd, (const char *)hello, strlen(hello),
				0, (const struct sockaddr *) &cliaddr,
					sizeof(cliaddr));
		}
	}

	close(sockfd);
	return 0;
}

