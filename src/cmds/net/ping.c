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
 * @author Daria Dzendzik
 */

#include <embox/cmd.h>

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <fcntl.h>

#include <errno.h>
#include <netdb.h>

#include <net/l3/route.h>
#include <net/l3/icmpv4.h>
#include <net/l3/ipv4/ip.h>
#include <net/inetdevice.h>
#include <net/util/checksum.h>
#include <net/util/macaddr.h>

EMBOX_CMD(exec);

/* Constants */
#define DEFAULT_COUNT    4
#define DEFAULT_PADLEN   64
#define DEFAULT_TIMEOUT  10
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
	struct in_addr dst;  /* Destination host */
};

struct packet_in {
	struct iphdr iph;
	struct icmphdr icmph;
	char pad[MAX_PADLEN];
};

struct packet_out {
	struct icmphdr icmph;
	char pad[MAX_PADLEN];
};

static void print_usage(void) {
	printf("Usage: ping [-c count] [-i interval]\n"
		"            [-p pattern] [-s packetsize] [-t ttl]\n"
		"            [-I interface] [-W timeout] destination\n");
}

static int parse_result(struct packet_in *rx_pack,
		struct packet_out *tx_pack, char *name,
		struct sockaddr_in *to, uint16_t *last_seq,
		uint32_t started, uint32_t interval) {
	uint32_t elapsed;
	char *dst_addr_str;
	int res = 0;
	struct iphdr *emb_iph;
	struct icmphdr *emb_icmph;

	switch (rx_pack->icmph.type) {
	case ICMP_ECHOREPLY:
	    if ((to->sin_addr.s_addr != rx_pack->iph.saddr)
				|| (tx_pack->icmph.un.echo.id
					!= rx_pack->icmph.un.echo.id)
				|| (ntohs(tx_pack->icmph.un.echo.sequence)
					< ntohs(rx_pack->icmph.un.echo.sequence))) {
			break;
		}
		dst_addr_str = inet_ntoa(*(struct in_addr *)&rx_pack->iph.saddr);
		*last_seq = ntohs(rx_pack->icmph.un.echo.sequence);
		printf("%d bytes from %s (%s): icmp_seq=%u ttl=%d ",
				ntohs(rx_pack->iph.tot_len) - (
					IP_MIN_HEADER_SIZE + ICMP_HEADER_SIZE),
				name, dst_addr_str, *last_seq,
				rx_pack->iph.ttl);
		elapsed = clock() - (started + (*last_seq - 1) * interval);
		if (elapsed < 1) {
			printf("time<1 ms\n");
		}
		else {
			printf("time=%d ms\n", elapsed);
		}
		res = 1;
		break;
	case ICMP_DEST_UNREACH:
		emb_iph = (struct iphdr *)((char *)&rx_pack->icmph + ICMP_LEN_DEST_UNREACH);
		emb_icmph = (struct icmphdr *)(emb_iph + 1);
	    if ((to->sin_addr.s_addr != emb_iph->daddr)
				|| (tx_pack->icmph.un.echo.id
					!= emb_icmph->un.echo.id)
				|| (ntohs(tx_pack->icmph.un.echo.sequence)
					< ntohs(emb_icmph->un.echo.sequence))) {
			break;
		}
		dst_addr_str = inet_ntoa(*(struct in_addr *)&rx_pack->iph.saddr);
		*last_seq = ntohs(emb_icmph->un.echo.sequence);
		printf("From %s	icmp_seq=%u %s\n",
				dst_addr_str, *last_seq,
				rx_pack->icmph.code == ICMP_NET_UNREACH
						? "Destination Network Unreachable"
					: rx_pack->icmph.code == ICMP_HOST_UNREACH
						? "Destination Host Unreachable"
					: rx_pack->icmph.code == ICMP_PROT_UNREACH
						? "Destination Protocol Unreachable"
					: rx_pack->icmph.code == ICMP_PORT_UNREACH
						? "Destination Port Unreachable"
					: "unknown icmp_code");
		break;
	default:
		*last_seq = -1;
		printf("ping: ignore icmp_type=%d icmp_type=%d\n",
				rx_pack->icmph.type, rx_pack->icmph.code);
		break;
	}

	return res;
}

