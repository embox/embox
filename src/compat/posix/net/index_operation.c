/*
 * @file
 *
 * @date Jun 26, 2013
 * @author: Anton Bondarev
 */
#include <stddef.h>

#include <kernel/task.h>
#include <kernel/task/idx.h>

#include <net/socket/ksocket.h>

#include <sys/socket.h>

extern int recvmsg_sock(struct socket *sock, struct msghdr *msg, int flags);
extern int sendmsg_sock(struct socket *sock, struct msghdr *msg, int flags);

static ssize_t socket_read(struct idx_desc *desc, void *buff,
		size_t size) {
	int ret;
	struct msghdr msg;
	struct iovec iov;

	assert(desc != NULL);

	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_flags = 0;

	iov.iov_base = (void *)buff;
	iov.iov_len = size;

	ret = recvmsg_sock((struct socket *)task_idx_desc_data(desc),
			&msg, *task_idx_desc_flags_ptr(desc));
	if (ret != 0) {
		SET_ERRNO(-ret);
		return -1;
	}

	return iov.iov_len;
}

static ssize_t socket_write(struct idx_desc *desc, const void *buff,
		size_t size) {
	int ret;
	struct msghdr msg;
	struct iovec iov;

	assert(desc != NULL);

	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_flags = 0;

	iov.iov_base = (void *)buff;
	iov.iov_len = size;

	ret = sendmsg_sock((struct socket *)task_idx_desc_data(desc),
			&msg, *task_idx_desc_flags_ptr(desc));
	if (ret != 0) {
		SET_ERRNO(-ret);
		return -1;
	}

	return iov.iov_len;
}

static int socket_close(struct idx_desc *desc) {
	int ret;

	assert(desc != NULL);

	ret = ksocket_close(task_idx_desc_data(desc));
	if (ret != 0) {
		SET_ERRNO(-ret);
		return -1;
	}

	return 0;
}

const struct task_idx_ops task_idx_ops_socket = {
	.read = socket_read,
	.write = socket_write,
	.close = socket_close
};
