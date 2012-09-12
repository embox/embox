/**
 * @file
 * @brief Simple BOOTP client
 *
 * @date 30.07.2012
 * @author Alexander Kalmuk
 */

#include <stdio.h>
#include <errno.h>
#include <embox/cmd.h>
#include <getopt.h>
#include <util/dlist.h>

#include <net/socket.h>
#include <net/inetdevice.h>
#include <net/bootp.h>

#define DEFAULT_WAIT_TIME 3000

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: bootp <ifname>");
}

int bootp_client(int bootp_server_timeout, in_device_t *dev) {
	int res, bootp_sock;
	struct bootphdr bphdr;
	struct sockaddr_in our, dst;
	if (0 > (bootp_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))) {
		printf("Can't to alloc socket\n");
		return bootp_sock;
	}

	socket_set_encap_recv(bootp_sock, bootp_receive);

	our.sin_family = AF_INET;
	our.sin_port = htons((__u16)BOOTP_PORT_CLIENT);
	our.sin_addr.s_addr = htonl(INADDR_ANY);

	res = bind(bootp_sock, (struct sockaddr *) &our, sizeof(our));

	if (res < 0) {
		printf("error at bind()\n");
		goto exit;
	}

	dst.sin_family = AF_INET;
	dst.sin_addr.s_addr = htonl(INADDR_BROADCAST);
	dst.sin_port = htons((__u16)BOOTP_PORT_SERVER);

	setsockopt(bootp_sock, IPPROTO_IP, SO_BINDTODEVICE, dev->dev->name, 6);

	memset(&bphdr, 0, sizeof(bphdr));

	/* request option all option list from server */
	bphdr.options[4] = TAG_DHCP_PARM_REQ_LIST;
	bphdr.options[5] = 0;
	bphdr.options[6] = TAG_END;

	if (0 >= bootp_client_send(bootp_sock, &bphdr, dev->dev, &dst)) {
		printf("Sending error\n");
		goto exit;
	}

	/* TODO set O_NONBLOCK on socket's file descriptor */
	res = recvfrom(bootp_sock, &bphdr, sizeof(bphdr), 0, NULL, NULL);
exit:
	close(bootp_sock);
	return res;
}

static int exec(int argc, char **argv) {
	int opt;
	in_device_t *dev;
	uint32_t bootp_server_timeout = DEFAULT_WAIT_TIME;

	getopt_init();

	while (-1 != (opt = getopt(argc, argv, "h"))) {
		switch (opt) {
		case 'h':
			/* FALLTHROUGH */
		default:
			print_usage();
			return 0;
		}
	}

	if (argc < 2) {
		printf("no interface was specified\n");
		return 0;
	}

	if (NULL == (dev = inet_dev_find_by_name(argv[argc - 1]))) {
		printf("can't find interface %s\n", argv[argc - 1]);
		return 0;
	}

	bootp_client(bootp_server_timeout, dev);

	return 0;
}