static int ping(struct ping_info *pinfo, char *name, char *official_name) {
	uint32_t total, last_sent;
	int i, cnt_resp, cnt_err, sk;
	struct sockaddr_in to;
	struct packet_out *tx_pack = malloc(sizeof *tx_pack);
	struct packet_in *rx_pack = malloc(sizeof *rx_pack);
	uint16_t next_seq, last_seq;

	if (tx_pack == NULL || rx_pack == NULL) {
		printf("packet allocate fail");
		free(tx_pack);
		free(rx_pack);
		return -ENOMEM;
	}

	cnt_resp = 0; cnt_err = 0; next_seq = 0;

	tx_pack->icmph.type = ICMP_ECHO;
	tx_pack->icmph.code = 0;
	tx_pack->icmph.un.echo.id = 11; /* TODO: get unique id */

	/* open socket */
	sk = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sk == -1) {
		printf("socket failed. error=%d\n", sk);
		free(tx_pack);
		return -errno;
	}

	to.sin_family = AF_INET;
	to.sin_addr.s_addr = pinfo->dst.s_addr;
	to.sin_port = 0;

	fcntl(sk, F_SETFD, O_NONBLOCK);

	printf("PING %s (%s) %d bytes of data\n", name, inet_ntoa(pinfo->dst), pinfo->padding_size);

	last_sent = 0;
	total = clock();
	i = 0;
	last_seq = -1;
	while (last_seq != next_seq || i < pinfo->count) {
		if (clock() - last_sent >= pinfo->interval
				&& i < pinfo->count) {
			tx_pack->icmph.un.echo.sequence = htons(++next_seq);
			tx_pack->icmph.checksum = 0;
			tx_pack->icmph.checksum = ptclbsum(tx_pack, ICMP_HEADER_SIZE + pinfo->padding_size);
			sendto(sk, tx_pack, ICMP_HEADER_SIZE + pinfo->padding_size, 0,
					(struct sockaddr *)&to, sizeof to);
			++i;
			last_sent = clock();
		}

		if (clock() - last_sent >= pinfo->timeout) {
			break;
		}

		/* we don't need to get pad data, only header */
		if (recv(sk, rx_pack, sizeof *rx_pack, 0) <= 0) {
			continue;
		}

		/* try to fetch response */
		if (parse_result(rx_pack, tx_pack, official_name, &to,
					&last_seq, total, pinfo->interval)) {
			cnt_resp++; /* if response was fetched proceed */
		} else { /* else output diagnostics */
			cnt_err++;
		}
	}

	free(tx_pack);
	free(rx_pack);

	/* output statistics */
	printf("--- %s ping statistics ---\n", inet_ntoa(pinfo->dst));
	printf("%d packets transmitted, %d received, %d%% packet loss, time %dms\n",
		cnt_resp + cnt_err, cnt_resp, (cnt_err * 100) / (cnt_err + cnt_resp),
		(int)(clock() - total));

	close(sk);
	return 0;
}

