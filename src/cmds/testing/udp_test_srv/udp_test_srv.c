#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdint.h>
#include <unistd.h>

#define BUFLEN 512
#define NPACK 1024 * 1024
#define PORT 9930

static void diep(char *s) {
	perror(s);
}

int main(int argc, char **argv) {
	struct sockaddr_in si_me, si_other;
	int s, i, slen=sizeof(si_other);
	char buf[BUFLEN];
	int port = PORT;
	int total_packets = 0;

	if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1) {
		diep("socket");
	}

	if (argc > 1) {
		port = atoi(argv[1]);
	}

	memset((char *) &si_me, 0, sizeof(si_me));

	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(port);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(s, (void *) &si_me, sizeof(si_me))==-1) {
		return -1;
	}

	for (i = 0; i < NPACK; ) {
		int num = 0;

		if (recvfrom(s, buf, BUFLEN, 0, (void *)&si_other, &slen)==-1){
			diep("recvfrom()");
		}

		sscanf(buf, "This is packet %d", &num);

		if (sendto(s, buf, BUFLEN, 0, (void *) &si_other, slen)==-1) {
			diep("send()");
		}

		total_packets++;
		if (1 || total_packets % 1000 == 0) {
		}
	}

	close(s);
	return 0;
}
