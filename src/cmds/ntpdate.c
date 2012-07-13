/*
 * @brief NTPv4 client
 *
 * @date 13.07.2012
 * @author Alexander Kalmuk
 */

#include <kernel/prom_printf.h>
#include <embox/cmd.h>
#include <getopt.h>
#include <net/ip.h>
#include <net/in.h>
#include <net/socket.h>

#include <net/ntp.h>
static struct ntphdr x;
char buf[sizeof(x)];
static struct ntphdr *r = (struct ntphdr *)buf;

#define NTP_PORT 123

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: ntpdate [-q] server");
}

int ntpdate(char *dstip) {
	int sock;
	struct sockaddr_in dst;

	if (!inet_aton(dstip, &dst.sin_addr)) {
		prom_printf("Error: Invalid ip address '%s'", dstip);
		return -1;
	}

	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	dst.sin_family = AF_INET;
	dst.sin_port = htons((__u16)NTP_PORT);

	/* TODO */
	x.status = NTP_CLOCK_UNSYNC | NTP_V_4 | NTP_CLIENT;
	x.stratum = NTP_SERVER_UNSPEC;
	x.poll = 3;
	x.precision = 0xfa;
	x.rootdelay.sec = 1;
	x.rootdisp.sec = 1;
	x.xmt_ts.sec = (__u32)0xD3AA9D5E;
	x.xmt_ts.fraction = 0;

	if (0 >= sendto(sock, (void*) &x, sizeof(x), 0, (struct sockaddr *)&dst, sizeof(dst))) {
		prom_printf("%s\n", "Sending error");
	}

	while(0 == recvfrom(sock, buf, sizeof(x), 0, NULL, NULL));

	r = (struct ntphdr *) buf;

	prom_printf("%d %d\n", (int)r->stratum, (int)r->status);

	printf("%s",buf);

	return 0;
}

static int exec(int argc, char **argv) {
	int opt;

	getopt_init();

	while (-1 != (opt = getopt(argc, argv, "hq:t"))) {
		printf("\n");
		switch (opt) {
		case 'h':
			print_usage();
			return 0;
		case 'q':
			return ntpdate(argv[2]);
		default:
			break;
		}
	}

	return 0;
}

