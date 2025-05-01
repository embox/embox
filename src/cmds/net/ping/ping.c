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

#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <poll.h>
#include <signal.h>
#include <errno.h>
#include <netdb.h>

#include <util/math.h>
#include <net/l3/route.h>
#include <net/l3/icmpv4.h>
#include <net/l3/ipv4/ip.h>
#include <net/inetdevice.h>
#include <net/util/checksum.h>
#include <net/util/macaddr.h>


/* Constants */
#define DEFAULT_COUNT    4
#define DEFAULT_PADLEN   64
#define DEFAULT_TIMEOUT  10
#define DEFAULT_INTERVAL 1000
#define DEFAULT_PATTERN  0xFF
#define DEFAULT_TTL      59
#define MAX_PADLEN       65507

struct ping_info {
	int count;           /* Stop after sending count ECHO_REQUEST packets. */
	int padding_size;    /* The number of data bytes to be sent. */
	int timeout;         /* Time  to wait for a response, in seconds. */
	int interval;        /* Wait  interval milliseconds between sending each packet. */
	int pattern;         /* Specify up to 16 ``pad'' bytes to fill out the packet to send. */
	int ttl;             /* IP Time to Live. */
	struct in_addr dst;  /* Destination host */
};

struct packet_in {
	struct {
		struct iphdr hdr;
	} __attribute__((packed)) ip;
	struct {
		struct icmphdr hdr;
		union {
			struct icmpbody_echo echo_rep;
			struct icmpbody_dest_unreach dest_unreach;
		} __attribute__((packed)) body;
	} __attribute__((packed)) icmp;
	char data[];
} __attribute__((packed));

struct packet_out {
	struct {
		struct icmphdr hdr;
		union {
			struct icmpbody_echo echo_req;
		} __attribute__((packed)) body;
	} __attribute__((packed)) icmp;
	char data[];
} __attribute__((packed));

struct ping_stat {
	int cnt_request;
	int cnt_replies;
	int cnt_errors;

	clock_t ping_started;
};

static void print_usage(void) {
	printf("Usage: ping [-c count] [-i interval]\n"
		"            [-p pattern] [-s packetsize] [-t ttl]\n"
		"            [-I interface] [-W timeout] destination\n");
}

static void ping_report_stat(struct ping_info *pinfo, struct ping_stat *stat) {
	/* output statistics */
	printf("--- %s ping statistics ---\n", inet_ntoa(pinfo->dst));
	printf("%d packets transmitted, %d received, +%d errors, %d%% packet loss, time %jums\n",
			stat->cnt_request, stat->cnt_replies, stat->cnt_errors,
			(stat->cnt_request - stat->cnt_replies) * 100 / stat->cnt_request,
			(uintmax_t)(clock() - stat->ping_started));
}

static void parse_result(struct packet_in *rx_pack,
		struct packet_out *tx_pack, char *name,
		struct sockaddr_in *to, uint16_t seq,
		clock_t started, struct ping_stat *stat) {
	clock_t elapsed;
	char *dst_addr_str;
	struct iphdr *emb_iph;
	struct icmphdr *emb_icmph;
	struct icmpbody_echo *echo_req;
	struct icmpbody_echo *echo_rep;

	switch (rx_pack->icmp.hdr.type) {
	case ICMP_ECHO_REPLY:
		echo_req = &tx_pack->icmp.body.echo_req;
		echo_rep = &rx_pack->icmp.body.echo_rep;
		if ((to->sin_addr.s_addr != rx_pack->ip.hdr.saddr)
				|| (echo_req->id != echo_rep->id)
				|| (ntohs(echo_req->seq) < ntohs(echo_rep->seq))) {
			break;
		}
		dst_addr_str = inet_ntoa(*(struct in_addr *)&rx_pack->ip.hdr.saddr);
		printf("%u bytes from %s (%s): icmp_seq=%u ttl=%d ",
				(uint16_t)(ntohs(rx_pack->ip.hdr.tot_len)
					- (IP_HEADER_SIZE(&rx_pack->ip.hdr)
						+ ICMP_MIN_HEADER_SIZE)),
				name, dst_addr_str, seq, rx_pack->ip.hdr.ttl);
		elapsed = clock() - started;
		if (elapsed < 1) {
			printf("time<1 ms\n");
		}
		else {
			printf("time=%lu ms\n", elapsed);
		}
		stat->cnt_replies++;
		break;
	case ICMP_DEST_UNREACH:
		emb_iph = (struct iphdr *)&rx_pack->icmp.body.dest_unreach.msg[0];
		emb_icmph = (struct icmphdr *)((void *)emb_iph
					+ IP_HEADER_SIZE(emb_iph));
		if ((to->sin_addr.s_addr != emb_iph->daddr)
				|| (tx_pack->icmp.body.echo_req.id
					!= emb_icmph->body.echo.id)
				|| (ntohs(tx_pack->icmp.body.echo_req.seq)
					< ntohs(emb_icmph->body.echo.seq))) {
			break;
		}
		dst_addr_str = inet_ntoa(*(struct in_addr *)&rx_pack->ip.hdr.saddr);
		printf("From %s icmp_seq=%u %s\n",
				dst_addr_str, seq,
				rx_pack->icmp.hdr.code == ICMP_NET_UNREACH
						? "Destination Network Unreachable"
					: rx_pack->icmp.hdr.code == ICMP_HOST_UNREACH
						? "Destination Host Unreachable"
					: rx_pack->icmp.hdr.code == ICMP_PROT_UNREACH
						? "Destination Protocol Unreachable"
					: rx_pack->icmp.hdr.code == ICMP_PORT_UNREACH
						? "Destination Port Unreachable"
					: "unknown icmp_code");
		stat->cnt_errors++;
		break;
	default:
		printf("ping: ignore icmp_type=%d icmp_code=%d\n",
				rx_pack->icmp.hdr.type, rx_pack->icmp.hdr.code);
	case ICMP_ECHO_REQUEST:
		break;
	}
}

