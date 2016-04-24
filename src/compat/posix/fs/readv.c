#include <unistd.h>
#include <errno.h>

#include <fs/index_descriptor.h>
#include <fs/idesc.h>
#include <kernel/task/resource/idesc_table.h>

#include <netinet/in.h>
#include <sys/socket.h>

#include <util/macro.h>
#include <kernel/task/resource/idesc_table.h>
#include <net/sock.h>
#include <net/socket/socket_desc.h>
#include <net/socket/ksocket.h>

#define  socket_idesc_check(sockfd, sk) \
	if (!idesc_index_valid(sockfd) || !index_descriptor_get(sockfd)) {\
		return SET_ERRNO(EBADF);            \
	}                                \
	if (NULL == (sk = idesc_sock_get(sockfd))) { \
		return SET_ERRNO(ENOTSOCK);            \
	} \

ssize_t readv(int fd, const struct iovec *iov, int iovcnt) {
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

	ret = krecvmsg(sk, &msg, sk->idesc.idesc_flags);

	if (ret < 0) {
		return SET_ERRNO(-ret);
	}

	return msg.msg_iovlen;
}
