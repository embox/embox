/**
 * @file
 * @brief Socket interface function for kernel mode.
 *
 * @date 08.02.10
 * @author Nikolay Korotky
 * @author Ilia Vaprol
 */

#ifndef NET_KERNEL_SOCKET_H_
#define NET_KERNEL_SOCKET_H_

#include <linux/aio.h>
#include <net/socket.h>
#include <net/net.h>
#include <types.h>

/**
 * Create kernel socket.
 *
 * @param family a protocol family which will be used for communication.
 * @param a type communication semantics.
 * @param protocol a particular protocol to be used with the socket.
 * @param res pointer to the socket structure
 * @return On success, 0 otherwise result < 0.
 */
extern int kernel_socket_create(int family, int type, int protocol,
					struct socket **res);

/**
 * Close a kernel socket.
 * The socket is released from the protocol stack
 * if it has a release callback.
 *
 * @param sock socket to close
 * @return 0 on success, else error code.
 */
extern int kernel_socket_release(struct socket *sock);

/**
 * Bind a name to a socket.
 * Assigns the address specified to by addr to the socket.
 * Call bind callback from proto_ops.
 *
 * @param sock pointer to the socket structure
 * @param addr address
 * @param addrlen size, in bytes, of the address structure pointed to by addr.
 * @return error code
 */
extern int kernel_socket_bind(struct socket *sock, const struct sockaddr *addr,
					socklen_t addrlen);

/**
 * Listen for connections on a socket.
 * Call listen callback from proto_ops.
 *
 * @param sock pointer to the socket structure
 * @param backlog maximum length to which the queue of pending connections for sock may grow
 * @return error code
 */
extern int kernel_socket_listen(struct socket *sock, int backlog);

/**
 * Accept a connection on a socket
 * Call accept callback from proto_ops.
 *
 * @param sock pointer to the socket structure
 * @param sockaddr
 * @param *addrlen
 * @return error code
 */
extern int kernel_socket_accept(struct socket *sock, struct sockaddr *addr,
	       	socklen_t *addrlen);

/**
 * Initiate a connection on a socket.
 * Call connect callback from proto_ops.
 *
 * @param sock pointer to the socket structure
 * @param addr address to connect
 * @param addrlen the size of addr
 * @param flags
 * @return error code
 */
extern int kernel_socket_connect(struct socket *sock, const struct sockaddr *addr,
					int addrlen, int flags);

/**
 * Get socket name.
 * Note: not realized.
 */
extern int kernel_socket_getsockname(struct socket *sock, struct sockaddr *addr,
					int *addrlen);

/**
 * Get name of connected peer socket.
 * Note: not realized.
 */
extern int kernel_socket_getpeername(struct socket *sock, struct sockaddr *addr,
					int *addrlen);

/**
 * Get options on socket.
 * Note: not realized.
 */
extern int kernel_socket_getsockopt(struct socket *sock, int level, int optname,
					char *optval, int *optlen);

/**
 * Set options on socket.
 * Note: not realized.
 */
extern int kernel_socket_setsockopt(struct socket *sock, int level, int optname,
					char *optval, int optlen);

/**
 * Send a message on a socket.
 * Call sendmsg callback from proto_ops.
 *
 * @param iocb not used
 * @param sock pointer to the socket structure
 * @param msg
 * @param size
 * @return error code
 */
extern int kernel_socket_sendmsg(struct kiocb *iocb, struct socket *sock,
				struct msghdr *msg, size_t size);

/**
 * Receive a message from a socket.
 * Call recvmsg callback from proto_ops.
 *
 * @param iocb not used
 * @param sock pointer to the socket structure
 * @param msg
 * @param size
 * @param flags
 * @return error code
 */
extern int kernel_socket_recvmsg(struct kiocb *iocb, struct socket *sock,
				struct msghdr *msg, size_t size, int flags);

extern int kernel_socket_shutdown(struct socket *sock);
extern int kernel_socket_close(struct socket *sock);

#if 0
extern int kernel_sendpage(struct socket *sock, struct page *page, int offset,
			size_t size, int flags);
extern int kernel_sock_ioctl(struct socket *sock, int cmd, unsigned long arg);
#endif

#endif /* NET_KERNEL_SOCKET_H_ */