static int exec(int argc, char **argv) {
	int opt, i_opt;
	struct in_device *in_dev;
	struct ping_info pinfo;
	int iface_set, cnt_set, ttl_set, tout_set, psize_set, int_set, pat_set, ip_set;
	int garbage, duplicate;
	struct hostent *he = NULL;
	char *hostname = "";
	duplicate = garbage = iface_set = cnt_set = ttl_set = tout_set = psize_set = int_set = pat_set = ip_set = 0;

	in_dev = NULL;
	pinfo.count = DEFAULT_COUNT;
	pinfo.interval = DEFAULT_INTERVAL;
	pinfo.padding_size = DEFAULT_PADLEN;
	pinfo.pattern = DEFAULT_PATTERN;
	pinfo.timeout = DEFAULT_TIMEOUT;
	pinfo.ttl = DEFAULT_TTL;

	getopt_init();
	/* while (-1 != (opt = getopt(argc, argv, "I:c:t:W:s:i:p:h"))) { */
	/* Parse commandline options */
	for (i_opt = 0; i_opt < argc - 1; i_opt++) {
		opt = getopt(argc, argv, "I:c:t:W:s:i:p:h");
		switch (opt) {
		case 'I': /* interface */
			if (!iface_set) { /* is interface already set */
				in_dev = inetdev_get_by_name(optarg);
				if (NULL == in_dev) {
					printf("ping: unknown Iface %s\n", optarg);
					return -EINVAL;
				}
				iface_set = 1; /* now it is set */
			} else
				duplicate = 1; /* so if it is a duplicate output error message */
			i_opt++; /* getopt sacns option and its value, while in argv
				   everything divided by ' ' character is a separate
				   entity*/
			break;
		case 'c': /* count of pings */
			if (!cnt_set) {
				if ((sscanf(optarg, "%d", &pinfo.count) != 1) || (pinfo.count < 1)) {
					printf("ping: bad number of packets to transmit\n");
					return -EINVAL;
				}
				cnt_set = 1;
			} else
				duplicate = 1;
			i_opt++;
			break;
		case 't': /* time to live */
			if (!ttl_set) {
				if (sscanf(optarg, "%d", &pinfo.ttl) != 1) {
					printf("ping: can't set unicast time-to-live: Invalid argument\n");
					return -EINVAL;
				}
				ttl_set = 1;
			} else
				duplicate = 1;
			i_opt++;
			break;
		case 'W': /* timeout */
			if (!tout_set) {
				if ((sscanf(optarg, "%d", &pinfo.timeout) != 1)
						|| (pinfo.timeout < 0)) {
					printf("ping: bad linger time\n");
					return -EINVAL;
				}
				tout_set = 1;
			} else
				duplicate = 1;
			i_opt++;
			break;
		case 's': /* packet size */
			if (!psize_set) {
				if ((sscanf(optarg, "%d", &pinfo.padding_size) != 1)
						|| (pinfo.padding_size < 0)) {
					printf("ping: bad padding size\n");
					return -EINVAL;
				}
				psize_set = 1;
			} else
				duplicate = 1;
			i_opt++;
			if (pinfo.padding_size > MAX_PADLEN) {
				printf("packet size is too large. Maximum is %d\n", MAX_PADLEN);
			}
			break;
		case 'i': /* interval */
			if (!int_set) {
				if ((sscanf(optarg, "%d", &pinfo.interval) != 1) ||
						(pinfo.interval < 0)) {
					printf("ping: bad timing interval.\n");
					return -EINVAL;
				}
				int_set = 1;
			} else
				duplicate = 1;
			i_opt++;
			break;
		case 'p': /* pattern */
			if (!pat_set) {
				if (sscanf(optarg, "%d", &pinfo.pattern) != 1) {
					printf("ping: patterns must be specified as hex digits.\n");
					return -EINVAL;
				}
				pat_set = 1;
			} else
				duplicate = 1;
			i_opt++;
			break;
		case 'h': /* print isage message */
			print_usage();
			return 0;
		case -1: /* non-option argument, should be ip*/
			if (!ip_set) {
				he = gethostbyname(argv[i_opt + 1]);
				if (NULL == he) {
					printf("%s: %s %s\n",
					    argv[0], hstrerror(h_errno), argv[i_opt + 1]);
					return -EINVAL;
				}
				hostname = argv[i_opt + 1];
				pinfo.dst.s_addr = ((struct in_addr *)he->h_addr_list[0])->s_addr;
				ip_set = 1;
			} else
				garbage = 1; /* in case of garbage in option string */
			break;
		default:
			printf("ping: unknown option '%s' or unspecified value\n", argv[i_opt+1]);
			print_usage();
			return 0;
		}
		if (garbage) { /* inform about garbage in commandline */
			printf("ping: info: garbage in option string\n");
		}
		if (duplicate) { /* inform about duplicate options */
			printf("ping: duplicate option '%c'\n", (char)opt);
			return -EINVAL;
		}
	}

	if (!ip_set) { /* if no arguments or no ip address was specified
			output usage message*/
		print_usage();
		return 0;
	}

	pinfo.interval *= 1000;
	pinfo.timeout *= 1000;

	/* ping! */
	return ping(&pinfo, hostname, he->h_name);
}
