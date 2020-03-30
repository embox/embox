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

void diep(char *s)
{
	perror(s);
}

extern uint32_t udp_time, send1_time, udp_id;
uint32_t after;
//uint32_t before;



int last_udp_num[1000000];
int last_udp;

void dcache_inval(const void *p, size_t size);
int main(int argc, char **argv) {
	struct sockaddr_in si_me, si_other;
	int s, i, slen=sizeof(si_other);
	char buf[BUFLEN];
	int port = PORT;

	static int t = 0;

	if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
		diep("socket");

	if (argc > 1) {
		port = atoi(argv[1]);
	}

	memset((char *) &si_me, 0, sizeof(si_me));

	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(port);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(s, (void *) &si_me, sizeof(si_me))==-1)
		return -1;

	for (i=0; i<NPACK; ) {
		if (recvfrom(s, buf, BUFLEN, 0, (void *)&si_other, &slen)==-1)
			diep("recvfrom()");

		int num = 0;

		sscanf(buf, "This is packet %d", &num);

		last_udp_num[(last_udp++) %1000000] = num;
	//	printf("got packet!\n");
	//	clock_gettime(CLOCK_REALTIME, &after);

//		after = *((uint32_t *)0xfffec604);
//			uint32_t diff = udp_time > after ? udp_time - after : after - udp_time;
//			if (diff > 270000) {
//			if (1) printf("<<<%d packed #%d\n", diff, t);
//			}
//
			//udp_id = t;
		//send1_time = *((uint32_t *)0xfffec604);
		if (sendto(s, buf, BUFLEN, 0, (void *) &si_other, slen)==-1) {
			diep("send()");
		}
	//	printf("Received packet from %s:%d\nData: %s\n\n",
	//			inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port), buf);

		t++;
		if (1 || t % 1000 == 0) {
			//dcache_inval((void*)0xfffec004, 4);
			//dcache_inval((void*)0xfffec004, 4);
//		before = *((uint32_t *)0xfffec004);
		}
	}

	close(s);
	return 0;
}
