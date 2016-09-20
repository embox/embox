/**
 * @file
 * @brief
 *
 * @date 28.04.2016
 * @author Denis Chusovitin
 */

#include "vec_common.h"

ssize_t writev(int fd, const struct iovec *iov, int iovcnt) {
	int ret;
	struct msghdr msg;
	struct sock *sk;

	if (iovcnt < 0) {
		return SET_ERRNO(EINVAL);
	}

	socket_idesc_check(fd, sk);

	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = (struct iovec*)iov;
	msg.msg_iovlen = iovcnt;
	msg.msg_flags = 0;

	ret = ksendmsg(sk, &msg, sk->idesc.idesc_flags);

	if (ret < 0) {
		return SET_ERRNO(-ret);
	}

	return ret;
}
