/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    15.12.2013
 */

#include "host_defs.h"
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stropts.h>

#include <net/if.h>
#include <linux/if_tun.h>
#include <linux/ioctl.h>

#include <host.h>

#define TAP_DEFAULT_DEV_NAME "tap77"
#define TAP_DEV_ENV_NAME "EMBOX_USERMODE_TAP_NAME"

static HOST_FNX(int, open, CONCAT(const char *path, int flags),
		path, flags)
static HOST_FNX(int, ioctl, CONCAT(int fd, int req, void *arg),
		fd, req, arg)
static HOST_FNX(int, writev, CONCAT(int fd, struct iovec *iov, int iovcnt), fd, iov, iovcnt)
static HOST_FNX(int, fcntl, CONCAT(int fd, int req, int arg),
		fd, req, arg)
static HOST_FNX(int, close, int fd, fd)
static HOST_FNX(int, getpid, void)
static HOST_FNX(char *, getenv, const char *name, name)

static int tun_alloc(const char *dev, char *out_dev, int len) {
	struct ifreq ifr;
	int fd, err;

	if ((fd = host_open("/dev/net/tun", O_RDWR)) < 0) {
		return -errno;
	}

	memset(&ifr, 0, sizeof(ifr));
	ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
	if (dev)
		strncpy(ifr.ifr_name, dev, IFNAMSIZ);

	if ((err = host_ioctl(fd, TUNSETIFF, &ifr)) < 0) {
		host_close(fd);
		return -errno;
	}

	if (out_dev)
		strncpy(out_dev, ifr.ifr_name, len);
	return fd;
}

int host_net_cfg(struct host_net_adp *hnet, enum host_net_op op) {
	int res, fd;
	char name[16];
	const char *tap_name;

	switch (op) {
	case HOST_NET_INIT:
		tap_name = host_getenv(TAP_DEV_ENV_NAME);
		if (!tap_name) {
			tap_name = TAP_DEFAULT_DEV_NAME;
		}

		if (0 <= (fd = tun_alloc(tap_name, name, 16))) {
			hnet->fd = fd;
			return 0;
		}
		return -errno;
		break;
	case HOST_NET_START:
		assert(0 <= hnet->fd);
		fd = hnet->fd;

		res = host_fcntl(fd, F_SETOWN, host_getpid());
		assert(res != -1);
		res = host_fcntl(fd, F_SETFL, FASYNC | O_NONBLOCK |
				host_fcntl(fd, F_GETFL, 0));
		assert(res != -1);
		break;
	case HOST_NET_STOP:
		assert(0 <= hnet->fd);
		res = host_fcntl(fd, F_SETFL, ~FASYNC &
				host_fcntl(fd, F_GETFL, 0));
		assert(res != -1);
		break;
	}
	return 0;
}

#define SCRATCH_LEN 1514
static char host_net_scratch_rx[SCRATCH_LEN];
int host_net_rx_count(struct host_net_adp *hnet) {
	int res;

	res = host_read(hnet->fd, host_net_scratch_rx, SCRATCH_LEN);

	return res > 0 ? res : 0;
}

int host_net_rx(struct host_net_adp *hnet, void *buf, int len) {
	memcpy(buf, host_net_scratch_rx, len);

	return len;
}

void host_net_tx(struct host_net_adp *hnet, const void *buf, int len) {
	host_write(hnet->fd, buf, len);
}

