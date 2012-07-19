/*
 * @brief SNTPv4 client
 *
 * @date 13.07.2012
 * @author Alexander Kalmuk
 */

#include <embox/cmd.h>
#include <getopt.h>
#include <net/ip.h>
#include <net/in.h>
#include <net/socket.h>
#include <stdio.h>
#include <err.h>
#include <errno.h>
#include <time.h>
#include <kernel/time/time.h>

#include <kernel/time/timer.h>
#include <net/ntp.h>

#define NTP_PORT 123
#define DEFAULT_WAIT_TIME 1000

static struct ntphdr x;
static char buf[sizeof(x)];
static struct ntphdr *r = (struct ntphdr *) buf;

static uint32_t ntp_server_timeout = DEFAULT_WAIT_TIME;
static struct sys_timer ntp_timer;

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: ntpdate [-q] server");
}

void wake_on_server_resp(struct sys_timer *timer, void *param) {
	*(int*)param = false;
}

int ntpdate_common(char *dstip) {
	int sock, res;
	struct sockaddr_in our, dst;
	struct timespec ts;
	bool wait_response = true; /* wait for server response */

	if (!inet_aton(dstip, &dst.sin_addr)) {
		printf("Error: Invalid ip address '%s'", dstip);
		return -1;
	}

	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	our.sin_family = AF_INET;
	/* FIXME */
	our.sin_port = htons(512);
	our.sin_addr.s_addr = htonl(INADDR_ANY);

	res = bind(sock, (struct sockaddr *)&our, sizeof(our));

	if (res < 0) {
		printf("error at bind() %d!\n", res);
		return res;
	}

	dst.sin_family = AF_INET;
	dst.sin_port = htons((__u16)NTP_PORT);

	/* TODO */
	x.status = NTP_CLOCK_UNSYNC | NTP_V_4 | NTP_CLIENT;
	x.stratum = NTP_SERVER_UNSPEC;
	x.poll = 3;
	x.precision = 0xfa;
	x.rootdelay.sec = htons(0x0001);
	x.rootdisp.sec = htons(0x0001);
	gettimeofday(&ts, NULL);
	x.xmt_ts.sec = htonl(ts.tv_sec);
	x.xmt_ts.fraction = 0;

	if (0 >= sendto(sock, (void*) &x, sizeof(x), 0, (struct sockaddr *)&dst, sizeof(dst))) {
		printf("%s\n", "Sending error");
	}

	/* wait for server response or quit with timeout */
	timer_init(&ntp_timer, 0, ntp_server_timeout, wake_on_server_resp, &wait_response);
	while (!(res = recvfrom(sock, buf, sizeof(x), 0, NULL, NULL)) && wait_response);

	close(sock);
	timer_close(&ntp_timer);

	if (res <= 0) {
		printf("Server timeout\n");
		return -ETIMEDOUT;
	}

	printf("server %s, stratum %d\n", dstip, (int)(r->stratum));

	return ENOERR;
}

static int exec(int argc, char **argv) {
	int opt;
	struct timespec ts;
	bool query = false;

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

	ntpdate_common(argv[argc - 1]);

	if (!query) {
		ts.tv_sec = ntohl(r->xmt_ts.sec);
		/* convert pico to nanoseconds (RFC 5905, data types) */
		ts.tv_nsec = (r->xmt_ts.fraction * 1000) / 232;
		settimeofday(&ts, NULL);
	}

	return 0;
}
