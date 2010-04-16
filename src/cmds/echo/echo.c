/**
 * @file
 * @brief echo test
 *
 * @date 08.05.2009
 * @author Nikolay Korotky
 */
#include <shell_command.h>
#include <string.h>
#include <netutils.h>
#include <net/net.h>
#include <net/udp.h>
#include <net/ip.h>
#include <net/checksum.h>
#include <unistd.h>

#define COMMAND_NAME     "echo"
#define COMMAND_DESC_MSG "echo server"
#define HELP_MSG         "Usage: echo [-h] [-p <sock type>]"

static const char *man_page =
	#include "echo_help.inc"
	;

DECLARE_SHELL_COMMAND(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG, man_page);

int raw_echo_server(void) {
	int fd, fromlen;
	struct sockaddr_in from;
	char datagram[1024];
	in_addr_t tmp_addr;
	__be16 tmp_port;
	iphdr_t *iph;
	udphdr_t *udph;
	fd = socket(PF_INET, SOCK_RAW, IPPROTO_UDP);
	iph = (iphdr_t *) datagram;
	memset (datagram, 0, 1024);

	if (fd < 0) {
		LOG_ERROR("socket error\n");
		return -1;
	}
	while(1) {
		if (recvfrom(fd, datagram, 1024, 0, (struct sockaddr *)&from, &fromlen) > 0) {
			//printf ("Caught udp packet: %s\n", datagram + IP_HEADER_SIZE + UDP_HEADER_SIZE);
			tmp_addr = iph->saddr;
			iph->saddr = iph->daddr;
			iph->daddr = tmp_addr;
			udph = (udphdr_t *) (datagram + IP_HEADER_SIZE(iph));
			tmp_port = udph->source;
			udph->source = udph->dest;
			udph->dest = tmp_port;
			udph->check = 0;
			iph->check = 0;
			iph->check = ptclbsum((void*)iph, IP_HEADER_SIZE(iph));
			sendto(fd, datagram, iph->tot_len, 0, (struct sockaddr *)&from, fromlen);
		}
		usleep(10);
	}
	close(fd);
	return 0;
}

int raw_client(void) {
	//TODO:
	return 0;
}

int udp_echo_server(void) {
	int fd, len, fromlen;
	struct sockaddr_in server, from;
	char buf[1024];
	fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(33);
	if(bind(fd, (struct sockaddr *)&server, sizeof(struct sockaddr_in)) == -1) {
		close(fd);
		return -1;
	}
	while (1) {
		len = recvfrom(fd, buf, 1024, 0, (struct sockaddr *)&from, &fromlen);
		if ( len > 0) {
			//printf ("Caught udp packet: %s\n", buf);
			sendto(fd, buf, len, 0, (struct sockaddr *)&from, fromlen);
		}
		usleep(10);
	}
	close(fd);
	return 0;
}

int udp_client(void) {
	int fd, len;
	struct sockaddr_in server, from;
	char buf[256] = "test";
	fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("192.168.0.59");
	server.sin_port = htons(33);
	sendto(fd, buf, strlen(buf), 0, (struct sockaddr *)&server, 0);
	while(1) {
		len = recvfrom(fd, buf, 256, 0, (struct sockaddr *)&from, NULL);
		if(len > 0) {
			printf ("Caught udp packet: %s\n", buf);
			break;
		}
		usleep(10);
	}
	close(fd);
	return 0;
}

static int exec(int argsc, char **argsv) {
	int nextOption;
	int type = 0;
	getopt_init();
	do {
		nextOption = getopt(argsc, argsv, "hp:");
		switch(nextOption) {
		case 'h':
			show_help();
			return 0;
		case 'p':
			if (1 != sscanf(optarg, "%d", &type)) {
				return -1;
			}
			break;
		case -1:
			break;
		default:
			return 0;
		}
	} while(-1 != nextOption);

	switch(type) {
	case 0:
		raw_echo_server();
		break;
	case 1:
		udp_echo_server();
		break;
	case 2:
		raw_client();
		break;
	case 3:
		udp_client();
		break;
	case -1:
		break;
	default:
		return 0;
	}
	return 0;
}
