/**
 * @file
 * @brief TFTP client
 *
 * @date 12.03.10
 * @author Nikolay Korotky
 */
#include <shell_command.h>
#include <string.h>
#include <stdio.h>
#include "tftp.h"

#define COMMAND_NAME     "tftp"
#define COMMAND_DESC_MSG "TFTP client"
#define HELP_MSG         "Usage: tftp <server> <file>"

static const char *man_page =
	#include "tftp_help.inc"
;

DECLARE_SHELL_COMMAND(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG, man_page);

static int create_socket(struct sockaddr_in *addr) {
	int sock;

	if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		printf("Creation socket impossible");
		return -1;
	}

	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = htonl(INADDR_ANY);
	addr->sin_port = htons(666);

	if (bind(sock, (struct sockaddr *)addr, sizeof(struct sockaddr_in)) == -1) {
		printf("Attachement socket impossible.\n");
		close(sock);
		return -1;
	}
	return sock;
}

int tftp_receive(struct sockaddr_in *to, char *mode, char *name, FILE *file) {
	int desc;
	size_t size, fromlen;
	struct sockaddr_in from;
	char buf[PKTSIZE], ackbuf[PKTSIZE], *dat, *cp;
	tftphdr_t *dp,*ap;
	dp = (tftphdr_t *)buf;
	ap = (tftphdr_t *)ackbuf;
	dat = (char*)&dp->th_data[0];
	cp = (char*)&ap->th_stuff[0];

	if((desc = create_socket(&from)) < 0) {
		return -1;
	}

	ap->th_opcode = htons((short)RRQ);
	strcpy(cp, name);
	cp += strlen(name);
	*cp++ = '\0';
	strcpy(cp, mode);
	cp += strlen(mode);
	*cp++ = '\0';
	size = (unsigned long)cp - (unsigned long)ackbuf;
	fromlen = sizeof(struct sockaddr_in);
	sendto(desc, ap, size, 0, (struct sockaddr *)to, fromlen);
	while(1) {
		size = recvfrom(desc, dp, PKTSIZE, 0, (struct sockaddr *)&from, &fromlen);
		if(size > 0) {
			//printf("data: %s\n", dp->th_data);
			fwrite(dp->th_data, size - 4, 1, file);
			ap->th_opcode = htons((short)ACK);
			ap->th_block = dp->th_block;
			sendto(desc, ap, 4, 0, (struct sockaddr *)&from, fromlen);
			if(size < PKTSIZE) {
				break;
			}
		}
	}
	close(desc);
}

static int exec(int argsc, char **argsv) {
	struct sockaddr_in server;
	FILE *f;

	if (argsc < 3) {
		show_help();
		return -1;
	}
	server.sin_family = AF_INET;
	server.sin_port = htons(69);
	server.sin_addr.s_addr = inet_addr(argsv[1]);

	f = fopen(argsv[2], "wb");
	tftp_receive(&server, "octet", argsv[2], f);
	fclose(f);
	return 0;
}
