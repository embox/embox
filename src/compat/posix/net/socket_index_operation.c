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
#include <sys/uio.h>
#include <sys/socket.h>
#include <poll.h>

#include <net/socket/ksocket.h>

#include <linux/net_tstamp.h>

const struct idesc_ops task_idx_ops_socket;

static ssize_t socket_read(struct idesc *desc, const struct iovec *iov, int cnt) {
	int ret;
	ssize_t ret_size;
	int i;
	struct msghdr msg;
	struct sock *sk = (struct sock *)desc;

	assert(desc);
	assert(desc->idesc_ops == &task_idx_ops_socket);

	if (sk->shutdown_flag & (SHUT_RD + 1)) {
		return -EPIPE;
	}

	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = (struct iovec *)iov;
	msg.msg_iovlen = cnt;
	msg.msg_flags = 0;

	ret = krecvmsg(sk, &msg, desc->idesc_flags);
	if (ret != 0) {
		return ret;
	}

	ret_size = 0;
	for (i = 0; i < cnt; i++) {
		ret_size += iov[i].iov_len;
	}

	return ret_size;
}

static ssize_t socket_write(struct idesc *desc, const struct iovec *iov, int cnt) {
	int ret;
	ssize_t ret_size;
	int i;
	struct msghdr msg;
	struct sock *sk = (struct sock *)desc;

	assert(desc);
	assert(desc->idesc_ops == &task_idx_ops_socket);

	if (sk->shutdown_flag & (SHUT_WR + 1))
		return -EPIPE;

	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = (struct iovec *)iov;
	msg.msg_iovlen = cnt;
	msg.msg_flags = 0;

	ret = ksendmsg(sk, &msg, desc->idesc_flags);
	if (ret != 0) {
		return ret;
	}

	ret_size = 0;
	for (i = 0; i < cnt; i++) {
		ret_size += iov[i].iov_len;
	}

	return ret_size;
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
	.id_readv   = socket_read,
	.id_writev  = socket_write,
	.ioctl  = socket_ioctl,
	.status = socket_status,
	.close  = socket_close,
};

