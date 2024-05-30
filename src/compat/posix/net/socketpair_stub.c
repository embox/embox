/**
 * @file
 *
 * @date 30.05.24
 * @author Anton Bondarev
 */

#include <sys/socket.h>

#include <errno.h>

int socketpair(int domain, int type, int protocol, int sv[2]) {
	(void)domain;
	(void)type;
	(void)protocol;
	(void)sv;
	errno = -EPROTONOSUPPORT;
	return -1;
}