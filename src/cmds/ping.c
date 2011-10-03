/**
 * @file
 * @brief send ICMP ECHO_REQUEST to network hosts.
 *
 * @date 20.03.09
 * @author Anton Bondarev
 * @author Nikolay Korotky
 * 	- implement ping through raw socket.
 * 	- major refactoring
 * @author Ilia Vaprol
 */

#include <embox/cmd.h>
#include <getopt.h>
#include <net/icmp.h>
#include <net/ip.h>
#include <net/inetdevice.h>
#include <net/checksum.h>
#include <net/util.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <err.h>

EMBOX_CMD(exec);

/* Constants */
#define DEFAULT_COUNT    4
#define DEFAULT_PADLEN   56
#define DEFAULT_TIMEOUT  1
#define DEFAULT_INTERVAL 1
#define DEFAULT_PATTERN  0xFF
#define DEFAULT_TTL      59
#define MAX_PADLEN       65507

struct ping_info {
	int count;           /* Stop after sending count ECHO_REQUEST packets. */
	int padding_size;    /* The number of data bytes to be sent. */
	int timeout;         /* Time  to wait for a response, in seconds. */
	int interval;        /* Wait  interval seconds between sending each packet. */
	int pattern;         /* Specify up to 16 ``pad'' bytes to fill out the packet to send. */
	int ttl;             /* IP Time to Live. */
	struct in_addr from; /* Source address to specified interface address. */
	struct in_addr dst;  /* Destination host */
} ping_info_t;

static void print_usage(void) {
}

static int ping(struct ping_info *pinfo) {
	int cnt_resp = 0, cnt_err = 0, sk;
	size_t i;
	iphdr_t *iph_s, *iph_r;
	icmphdr_t *icmph_s, *icmph_r;
	char *dst_addr_str;
	char packet[IP_MIN_HEADER_SIZE + ICMP_HEADER_SIZE + MAX_PADLEN];
	char rcv_buff[IP_MIN_HEADER_SIZE + ICMP_HEADER_SIZE + MAX_PADLEN];
	uint32_t timeout, start, delta, total;

	timeout = pinfo->timeout * 1000;
	dst_addr_str = inet_ntoa(pinfo->dst);
	printf("PING %s %d bytes of data.\n", dst_addr_str, pinfo->padding_size);

	if ((sk = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
		LOG_ERROR("socket error\n");
		return -1;
	}
	iph_s = (iphdr_t *) packet;
	icmph_s = (icmphdr_t *) (packet + IP_MIN_HEADER_SIZE);
	memset(packet, 0, IP_MIN_HEADER_SIZE + ICMP_HEADER_SIZE + pinfo->padding_size);
	memset(icmph_s, pinfo->pattern, pinfo->padding_size);
	iph_s->version = 4;
	iph_s->ihl = IP_MIN_HEADER_SIZE >> 2;
	iph_s->tos = 0;
	iph_s->frag_off = htons(IP_DF);
//	iph_s->saddr = pinfo->from.s_addr;
	iph_s->daddr = pinfo->dst.s_addr;
	iph_s->tot_len = htons(IP_MIN_HEADER_SIZE + ICMP_HEADER_SIZE + pinfo->padding_size);
	iph_s->ttl = pinfo->ttl;
	iph_s->proto = IPPROTO_ICMP;
	icmph_s->type = ICMP_ECHO;
	icmph_s->code = 0;
	icmph_s->un.echo.id = /*TID*/0;

	/* set header for recive packet */
	iph_r = (iphdr_t *) rcv_buff;
	icmph_r = (icmphdr_t *) (rcv_buff + IP_MIN_HEADER_SIZE);

	total = clock();
	i = 0;
	for (;;) {
		icmph_s->un.echo.sequence = htons(ntohs(icmph_s->un.echo.sequence) + 1);
		icmph_s->checksum = 0;
		icmph_s->checksum = htons(ptclbsum(packet + IP_MIN_HEADER_SIZE,
						ICMP_HEADER_SIZE + pinfo->padding_size));

		sendto(sk, packet, ntohs(iph_s->tot_len), 0, (struct sockaddr *)&pinfo->dst, 0);

		++i;
		start = clock();
		while ((delta = clock() - start) < timeout) {
			/* we don't need to get pad data, only header */
			if (!recvfrom(sk, rcv_buff, IP_MIN_HEADER_SIZE + ICMP_HEADER_SIZE,
					0, (struct sockaddr *)&pinfo->from, NULL)) {
				continue;
			}
			if ((icmph_r->type != ICMP_ECHOREPLY) || (icmph_s->un.echo.sequence != icmph_r->un.echo.sequence)
					||(iph_s->daddr != iph_r->saddr)) {
				continue;
			}
			if (delta < 1) {
				printf("%d bytes from %s: icmp_seq=%d ttl=%d time <1ms\n",
						pinfo->padding_size, dst_addr_str, i, pinfo->ttl);
			}
			else {
				printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%d ms\n",
						pinfo->padding_size, dst_addr_str, i, pinfo->ttl, delta);
			}
			cnt_resp++;
			break;
		}
		if (delta >= timeout) {
			printf("From %s icmp_seq=%d Destination Host Unreachable\n", dst_addr_str, i);
			cnt_err++;
		}
		if (i >= pinfo->count) {
			break;
		}
		sleep(pinfo->interval);
	}
	printf("--- %s ping statistics ---\n", dst_addr_str);
	printf("%d packets transmitted, %d received, %d%% packet loss, time %dms\n",
		cnt_resp + cnt_err,
		cnt_resp,
		(cnt_err * 100) / (cnt_err + cnt_resp),
		(int)(clock() - total));

	close(sk);
	return 0;
}

