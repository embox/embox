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
#include <unistd.h>
#include <util/dlist.h>

#include <sys/socket.h>
#include <net/socket.h>
#include <net/inetdevice.h>
#include <net/bootp.h>
#include <unistd.h>

#define DEFAULT_WAIT_TIME 3000

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: bootp <ifname>\n");
}

int bootp_client(int bootp_server_timeout, struct in_device *dev) {
	int res, bootp_sock;
	struct bootphdr bphdr;
	struct sockaddr_in our, dst;
	if (0 > (bootp_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))) {
		res = -errno;
		printf("Can't to alloc socket\n");
		return res;
	}

	socket_set_encap_recv(bootp_sock, bootp_receive);

	our.sin_family = AF_INET;
	our.sin_port = htons((__u16)BOOTP_PORT_CLIENT);
	our.sin_addr.s_addr = htonl(INADDR_ANY);

	res = bind(bootp_sock, (struct sockaddr *) &our, sizeof(our));

	if (res < 0) {
		res = -errno;
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
	do {
		res = recvfrom(bootp_sock, &bphdr, sizeof(bphdr), 0, NULL, NULL);
	} while (res == 0);
	if (res == -1) {
		res = -errno;
	}
	else {
		res = 0;
	}

exit:
	close(bootp_sock);
	return res;
}

static int exec(int argc, char **argv) {
	int opt;
	struct in_device *dev;
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
		return -EINVAL;
	}

	if (NULL == (dev = inetdev_get_by_name(argv[argc - 1]))) {
		printf("can't find interface %s\n", argv[argc - 1]);
		return -EINVAL;
	}

	return bootp_client(bootp_server_timeout, dev);
}