static int ping(struct ping_info *pinfo, char *name, char *official_name, struct in_device *in_dev) {
	struct sockaddr_in to;
	struct ping_stat stat;
	struct packet_out *tx_pack = malloc(sizeof *tx_pack + pinfo->padding_size);
	struct packet_in *rx_pack = malloc(sizeof *rx_pack + pinfo->padding_size);
	clock_t started;
	int sk, ret;
	uint16_t seq;
	int timeout;
	struct pollfd fds;

	if (tx_pack == NULL || rx_pack == NULL) {
		perror("packet allocate fail");
		free(tx_pack);
		free(rx_pack);
		return -ENOMEM;
	}

	tx_pack->icmp.hdr.type = ICMP_ECHO_REQUEST;
	tx_pack->icmp.hdr.code = 0;
	tx_pack->icmp.body.echo_req.id = (uint16_t)getpid();

	/* open socket */
	sk = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sk == -1) {
		printf("socket failed. error=%d\n", sk);
		free(tx_pack);
		free(rx_pack);
		return -errno;
	}

	if (in_dev != NULL) {
		if (-1 == setsockopt(sk, SOL_SOCKET, SO_BINDTODEVICE, &in_dev->dev->name[0], strlen(&in_dev->dev->name[0]))) {
			free(tx_pack);
			free(rx_pack);
			return -errno;
		}
	}

	to.sin_family = AF_INET;
	to.sin_addr.s_addr = pinfo->dst.s_addr;
	to.sin_port = 0;

	fcntl(sk, F_SETFL, O_NONBLOCK);
	signal(SIGINT, SIG_IGN);

	printf("PING %s (%s) %d bytes of data\n", name, inet_ntoa(pinfo->dst), pinfo->padding_size);

	stat.cnt_request = pinfo->count;
	stat.cnt_replies = stat.cnt_errors = 0;
	stat.ping_started = clock();

	fds.fd = sk;
	fds.events = POLLIN;
	timeout = 0;

	for (seq = 1; seq <= pinfo->count; seq++) {
		started = clock();

		tx_pack->icmp.body.echo_req.seq = htons(seq);
		tx_pack->icmp.hdr.check = 0;
		tx_pack->icmp.hdr.check = ptclbsum(tx_pack,
				sizeof(tx_pack->icmp) + pinfo->padding_size);
		if (-1 == sendto(sk, tx_pack,
					sizeof(tx_pack->icmp) + pinfo->padding_size,
					0, (struct sockaddr *)&to, sizeof(to))) {
			perror("ping: sendto() failure");
			ret = -errno;
			goto out;
		}

		timeout = min(pinfo->interval, pinfo->timeout);

		do {

			switch (poll(&fds, 1, timeout)) {
			case 0:
				break;
			case 1:
				if (-1 == recv(sk, rx_pack, sizeof *rx_pack, 0)) {
					perror("ping: recv() failure");
					ret = -errno;
					goto out;
				}
				/* try to fetch response */
				parse_result(rx_pack, tx_pack, official_name,
						&to, seq, started, &stat);
				break;
			default:
				perror("ping: poll failure");
				ret = -errno;
				goto out;
			}
		} while (clock() - started < timeout);

		timeout = clock() - started;
		if (timeout < pinfo->interval) {
			sleep((pinfo->interval - timeout) / 1000);
		}
	}

	ret = stat.cnt_request - stat.cnt_replies;
out:
	free(tx_pack);
	free(rx_pack);
	close(sk);

	ping_report_stat(pinfo, &stat);

	return ret;
}

int main(int argc, char **argv) {
	int opt, i_opt;
	struct in_device *in_dev = NULL;
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
				if (!strncmp(optarg, "0.", 2)) {
					char *curr = optarg + 2, /* skip 0. */
						 *last = curr + 3; /* max precision is 0.001 */
					pinfo.interval = 0;
					while (*curr) {
						if (curr == last) {
							pinfo.interval += (*curr >= '5');
							break;
						}
						if (!isdigit(*curr)) {
							printf("ping: bad timing interval.\n");
							return -EINVAL;
						}
						pinfo.interval *= 10;
						pinfo.interval += *curr++ - '0';
					}
					while (curr++ < last) {
						pinfo.interval *= 10;
					}
				}
				else {
					if ((sscanf(optarg, "%d", &pinfo.interval) != 1) ||
							(pinfo.interval < 0)) {
						printf("ping: bad timing interval.\n");
						return -EINVAL;
					}
					pinfo.interval *= 1000;
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
				if (he == NULL) {
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

	pinfo.timeout *= 1000;

	/* ping! */
	return ping(&pinfo, hostname, he->h_name, in_dev);
}
