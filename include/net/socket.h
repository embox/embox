/**
 * @file socket.h
 *
 * @date 19.03.2009
 * @author Anton Bondarev
 * @author Nikolay Korotky
 */
#ifndef SOCKET_H_
#define SOCKET_H_

struct sk_buff;

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

#endif /* SOCKET_H_ */
