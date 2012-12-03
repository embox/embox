/**
 * @file
 *
 * @brief POSIX socket API
 *
 * @date 01.02.2012
 * @author Anton Bondarev
 */

#ifndef SYS_SOCKET_H_
#define SYS_SOCKET_H_

//TODO socket header in POSIX style must name <sys/socket.h>
#include <net/socket.h>
#include <net/net.h>
#include <arpa/inet.h>
#include <net/ip.h> /* IPPROTO_IP and so on */

static inline int getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
	return -1;
}

static inline int getpeername(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
	return -1;
}

#endif /* SYS_SOCKET_H_ */
