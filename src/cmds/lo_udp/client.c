/* Client side implementation of UDP client-server model */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <net/net_namespace.h>

#define PORT	 8080
#define MAXLINE 1024

#define SERV_ADDR "10.0.0.1"
#define CL_ADDR "10.0.0.2"

int main(int argc, char *argv[]) {
	int sockfd;
	char *hello = "Hello from client";
	struct sockaddr_in servaddr, cliaddr;
	int len = sizeof(servaddr), n;
	char buffer[MAXLINE];

	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

	memset(&servaddr, 0, sizeof(servaddr));

	memset(&cliaddr, 0, sizeof(cliaddr));

	cliaddr.sin_family = AF_INET;
	cliaddr.sin_port = htons(PORT);
	if (inet_aton(CL_ADDR, &cliaddr.sin_addr) == 0) {
		printf("Fail\n");
		return 0;
	}

	if ( bind(sockfd, (const struct sockaddr *)&cliaddr,
			sizeof(cliaddr)) < 0 ) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	if (inet_aton(SERV_ADDR, &servaddr.sin_addr) == 0) {
		printf("Fail\n");
		return 0;
	}

	while (1) {
		sendto(sockfd, (const char *)hello, strlen(hello),
			0, (const struct sockaddr *) &servaddr,
				sizeof(servaddr));
		sleep(3);
		n = recvfrom(sockfd, (char *)buffer, MAXLINE,
					0, ( struct sockaddr *) &servaddr,
					&len);
		buffer[n] = '\0';
		if (n > 0) {
			printf("Server msg from port %d: %s; client net namespace %s\n",
				ntohs(servaddr.sin_port), buffer,
				(get_net_ns()).p->name);
		}
	}

	close(sockfd);
	return 0;
}

