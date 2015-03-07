/*
 * @file
 * @brief
 *
 * @date 26.06.13
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <stddef.h>
#include <errno.h>

#include <net/socket/ksocket.h>

#include <linux/net_tstamp.h>
#include <sys/socket.h>
#include <poll.h>

const struct idesc_ops task_idx_ops_socket;

static ssize_t socket_read(struct idesc *desc, void *buff,
		size_t size) {
	int ret;
	struct msghdr msg;
	struct iovec iov;
	struct sock *sk = (struct sock *)desc;

	assert(desc);
	assert(desc->idesc_ops == &task_idx_ops_socket);

	if (sk->shutdown_flag & (SHUT_RD + 1)) {
		return -EPIPE;
	}

	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_flags = 0;

	iov.iov_base = buff;
	iov.iov_len = size;

	ret = krecvmsg(sk, &msg, desc->idesc_flags);
	if (ret != 0) {
		return ret;
	}

	return iov.iov_len;
}

static ssize_t socket_write(struct idesc *desc, const void *buff,
		size_t size) {
	int ret;
	struct msghdr msg;
	struct iovec iov;
	struct sock *sk = (struct sock *)desc;

	assert(desc);
	assert(desc->idesc_ops == &task_idx_ops_socket);

	if (sk->shutdown_flag & (SHUT_WR + 1))
		return -EPIPE;

	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_flags = 0;

	iov.iov_base = (void *)buff;
	iov.iov_len = size;

	ret = ksendmsg(sk, &msg, desc->idesc_flags);
	if (ret != 0) {
		return ret;
	}

	return iov.iov_len;
}

static int socket_ioctl(struct idesc *idesc, int request, void *data) {
	struct sock *sk = (struct sock *) idesc;

	switch (request) {
	case SIOCGSTAMP:
		memcpy(data, &sk->last_packet_tstamp, sizeof(struct timeval));
		return 0;
	default:
		break;
	}
	return -ENOTSUP;
}

static int socket_status(struct idesc *desc, int status_nr) {
	struct sock *sk = (struct sock *)desc;
	int res;

	assert(sk);
	assert(desc->idesc_ops == &task_idx_ops_socket);

	res = 0;

	if (status_nr & POLLIN) {
		res += sk->rx_data_len + sk->opt.so_error;
	}
	if (status_nr & POLLOUT) {
		res += 0x600 + sk->opt.so_error;
	}
	if (status_nr & POLLERR) {
		res += sk->opt.so_error;
	}

	return res;
}

static void socket_close(struct idesc *desc) {
	struct sock *sk = (struct sock *)desc;

	assert(desc);
	assert(desc->idesc_ops == &task_idx_ops_socket);

	ksocket_close(sk);
}

const struct idesc_ops task_idx_ops_socket = {
	.read   = socket_read,
	.write  = socket_write,
	.ioctl  = socket_ioctl,
	.status = socket_status,
	.close  = socket_close,
};

