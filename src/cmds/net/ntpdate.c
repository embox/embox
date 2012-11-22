/**
 * @file
 * @brief SNTPv4 client
 *
 * @date 13.07.2012
 * @author Alexander Kalmuk
 */

#include <stdio.h>
#include <err.h>
#include <errno.h>
#include <embox/cmd.h>
#include <getopt.h>

#include <net/ip.h>
#include <arpa/inet.h>
#include <net/socket.h>
#include <net/ntp.h>

#include <time.h>
#include <kernel/time/time.h>

#define DEFAULT_WAIT_TIME 10000

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: ntpdate [-q] server");
}

int ntpdate_common(char *dstip, int ntp_server_timeout, struct ntphdr *r, bool query) {
	int sock, res;
	struct sockaddr_in our, dst;

	if (!inet_aton(dstip, &dst.sin_addr)) {
		printf("Invalid ip address %s\n", dstip);
		return -ENOENT;
	}

	if (0 > (sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))) {
		printf("Can't to alloc socket\n");
		return sock;
	}

	socket_set_encap_recv(sock, ntp_receive);

	our.sin_family = AF_INET;
	our.sin_addr.s_addr = htonl(INADDR_ANY);

	if (!query) {
		/* we want, that ntp module set system time if receive request. */
		our.sin_port = htons(NTP_SERVER_PORT);
	} else {
		/* FIXME any free port except NTP_SERVER_PORT */
		our.sin_port = htons(768);
	}

	res = bind(sock, (struct sockaddr *)&our, sizeof(our));

	if (res < 0) {
		printf("error at bind()\n");
		goto error;
	}

	dst.sin_family = AF_INET;
	dst.sin_port = htons((__u16)NTP_SERVER_PORT);

	if (0 >= (res = ntp_client_xmit(sock, &dst))) {
		/* FIXME  It is not sending error always. In case, if packet sends by us at first,
		 * it put in arp_queue and we will think that it is sending error. */
		printf("Sending error\n");
		goto error;
	}

	/* TODO set O_NONBLOCK on socket's file descriptor */
	res = recvfrom(sock, r, sizeof(struct ntphdr), 0, NULL, NULL);
error:
	close(sock);
	return res;
}

static int exec(int argc, char **argv) {
	int opt;
	struct ntphdr r;
	struct timespec delay;
	bool query = false;
	uint32_t ntp_server_timeout = DEFAULT_WAIT_TIME;

	getopt_init();

	while (-1 != (opt = getopt(argc, argv, "hq:t:"))) {
		printf("\n");
		switch (opt) {
		case 'h':
			print_usage();
			return 0;
		case 'q': /* Query only - don't set the clock */
			query = true;
			break;
		case 't': /* Maximum time waiting for a server response */
			if (1 != sscanf(optarg, "%d", &ntp_server_timeout)) {
				LOG_ERROR("wrong -t argument %s\n", optarg);
				return -1;
			}
			break;
		default:  /* Set system clock */
			return 0;
			break;
		}
	}

	if (0 > ntpdate_common(argv[argc - 1], ntp_server_timeout, &r, query)) {
		return 0;
	}

	if (query) {
		delay = ntp_delay(&r);
		printf("server %s, stratum %d, delay %d:%d (s:ms)\n", argv[argc - 1], (int)(r.stratum),
				(int)delay.tv_sec, (int)delay.tv_nsec / 1000);
	}

	return 0;
}
