/**
 * @file
 * @brief Easy TFTP client
 *
 * @date 12.03.10
 * @author Nikolay Korotky
 */
#include <embox/cmd.h>
#include <getopt.h>
#include <string.h>
#include <stdio.h>
#include "tftp.h"

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: tftp <server> <file>\n");
}

#define MAX_FILENAME_LEN 0x40

static int create_socket(struct sockaddr_in *addr) {
	int sock;

	if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		printf("Creation socket impossible");
		return -1;
	}

	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = htonl(INADDR_ANY);
	addr->sin_port = htons(38666); /* TODO: catch some availible port */

	if (bind(sock, (struct sockaddr *) addr,
			sizeof(struct sockaddr_in)) == -1) {
		printf("Attachement socket impossible.\n");
		close(sock);
		return -1;
	}
	return sock;
}

static int tftp_receive(struct sockaddr_in *to, char *mode,
					char *name, FILE *file) {
	int desc;
	size_t size, dsize = 0;
	socklen_t fromlen;
	struct sockaddr_in from;
	char buf[PKTSIZE], ackbuf[PKTSIZE], *dat, *cp;
	tftphdr_t *dp, *ap;
	dp = (tftphdr_t *) buf;
	ap = (tftphdr_t *) ackbuf;
	dat = (char *) &dp->th_data[0];
	cp = (char *) &ap->th_stuff[0];

	if ((desc = create_socket(&from)) < 0) {
		return -1;
	}

	ap->th_opcode = htons((short) RRQ);
	strcpy(cp, name);
	cp += strlen(name);
	*cp++ = '\0';
	strcpy(cp, mode);
	cp += strlen(mode);
	*cp++ = '\0';
	strcpy(cp, "tsize");
	cp += strlen("tsize");
	*cp++ = '\0';
	strcpy(cp, "0");
	cp += strlen("0");
	*cp++ = '\0';
	size = (unsigned long) cp - (unsigned long) ackbuf;
	fromlen = sizeof(struct sockaddr_in);
	sendto(desc, ap, size, 0, (struct sockaddr *) to, fromlen);
	while (1) {
		size = recvfrom(desc, dp, PKTSIZE, 0,
			(struct sockaddr *) &from, &fromlen);
		if (size > 0) {
			if (dp->th_opcode == ERROR) {
				printf("Error %d\n", dp->th_u.tu_code);
				break;
			}
			if (dp->th_opcode == DATA) {
				//printf("data: %s\n", dp->th_data);
				fwrite(dp->th_data, size - 4, 1, file);
				dsize += size - 4;
				if (dsize % 0x1000 == 0) {
					printf("Download: %d bytes\r", dsize);
				}
				ap->th_opcode = htons((short)ACK);
				ap->th_block = dp->th_block;
				if (sendto(desc, ap, 4, 0,
				    (struct sockaddr *) &from, fromlen) < 0) {
					printf("Error occured\n");
				}
				if (size < PKTSIZE) {
					break;
				}
			}
			if (dp->th_opcode == OACK) {
				/* TODO: check availible memory capacity */
				ap->th_opcode = htons((short) ACK);
				ap->th_block = dp->th_block;
				if (sendto(desc, ap, 4, 0,
				    (struct sockaddr *) &from, fromlen) < 0) {
					printf("Error occured\n");
				}
				break;
			}
		}
	}
	printf("Downloaded: %d bytes\n", dsize);
	close(desc);
	return 0;
}

static int exec(int argc, char **argv) {
	struct sockaddr_in server;
	FILE *f;
	char fname[MAX_FILENAME_LEN];

	if (argc < 3) {
		print_usage();
		return -1;
	}
	server.sin_family = AF_INET;
	server.sin_port = htons(TFTP_TRANSFER_PORT);
	server.sin_addr.s_addr = inet_addr(argv[1]);

	sprintf(fname, "%s", argv[2]);

	if (NULL == (f = fopen(fname, "wb"))) {
		printf("Can't open file %s\n", fname);
		return -1;
	}
	tftp_receive(&server, "octet", argv[2], f);
	fclose(f);
	return 0;
}
