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
 *
 * @author Daria Dzendzik
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
#include <fcntl.h>
#include <err.h>

EMBOX_CMD(exec);


/* Constants */
#define DEFAULT_COUNT    4
#define DEFAULT_PADLEN   64
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
};

union packet {
	char packet_buff[IP_MIN_HEADER_SIZE + ICMP_HEADER_SIZE + MAX_PADLEN];
	struct {
		iphdr_t ip_hdr;
		icmphdr_t icmp_hdr;
	} hdr;
};

static void print_usage(void) {
	printf("Usage: ping [-c count] [-i interval]\n"
		"            [-p pattern] [-s packetsize] [-t ttl]\n"
		"            [-I interface] [-W timeout] destination\n");
}

static int sent_resalt(int sock, uint32_t timeout, union packet *ptx_pack) {
	uint32_t start, delta;
	union packet rx_pack;
	struct sockaddr_in from;
	char *dst_addr_str;

	start = clock();
	while ((delta = clock() - start) < timeout) {
		/* we don't need to get pad data, only header */
		if (!recvfrom(sock, rx_pack.packet_buff, IP_MIN_HEADER_SIZE + ICMP_HEADER_SIZE, 0,
				(struct sockaddr *)&from, NULL)) {
			continue;
		}
		if ((rx_pack.hdr.icmp_hdr.type != ICMP_ECHOREPLY) ||
				(ptx_pack->hdr.ip_hdr.daddr != rx_pack.hdr.ip_hdr.saddr) ||
				(ptx_pack->hdr.icmp_hdr.un.echo.id != rx_pack.hdr.icmp_hdr.un.echo.id) ||
				(ptx_pack->hdr.icmp_hdr.un.echo.sequence != rx_pack.hdr.icmp_hdr.un.echo.sequence)) {
			continue;
		}
		dst_addr_str = inet_ntoa(*(struct in_addr *)&rx_pack.hdr.ip_hdr.saddr);
		if (delta < 1) {
			printf("%d bytes from %s: icmp_seq=%d ttl=%d time <1ms\n",
					ntohs(rx_pack.hdr.ip_hdr.tot_len) - IP_MIN_HEADER_SIZE - ICMP_HEADER_SIZE,
					dst_addr_str, ntohs(rx_pack.hdr.icmp_hdr.un.echo.sequence), rx_pack.hdr.ip_hdr.ttl);
		}
		else {
			printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%d ms\n",
					ntohs(rx_pack.hdr.ip_hdr.tot_len) - IP_MIN_HEADER_SIZE - ICMP_HEADER_SIZE,
					dst_addr_str, ntohs(rx_pack.hdr.icmp_hdr.un.echo.sequence), rx_pack.hdr.ip_hdr.ttl, delta);
		}
		return 1;
	}
	return 0;
}


static int ping(struct ping_info *pinfo) {
	uint32_t timeout, total;
	size_t i;
	int cnt_resp, cnt_err, sk;
	union packet tx_pack;

	tx_pack.hdr.ip_hdr.version = 4;
	tx_pack.hdr.ip_hdr.ihl = IP_MIN_HEADER_SIZE >> 2;
	tx_pack.hdr.ip_hdr.tos = 0;
	tx_pack.hdr.ip_hdr.frag_off = htons(IP_DF);
	tx_pack.hdr.ip_hdr.saddr = pinfo->from.s_addr;
	tx_pack.hdr.ip_hdr.daddr = pinfo->dst.s_addr;
	tx_pack.hdr.ip_hdr.tot_len = htons(IP_MIN_HEADER_SIZE + ICMP_HEADER_SIZE + pinfo->padding_size);
	tx_pack.hdr.ip_hdr.ttl = pinfo->ttl;
	tx_pack.hdr.ip_hdr.proto = IPPROTO_ICMP;

	cnt_resp = 0; cnt_err = 0;

	timeout = pinfo->timeout * 1000;
	printf("PING %s %d bytes of data.\n", inet_ntoa(pinfo->dst), pinfo->padding_size);

	if ((sk = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
		LOG_ERROR("socket error\n");
		return -1;
	}

	tx_pack.hdr.icmp_hdr.type = ICMP_ECHO;
	tx_pack.hdr.icmp_hdr.code = 1;
	tx_pack.hdr.icmp_hdr.un.echo.id = /*TID*/0;
	tx_pack.hdr.icmp_hdr.un.echo.sequence = 0;

	total = clock();
	i = 0;
	for (;;) {
		tx_pack.hdr.icmp_hdr.un.echo.sequence = htons((unsigned)ntohs(tx_pack.hdr.icmp_hdr.un.echo.sequence) + 1);
		tx_pack.hdr.icmp_hdr.checksum = 0;
		/* TODO checksum must be at network byte order */
		/* XXX linux-0.2.img sends checksum in host byte order,
		 * but it's wrong */
		tx_pack.hdr.icmp_hdr.checksum = ptclbsum(tx_pack.packet_buff + IP_MIN_HEADER_SIZE,
						ICMP_HEADER_SIZE + pinfo->padding_size);

		sendto(sk, tx_pack.packet_buff, ntohs(tx_pack.hdr.ip_hdr.tot_len), 0, (struct sockaddr *)&pinfo->dst, 0);

		++i;
		if (sent_resalt(sk, timeout, &tx_pack)) {
			cnt_resp++;
		}
		else {
			printf("From %s icmp_seq=%d Destination Host Unreachable\n", inet_ntoa(pinfo->dst), i); // TODO
			cnt_err++;
		}
		if (i >= pinfo->count) {
				break;
		}
		sleep(pinfo->interval);
	}
	printf("--- %s ping statistics ---\n", inet_ntoa(pinfo->dst));
	printf("%d packets transmitted, %d received, %d%% packet loss, time %dms\n",
		cnt_resp + cnt_err, cnt_resp, (cnt_err * 100) / (cnt_err + cnt_resp),
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
	while (-1 != (opt = getopt(argc, argv, "I:c:t:W:s:i:p:h"))) {
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
		case 'h':
			print_usage();
			return 0;
		default:
			return 0;
		}
	}

	if (argc == 1) {
		print_usage();
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
	} else {
		pinfo.from.s_addr = 0;
	}

	ping(&pinfo);

	return 0;
}
