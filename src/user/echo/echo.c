/**
 * @file echo.c
 *
 * @date 08.05.2009
 * @author Nikolay Korotky
 */
#include <shell_command.h>
#include <string.h>
#include <netutils.h>
#include <net/net.h>
#include <net/in.h>
#include <net/socket.h>
#include <net/udp.h>
#include <net/ip.h>

#define COMMAND_NAME     "echo"
#define COMMAND_DESC_MSG "echo server"
#define HELP_MSG         "Usage: echo [-h] [-a addr] [-p port]"

static const char *man_page =
	#include "echo_help.inc"
	;

DECLARE_SHELL_COMMAND(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG, man_page);

static int exec(int argsc, char **argsv) {
	int nextOption;
	char *name;
	int count, fd;
	char buffer[8192];

	getopt_init();
	do {
		nextOption = getopt(argsc, argsv, "h");
		switch(nextOption) {
		case 'h':
			show_help();
			return 0;
		case -1:
			break;
		default:
			return 0;
		}
	} while(-1 != nextOption);

	fd = socket(PF_INET, SOCK_RAW, IPPROTO_UDP);
	char datagram[4096];
	iphdr_t *iph = (iphdr_t *) datagram;
	udphdr_t *udph = (udphdr_t *) (datagram + IP_HEADER_SIZE);
	memset (datagram, 0, 4096);
/*	iph->ihl = 5;
	iph->version = 4;
	iph->tos = 0;
	iph->tot_len = IP_HEADER_SIZE + UDP_HEADER_SIZE;
	iph->id = 12;
	iph->frag_off = 0;
	iph->ttl = 255;
	iph->proto = 17;
	iph->check = 0;
	iph->saddr = 0xC0A80050;
	iph->daddr = 0xFFFFFFFF;
	udph->source = 2010;
	udph->dest = 2010;
	udph->len = UDP_HEADER_SIZE;
	udph->check = 0;
	iph->check = ptclbsum((void*)iph, IP_HEADER_SIZE);*/
	if (fd < 0) {
		LOG_ERROR("socket error\n");
		return -1;
	}
//	while(1) {
//		send(fd, datagram, ETH_HEADER_SIZE + iph->tot_len, 0);
//	}
	while(1) {
		if (recv (fd, datagram, 4096, 0) > 0) {
			printf ("Caught udp packet: %s\n", datagram + IP_HEADER_SIZE + UDP_HEADER_SIZE);
		}
	}
	return 0;
}