static int exec(int argc, char **argv) {
	int opt;
	in_device_t *in_dev;
	struct ping_info pinfo;

	in_dev = NULL;
	pinfo.count = DEFAULT_COUNT;
	pinfo.interval = DEFAULT_INTERVAL;
	pinfo.padding_size = DEFAULT_PADLEN;
	pinfo.pattern = DEFAULT_PATTERN;
	pinfo.timeout = DEFAULT_TIMEOUT;
	pinfo.ttl = DEFAULT_TTL;
	getopt_init();
	do {
		opt = getopt(argc, argv, "I:c:t:W:s:i:p:");
		switch(opt) {
			case 'I':
				in_dev = inet_dev_find_by_name(optarg);
				if (in_dev == NULL) {
					printf("ping: unknown Iface %s\n", optarg);
					return -1;
				}
				break;
			case 'c':
				if ((sscanf(optarg, "%d", &pinfo.count) != 1)  || (pinfo.count < 1)) {
					printf("ping: bad number of packets to transmit\n");
					return -1;
				}
				break;
			case 't':
				if (sscanf(optarg, "%d", &pinfo.ttl) != 1) {
					printf("ping: can't set unicast time-to-live: Invalid argument\n");
					return -1;
				}
				break;
			case 'W':
				if ((sscanf(optarg, "%d", &pinfo.timeout) != 1)
						|| (pinfo.timeout < 0)) {
					printf("ping: bad linger time\n");
					return -1;
				}
				break;
			case 's':
				if ((sscanf(optarg, "%d", &pinfo.padding_size) != 1)
						|| (pinfo.padding_size < 0)) {
					printf("ping: bad padding size\n");
					return -1;
				}
				if (pinfo.padding_size > MAX_PADLEN) {
					printf("packet size is too large. Maximum is %d\n", MAX_PADLEN);
				}
				break;
			case 'i':
				if ((sscanf(optarg, "%d", &pinfo.interval) != 1) ||
						(pinfo.interval < 0)) {
					printf("ping: bad timing interval.\n");
					return -1;
				}
				break;
			case 'p':
				if (sscanf(optarg, "%d", &pinfo.pattern) != 1) {
					printf("ping: patterns must be specified as hex digits.\n");
					return -1;
				}
				break;
			case -1:
				break;
			default:
				return 0;
		}
	} while (opt != -1);

	if (argc == 1) {
		printf("Usage: ping [-I if] [-c cnt] [-W timeout] [-t ttl]\n"
			"[-i interval] [-p pattern] [-s packetsize] destination\n");
		return 0;
	}

//	inet_dev_find_by_name("eth0");
	/* Get destination addr */
	if (inet_aton(argv[argc - 1], &pinfo.dst) == 0) {
		printf("wrong ip addr format (%s)\n", argv[argc - 1]);
		print_usage();
		return -1;
	}

	/* Get source addr */
	if (in_dev != NULL) {
		pinfo.from.s_addr = inet_dev_get_ipaddr(in_dev);
	}
	else {
		pinfo.from.s_addr = 0;
	}

	ping(&pinfo);

	return 0;
}
