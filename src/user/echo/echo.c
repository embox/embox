/**
 * @file echo.c
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

#define COMMAND_NAME     "echo"
#define COMMAND_DESC_MSG "echo server"
#define HELP_MSG         "Usage: echo [-h] [-a addr] [-p port]"

static const char *man_page =
	#include "echo_help.inc"
	;

DECLARE_SHELL_COMMAND(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG, man_page);

static int exec(int argsc, char **argsv) {
	int nextOption;
	int fd;
	char datagram[4096];
	in_addr_t tmp_addr;
	__be16 tmp_port;
	iphdr_t *iph;
	udphdr_t *udph;

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
	iph = (iphdr_t *) datagram;
	udph = (udphdr_t *) (datagram + IP_HEADER_SIZE);
	memset (datagram, 0, 4096);

	if (fd < 0) {
		LOG_ERROR("socket error\n");
		return -1;
	}
	while(1) {
		if (recv (fd, datagram, 4096, 0) > 0) {
			printf ("Caught udp packet: %s\n", datagram + IP_HEADER_SIZE + UDP_HEADER_SIZE);
			tmp_addr = iph->saddr;
			iph->saddr = iph->daddr;
			iph->daddr = tmp_addr;
			tmp_port = udph->source;
			udph->source = udph->dest;
			udph->dest = tmp_port;
			udph->check = 0;
			iph->check = 0;
			iph->check = ptclbsum((void*)iph, IP_HEADER_SIZE);
			send(fd, datagram, iph->tot_len, 0);
		}
	}
	return 0;
}
