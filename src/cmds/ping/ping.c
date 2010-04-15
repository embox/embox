/**
 * @file
 * @brief send ICMP ECHO_REQUEST to network hosts.
 *
 * @date 20.03.2009
 * @author Anton Bondarev
 * @author Nikolay Korotky
 * 			- implement ping through raw socket.
 * 			- major refactoring
 */
#include <shell_command.h>
#include <net/icmp.h>
#include <net/ip.h>
#include <net/inetdevice.h>
#include <net/checksum.h>
#include <netutils.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define COMMAND_NAME     "ping"
#define COMMAND_DESC_MSG "send ICMP ECHO_REQUEST to network hosts"
#define HELP_MSG         "Usage: ping [-I if] [-c cnt] [-W timeout] [-t ttl]\n\
		[-i interval] [-p pattern] [-s packetsize] destination"

static const char *man_page =
	#include "ping_help.inc"
;

DECLARE_SHELL_COMMAND(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG, man_page);

typedef struct ping_info {
	/* Stop after sending count ECHO_REQUEST packets. */
	int count;
	/* The number of data bytes to be sent. */
	int padding_size;
	/* Time  to wait for a response, in seconds. */
	int timeout;
	/* Wait  interval seconds between sending each packet. */
	int interval;
	/* Specify up to 16 ``pad'' bytes to fill out the packet to send. */
	int pattern;
	/* IP Time to Live. */
	int ttl;
	/* Source address to specified interface address. */
	struct in_addr from;
	/* Destination host */
	struct in_addr dst;
} ping_info_t;

/* boundary and default */
enum {
	DEF_COUNT = 4,
	DEF_PADLEN = 56,
	DEF_TIMEOUT = 1,
	DEF_INTERVAL = 1,
	DEF_PATTERN = 0xFF,
	DEF_TTL = 59,
	MAX_PADLEN = 65507,
	MAX_TIMEOUT = 2148,
};

