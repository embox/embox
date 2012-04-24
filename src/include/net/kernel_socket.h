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
 * Create socket method in kernel layer.
 * Allocate memory pick the protocol-type methods structure if present,
 * call native initialization routine and add a node to socket registry
 * to keep track of an entity. Sets socket connection state to UNCONNECTED
 *
 * @param family a protocol family which will be used for communication.
 * @param type socket type(i.e. SOCK_STREAM, SOCK_DGRAM, SOCK_RAW).
 * @param protocol a particular protocol to be used with the socket.
 * @param res pointer to the socket structure
 * @return On success, 0 otherwise result equal to minus posix errno for the
 * error reason.
 */
extern int kernel_socket_create(int family, int type, int protocol,
					struct socket **res);

/**
 * Close socket method in kernel layer.
 * Calls socket's native release method if present and
 * frees up it's memory after. Removes socket entity from registry
 *
 * @param sock socket to close
 * @return 0 on success, otherwise result equal to minus posix errno for the
 * error reason.
 */
extern int kernel_socket_release(struct socket *sock);

/**
 * Bind socket to a local address.
 * Performs various verifications of input parameters and calls protocol
 * realization of bind, which should perform the specific for the protocol
 * binding procedure. Upon success socket connection state is set to BOUND
 * else it is left to be UNCONNECTED.
 *
 * @param sock pointer to the socket structure
 * @param addr address
 * @param addrlen size, in bytes, of the address structure pointed to by addr.
 * @return 0 on success, minus posix errno indicating the reason
 */
extern int kernel_socket_bind(struct socket *sock, const struct sockaddr *addr,
					socklen_t addrlen);

/**
 * Listen for connections on a connection-mode socket.
 * Sets the state of a bound socket to listening, if is made sure to be able to
 * do so for a particular socket, by calling native protocol listen method.
 *
 * @param sock pointer to the socket structure
 * @param backlog maximum length to which the queue of pending connections for sock may grow
 * @return 0 on success, minus posix errno indicating the reason of an error
 * on a failure
 */
extern int kernel_socket_listen(struct socket *sock, int backlog);

/**
 * Accept a connection on a connection-mode socket
 * Call accept native method of a protocol to which the socket belongs, if
 * is made sure to be able to do so. On acception a new socket with the same type-
 * protocol is created. The new socket handles the particular connection, while
 * the listening sockets stays in it's state listening to incoming connections.
 *
 * @param sock pointer to the socket structure
 * @param pointer to new socket
 * @param sockaddr sockaddr structure representing incoming connection address
 * if available
 * @param *addrlen is set to the length of address structure if it is available
 * @return 0 on success, minus posix errno on failure
 */
extern int kernel_socket_accept(struct socket *sock, struct socket **new_sock,
		struct sockaddr *addr, socklen_t *addrlen);

/**
 * Initiate a connection from a connection-mode socket.
 * Perform verifications needed to be be able to connect and call native
 * protocol method.
 *
 * @param sock pointer to the socket structure
 * @param addr address to connect
 * @param addrlen the size of addr
 * @param flags
 * @return 0 on success, minus posix errno on failure
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
 * Get socket options.
 * Note: not realized.
 */
extern int kernel_socket_getsockopt(struct socket *sock, int level, int optname,
					char *optval, socklen_t *optlen);

/**
 * Set socket options.
 * Note: not realized.
 */
extern int kernel_socket_setsockopt(struct socket *sock, int level, int optname,
					char *optval, socklen_t optlen);

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

extern struct socket * socket_alloc(void);
extern void socket_free(struct socket *sock);

#endif /* NET_KERNEL_SOCKET_H_ */
