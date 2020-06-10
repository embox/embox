#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <drivers/adar7251_driver.h>
#include <drivers/nucleo_f429zi_audio.h>

#define PORT 9930
#define BUFLEN 1024

static uint8_t rx_buf[SAI_SAMPLES_BUFFER * 2]; /* half of sai buffer */
static struct adar7251_dev adar7251_dev;
static char buf[BUFLEN];

int main(int argc, char **argv) {
	int data_len;
	struct sockaddr_in si_me, si_other;
	int s, slen=sizeof(si_other);

	int port = PORT;

	if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1) {
		printf("socket() failed");
		return -1;
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

	printf("starting adar7251\n");
	adar7251_hw_init(&adar7251_dev);

	printf("waiting UDP packet on port %d\n", port);
	if (recvfrom(s, buf, BUFLEN, 0, (void *)&si_other, &slen)==-1){
		printf("recvfrom() failed");
		return 0;
	}

	printf("conversation loop\n");

	while(1) {
		data_len = sai_receive(adar7251_dev.sai_dev, rx_buf, sizeof(rx_buf));
		if (data_len == 0) {
			printf("sai timeout\n");
			continue;
		}
		//printf("%d\n", data_len);
		if (sendto(s, rx_buf, data_len, 0, (void *) &si_other, slen)==-1) {
			printf("sendto() failed");
		}
	}

	return 0;
}
