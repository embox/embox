/**
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 11.06.26
 */

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/can.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <drivers/can_dev.h>
#include <framework/mod/options.h>
#include <mem/misc/pool.h>
#include <net/family.h>
#include <net/net_sock.h>
#include <net/sock.h>

#define CAN_DEVICE         OPTION_STRING_GET(device)
#define CAN_SOCK_POOL_SIZE OPTION_GET(NUMBER, sock_pool_size)

struct can_sock {
	struct sock sk;
	int can_dev_fd;
};

static int socketcan_init(struct sock *sk) {
	struct can_sock *skcan;
	int fd;

	skcan = (struct can_sock *)sk;

	fd = char_dev_open(CAN_DEVICE, O_RDWR);
	if (fd < 0) {
		skcan->can_dev_fd = -1;
		return fd;
	}

	skcan->can_dev_fd = fd;

	return 0;
}

static int socketcan_close(struct sock *sk) {
	struct can_sock *skcan;

	skcan = (struct can_sock *)sk;

	close(skcan->can_dev_fd);

	skcan->can_dev_fd = -1;

	sock_release(sk);

	return 0;
}

static int socketcan_bind(struct sock *sk, const struct sockaddr *addr,
    socklen_t addrlen) {
	return 0;
}

static int socketcan_sendmsg(struct sock *sk, struct msghdr *msg, int flags) {
	struct can_sock *skcan;
	int err;

	skcan = (struct can_sock *)sk;

	err = writev(skcan->can_dev_fd, msg->msg_iov, msg->msg_iovlen);
	if (err < 0) {
		return err;
	}

	return 0;
}

static int socketcan_recvmsg(struct sock *sk, struct msghdr *msg, int flags) {
	struct can_sock *skcan;
	int err;

	skcan = (struct can_sock *)sk;

	if (msg->msg_flags & MSG_DONTWAIT) {
		/* TODO */
	}

	err = readv(skcan->can_dev_fd, msg->msg_iov, msg->msg_iovlen);
	if (err < 0) {
		return err;
	}

	return 0;
}

POOL_DEF(can_sock_pool, struct can_sock, CAN_SOCK_POOL_SIZE);

static const struct sock_family_ops socketcan_family_ops = {
    .init = socketcan_init,
    .close = socketcan_close,
    .bind = socketcan_bind,
    .sendmsg = socketcan_sendmsg,
    .recvmsg = socketcan_recvmsg,
    .sock_pool = &can_sock_pool,
};

static const struct net_family_type socketcan_family_types[] = {
    {SOCK_DGRAM, &socketcan_family_ops},
    {SOCK_RAW, &socketcan_family_ops},
};

const struct net_pack_out_ops *const socketcan_net_pack_ops;

EMBOX_NET_FAMILY(AF_CAN, socketcan_family_types, socketcan_net_pack_ops);