static int ping(ping_info_t *pinfo) {
	int cnt_resp = 0, cnt_err = 0, sk;
	size_t i;
	iphdr_t *iph;
	icmphdr_t *icmph;
	char *from_addr_str, *dst_addr_str;
	char packet[IP_MIN_HEADER_SIZE + ICMP_HEADER_SIZE + MAX_PADLEN];
	char rcv_buff[IP_MIN_HEADER_SIZE + ICMP_HEADER_SIZE + MAX_PADLEN];
	dst_addr_str = inet_ntoa(pinfo->dst);
	from_addr_str = inet_ntoa(pinfo->from);
	printf("PING %s %d bytes of data.\n", dst_addr_str, pinfo->padding_size);

	if((sk = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
		LOG_ERROR("socket error\n");
		return -1;
	}
	iph = (iphdr_t *)packet;
	icmph = (icmphdr_t *) (packet + IP_MIN_HEADER_SIZE);
	memset(packet, 0, IP_MIN_HEADER_SIZE + ICMP_HEADER_SIZE + pinfo->padding_size);
	memset(icmph, pinfo->pattern, pinfo->padding_size);
	iph->version = 4;
	iph->ihl = IP_MIN_HEADER_SIZE >> 2;
	iph->tos = 0;
	iph->frag_off = IP_DF;
	iph->saddr = pinfo->from.s_addr;
	iph->daddr = pinfo->dst.s_addr;
	iph->tot_len = IP_MIN_HEADER_SIZE + ICMP_HEADER_SIZE + pinfo->padding_size;
	iph->ttl = pinfo->ttl;
	iph->proto = IPPROTO_ICMP;
	icmph->type = ICMP_ECHO;
	icmph->code = 0;
	icmph->un.echo.id = /*TID*/0;

	for(i = 0; i < pinfo->count; i++) {
		icmph->un.echo.sequence++;
		icmph->checksum = 0;
		icmph->checksum = ptclbsum(packet + IP_MIN_HEADER_SIZE,
						ICMP_HEADER_SIZE + pinfo->padding_size);

		sendto(sk, packet, iph->tot_len, 0, (struct sockaddr *)&pinfo->dst, 0);
		sleep(pinfo->timeout);

		/* we don't need to get pad data, only header */
		if(recvfrom(sk, rcv_buff, IP_MIN_HEADER_SIZE + ICMP_HEADER_SIZE, 0,
				(struct sockaddr *)&pinfo->from, NULL) > 0) {
			printf("%d bytes from %s to %s: icmp_seq=%d ttl=%d time=%d ms\n",
					pinfo->padding_size, from_addr_str, dst_addr_str, i + 1, pinfo->ttl, 0);
			cnt_resp++;
		} else {
			printf("Destination Host Unreachable\n");
			cnt_err++;
		}
		sleep(pinfo->interval);
	}
	printf("--- %s ping statistics ---\n", dst_addr_str);
	printf("%d packets transmitted, %d received, %d%% packet loss, time %dms\n",
		cnt_resp + cnt_err, cnt_resp, cnt_err*100/(cnt_err+cnt_resp), 0);

	free(dst_addr_str);
	free(from_addr_str);
	close(sk);
	return 0;
}

static int exec(int argc, char **argv) {
	ping_info_t pinfo;
	int nextOption;
	in_device_t *in_dev = inet_dev_find_by_name("eth0");
	pinfo.count = DEF_COUNT;
	pinfo.interval = DEF_INTERVAL;
	pinfo.padding_size = DEF_PADLEN;
	pinfo.pattern = DEF_PATTERN;
	pinfo.timeout = DEF_TIMEOUT;
	pinfo.ttl = DEF_TTL;
	getopt_init();
	do {
		nextOption = getopt(argc, argv, "I:c:t:W:s:i:p:h");
		switch(nextOption) {
		case 'h':
			show_help();
			return 0;
		case 'I':
			if (NULL == (in_dev = inet_dev_find_by_name(optarg))) {
				printf("ping: unknown iface %s\n", optarg);
				return -1;
			}
			break;
		case 'c':
			if (1 != sscanf(optarg, "%d", &pinfo.count) || pinfo.count < 1) {
				printf("ping: bad number of packets to transmit.\n");
				return -1;
			}
			break;
		case 't':
			if (1 != sscanf(optarg, "%d", &pinfo.ttl)) {
				printf("ping: can't set unicast time-to-live: Invalid argument\n");
				return -1;
			}
			break;
		case 'W':
			if (1 != sscanf(optarg, "%d", &pinfo.timeout)) {
				pinfo.timeout = 1;
			}
			if(pinfo.timeout > MAX_TIMEOUT || pinfo.timeout < 0) {
				printf("ping: bad linger time.\n");
				return -1;
			}
			break;
		case 's':
			if (1 != sscanf(optarg, "%d", &pinfo.padding_size)) {
				pinfo.padding_size = 0;
			}
			if(pinfo.padding_size > MAX_PADLEN) {
				printf("packet size is too large. Maximum is 65507\n");
			} else if(pinfo.padding_size < 0) {
				printf("illegal negative packet size\n");
			}
			break;
		case 'i':
			if (1 != sscanf(optarg, "%d", &pinfo.interval) ||
					pinfo.interval < 0) {
				printf("ping: bad timing interval.\n");
				return -1;
			}
			break;
		case 'p':
			if (1 != sscanf(optarg, "%d", &pinfo.pattern)) {
				printf("ping: patterns must be specified as hex digits.\n");
				return -1;
			}
			break;
		case -1:
			break;
		default:
			return 0;
		}
	} while(-1 != nextOption);

	if (argc == 1) {
		show_help();
		return 0;
	}

	/* get destination addr */
	if (0 == inet_aton(argv[argc - 1], &pinfo.dst)) {
		printf("wrong ip addr format (%s)\n", argv[argc - 1]);
		show_help();
		return -1;
	}
	pinfo.from.s_addr = inet_dev_get_ipaddr(in_dev);
	/* carry out command */
	ping(&pinfo);
	return 0;
}
