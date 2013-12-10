/**
 * @file
 * @brief Simple BOOTP client
 *
 * @date 30.07.12
 * @author Alexander Kalmuk
 * @author Ilia Vaprol
 */

#include <arpa/inet.h>
#include <embox/cmd.h>
#include <errno.h>
#include <framework/mod/options.h>
#include <net/inetdevice.h>
#include <net/l3/route.h>
#include <net/lib/bootp.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <util/array.h>
#include <kernel/time/time.h>

EMBOX_CMD(exec);

#define MODOPS_TIMEOUT OPTION_GET(NUMBER, timeout)

static void print_usage(void) {
	printf("Usage: bootpc <ifname>\n");
}

static int bootp_prepare(struct net_device *dev) {
	struct in_device *in_dev;
	int ret;

	in_dev = inetdev_get_by_dev(dev);
	if (in_dev == NULL) {
		return -EINVAL;
	}

	ret = inetdev_set_addr(in_dev, 0);
	if (ret) {
		return ret;
	}

	ret = netdev_flag_up(dev, IFF_UP);
	if (ret) {
		return ret;
	}

	return 0;
}

static int bootp_process(struct bootphdr *bph,
		struct net_device *dev) {
	struct in_device *in_dev;
	in_addr_t mask;
	const unsigned char *tag;

	in_dev = inetdev_get_by_dev(dev);
	if (in_dev == NULL) {
		return -EINVAL;
	}

	if (bph->ciaddr == 0) {
		inetdev_set_addr(in_dev, bph->yiaddr);
	}

	tag = &bph->vend[0] + sizeof dhcp_magic_cookie;
	while ((tag < &bph->vend[ARRAY_SIZE(bph->vend)])
			&& (*tag != TAG_END)) {
		switch(*tag) {
		default:
			tag++;
			break;
		case TAG_PAD:
			tag++;
			break;
		case TAG_GATEWAY: /* add default route */
			tag += 2;
			rt_add_route(in_dev->dev, INADDR_ANY,
						INADDR_ANY, *(in_addr_t *)tag, 0);
			tag += sizeof(in_addr_t);
			break;
		case TAG_SUBNET_MASK: /* set mask */
			tag += 2;
			mask = *(in_addr_t*)tag;
			inetdev_set_mask(in_dev, mask);
			rt_add_route(in_dev->dev, bph->siaddr & mask, mask,
						INADDR_ANY, 0);
			tag += sizeof(in_addr_t);
			break;
		}
	}

	return 0;
}

int bootp_client(struct net_device *dev) {
	static const struct timeval timeout = {
		MODOPS_TIMEOUT / MSEC_PER_SEC,
		(MODOPS_TIMEOUT % MSEC_PER_SEC) * USEC_PER_MSEC
	};
	int ret, sock;
	struct bootphdr bph_req, bph_rep;
	struct sockaddr_in addr;

	ret = bootp_prepare(dev);
	if (ret) {
		return ret;
	}

	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == -1) {
		return -errno;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(BOOTP_CLIENT_PORT);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (-1 == bind(sock, (struct sockaddr *)&addr, sizeof addr)) {
		ret = -errno;
		goto error;
	}

	if (-1 == setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE,
				&dev->name[0], strlen(&dev->name[0]))) {
		ret = -errno;
		goto error;
	}
	if (-1 == setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,
				&timeout, sizeof timeout)) {
		ret = -errno;
		goto error;
	}

	ret = bootp_build(&bph_req, BOOTPREQUEST, dev->type,
			dev->addr_len, &dev->dev_addr[0]);
	if (ret != 0) {
		ret = -errno;
		goto error;
	}

	/* request option all option list from server */
	bph_req.vend[4] = TAG_DHCP_PARM_REQ_LIST;
	bph_req.vend[5] = 0;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(BOOTP_SERVER_PORT);
	addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);

	if (-1 == sendto(sock, &bph_req, sizeof bph_req, 0,
				(struct sockaddr *)&addr, sizeof addr)) {
		ret = -errno;
		goto error;
	}

	if (-1 == recv(sock, &bph_rep, sizeof bph_rep, 0)) {
		ret = -errno;
		goto error;
	}

	ret = bootp_valid(&bph_rep, BOOTPREPLY);
	if (ret != 0) {
		goto error;
	}

	ret = bootp_process(&bph_rep, dev);
error:
	close(sock);
	return ret;
}

static int exec(int argc, char **argv) {
	int opt;
	struct net_device *dev;

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

	dev = netdev_get_by_name(argv[argc - 1]);
	if (dev == NULL) {
		printf("can't find interface %s\n", argv[argc - 1]);
		return -EINVAL;
	}

	return bootp_client(dev);
}
