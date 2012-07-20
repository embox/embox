/*
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
#include <net/in.h>
#include <net/socket.h>
#include <net/ntp.h>

#include <time.h>
#include <kernel/time/time.h>
#include <kernel/time/timer.h>

#define NTP_PORT 123
#define DEFAULT_WAIT_TIME 1000

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: ntpdate [-q] server");
}

void wake_on_server_resp(struct sys_timer *timer, void *param) {
	*(int*)param = false;
}

int ntpdate_common(char *dstip, int ntp_server_timeout, struct ntphdr *r) {
	int sock, res;
	struct sockaddr_in our, dst;
	struct sys_timer ntp_timer;
	bool wait_response = true; /* wait for server response */

	if (!inet_aton(dstip, &dst.sin_addr)) {
		printf("Invalid ip address %s\n", dstip);
		return -ENOENT;
	}

	if (0 > (sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))) {
		printf("Can't to alloc socket\n");
		return sock;
	}

	socket_set_encap_recv(sock, ntp_client_receive);

	our.sin_family = AF_INET;
	/* FIXME */
	our.sin_port = htons(512);
	our.sin_addr.s_addr = htonl(INADDR_ANY);

	res = bind(sock, (struct sockaddr *)&our, sizeof(our));

	if (res < 0) {
		printf("error at bind() %d!\n", res);
		goto error;
	}

	dst.sin_family = AF_INET;
	dst.sin_port = htons((__u16)NTP_PORT);

	if (0 >= (res = ntp_client_xmit(sock, &dst))) {
		printf("Sending error\n");
		goto error;
	}

	/* wait for server response or quit with timeout */
	timer_init(&ntp_timer, 0, ntp_server_timeout, wake_on_server_resp, &wait_response);

	while (0 >= (res = recvfrom(sock, r, sizeof(struct ntphdr), 0, NULL, NULL))
			&& wait_response);

	close(sock);
	timer_close(&ntp_timer);

	if (res <= 0) {
		printf("Server timeout\n");
		timer_close(&ntp_timer);
		goto error;
	}

	printf("server %s, stratum %d\n", dstip, (int)(r->stratum));

	return ENOERR;

error:
	close(sock);
	return res;
}

static int exec(int argc, char **argv) {
	int opt, res;
	struct timespec ts;
	bool query = false;
	struct ntphdr r;
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
			break;
		}
	}

	res = ntpdate_common(argv[argc - 1], ntp_server_timeout, &r);

	if (res >= 0 && !query) {
		ts.tv_sec = ntohl(r.xmt_ts.sec);
		/* convert pico to nanoseconds (RFC 5905, data types) */
		ts.tv_nsec = (r.xmt_ts.fraction * 1000) / 232;
		settimeofday(&ts, NULL);
	}

	return 0;
}
