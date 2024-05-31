/**
 * @file
 *
 * @date 30.05.24
 * @author Anton Bondarev
 */

#include <errno.h>

#include <unistd.h>

#include <sys/socket.h>

int socketpair(int domain, int type, int protocol, int sv[2]) {
	if (domain != AF_UNIX) {
		errno = EAFNOSUPPORT;
		return -1;
	}

	if (protocol != 0) {
		errno = EPROTONOSUPPORT;
		return -1;
	}

	if (type != SOCK_STREAM) {
		errno = EOPNOTSUPP;
		return -1;
	}

	return pipe(sv);
}