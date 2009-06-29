/**
 * \file socket.h
 *
 * \date Mar 19, 2009
 * \author anton, sikmir
 */

#ifndef SOCKET_H_
#define SOCKET_H_

#include "sock.h"

#define MAX_SOCK_NUM 4

struct sockaddr {
	unsigned char ipaddr[4];
	int port;
};

/**
 * create an endpoint for communication.
 */
int socket(int domain, sk_type type, sk_proto protocol);

/**
 * bind a name to a socket.
 */
int bind(int sockfd, const struct sockaddr *addr, int addrlen);

/**
 * send a message on a socket.
 */
int send(int sockfd, const void *buf, int len, int flags);

/**
 * receive a message from a socket.
 */
int recv(int sockfd, void *buf, int len, int flags);

/**
 * close a socket descriptor
 */
void close(int sockfd);

int udpsock_push(net_packet *pack);

#endif /* SOCKET_H_ */
