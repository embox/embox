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

#include <kernel/task.h>
#include <kernel/task/idx.h>

#include <net/socket/ksocket.h>

#include <sys/socket.h>

static inline struct sock * desc2sock(struct idx_desc *desc) {
	return desc != NULL ? task_idx_desc_data(desc) : NULL;
}

static inline int desc2flags(struct idx_desc *desc) {
	return desc != NULL ? *task_idx_desc_flags_ptr(desc) : 0;
}

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

	iov.iov_base = buff;
	iov.iov_len = size;

	ret = krecvmsg(desc2sock(desc), &msg, desc2flags(desc));
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

	ret = ksendmsg(desc2sock(desc), &msg, desc2flags(desc));
	if (ret != 0) {
		SET_ERRNO(-ret);
		return -1;
	}

	return iov.iov_len;
}

static int socket_close(struct idx_desc *desc) {
	int ret;

	assert(desc != NULL);

	ret = ksocket_close(desc2sock(desc));
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
