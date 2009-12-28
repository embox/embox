/**
 * @file socket.h
 *
 * @date 19.03.2009
 * @author Anton Bondarev
 * @author Nikolay Korotky
 */
#ifndef SOCKET_H_
#define SOCKET_H_

#include <net/skbuff.h>

typedef unsigned short  sa_family_t;

typedef struct sockaddr {
	sa_family_t    sa_family;    /* address family, AF_xxx */
	char           sa_data[14];  /* 14 bytes of protocol address */
} sockaddr_t;

/* Supported address families. */
#define AF_UNSPEC    0
#define AF_UNIX      1  /* Unix domain sockets    */
#define AF_LOCAL     1  /* POSIX name for AF_UNIX */
#define AF_INET      2  /* Internet IP Protocol   */
#define AF_MAX       3  /* For now.. */

/* Protocol families, same as address families. */
#define PF_UNSPEC    AF_UNSPEC
#define PF_UNIX      AF_UNIX
#define PF_LOCAL     AF_LOCAL
#define PF_INET      AF_INET
#define PF_MAX       AF_MAX

//TODO: move out of here
int sock_init(void);

#if 0
/**
 * create an endpoint for communication.
 * @param protocol The protocol specifies a particular protocol to be used with the socket.
 * 	Normally only a single protocol exists to support a particular socket type within a
 * 	given protocol family, in which case protocol can be specified as 0.
 * @return On success, a file descriptor for the new socket is returned.
 * 	On error, -1 is returned.
 */
extern int socket(int domain, int type, int protocol);

/**
 * bind a name to a socket.
 * @return 0 on success.  On error, -1.
 */
extern int bind(int sockfd, const struct sockaddr *addr, int addrlen);

/**
 * send a message on a socket.
 * @return the number of characters sent. On error, -1.
 */
extern int send(int sockfd, const void *buf, int len, int flags);

/**
 * receive a message from a socket.
 * @return the number of bytes received, or -1 if an error occurred.
 */
extern int recv(int sockfd, void *buf, int len, int flags);

/**
 * check message in a socket
 * @return 1 if there isn't a message in socket, 0 otherwise or -1 if an error occurred.
 */
extern int empty_socket(int sockfd);

/**
 * close a socket descriptor
 * @return 0 on success. On error, -1.
 */
extern int close(int sockfd);

/**
 * Push packet received from udp_rcv into socket.
 */
extern int udpsock_push(struct sk_buff *pack);
#endif

#endif /* SOCKET_H_ */
