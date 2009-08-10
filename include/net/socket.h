/**
 * \file socket.h
 *
 * \date Mar 19, 2009
 * \author anton, sikmir
 */

#ifndef SOCKET_H_
#define SOCKET_H_

#include "net/sock.h"

#define MAX_SOCK_NUM 4

struct sockaddr {
        unsigned short    sa_family;    /* address family, AF_xxx */
	char              sa_data[14];  /* 14 bytes of protocol address */
};

/* Supported address families. */
#define AF_INET      2  /* Internet IP Protocol */

/* Protocol families, same as address families. */
#define PF_INET      AF_INET

/**
 * create an endpoint for communication.
 * @param protocol The protocol specifies a particular protocol to be used with the socket.
 * 	Normally only a single protocol exists to support a particular socket type within a
 * 	given protocol family, in which case protocol can be specified as 0.
 * @return On success, a file descriptor for the new socket is returned.
 * 	On error, -1 is returned.
 */
int socket(int domain, int type, int protocol);

/**
 * bind a name to a socket.
 * @return 0 on success.  On error, -1.
 */
int bind(int sockfd, const struct sockaddr *addr, int addrlen);

/**
 * send a message on a socket.
 * @return the number of characters sent. On error, -1.
 */
int send(int sockfd, const void *buf, int len, int flags);

/**
 * receive a message from a socket.
 * @return the number of bytes received, or -1 if an error occurred.
 */
int recv(int sockfd, void *buf, int len, int flags);

/**
 * close a socket descriptor
 * @return 0 on success. On error, -1.
 */
int close(int sockfd);

/**
 * Push packet received from udp_received_packet into socket.
 */
int udpsock_push(net_packet *pack);

#endif /* SOCKET_H_ */
