/**
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

#define DEFAULT_WAIT_TIME 10000

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: ntpdate [-q] server");
}

static void wake_on_server_resp(struct sys_timer *timer, void *param) {
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

	socket_set_encap_recv(sock, ntp_receive);

	our.sin_family = AF_INET;
	/* FIXME */
	our.sin_port = htons(768);
	our.sin_addr.s_addr = htonl(INADDR_ANY);

	res = bind(sock, (struct sockaddr *)&our, sizeof(our));

	if (res < 0) {
		printf("error at bind()\n");
		goto error;
	}

	dst.sin_family = AF_INET;
	dst.sin_port = htons((__u16)NTP_SERVER_PORT);

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

	return ENOERR;

error:
	close(sock);
	return res;
}

static int exec(int argc, char **argv) {
	int opt;
	struct ntphdr r;
	struct timespec ts, delay;
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

	if (0 > ntpdate_common(argv[argc - 1], ntp_server_timeout, &r)) {
		return 0;
	}

	delay = ntp_delay(&r);
	printf("server %s, stratum %d, delay %d:%d (s:ms)\n", argv[argc - 1], (int)(r.stratum),
			(int)delay.tv_sec, (int)delay.tv_nsec / 1000);

	if (!query) {
		ts = ntp_to_timespec(r.xmt_ts);
		ts = timespec_add(ts, delay);
		settimeofday(&ts, NULL);
	}

	return 0;
}
