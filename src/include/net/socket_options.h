/**
 * @file
 * @brief Socket options definitions.
 *
 * @date 16.04.12
 * @author Timur Abdukadyrov
 */

#ifndef NET_SOCKET_OPTIONS_H__
#define NET_SOCKET_OPTIONS_H__

#include <sys/time.h>

#define DEFAULT_DEBUG 0
#define DEFAULT_RCVBUF 9000
#define DEFAULT_SNDBUF 9000
#define DEFAULT_RCVLOWAT 1
/* normally this default is for TCP, but for the first time
   it doesn't matter for UDP */
#define DEFAULT_SNDLOWAT 2048
#define DEFAULT_TIMEO {.tv_sec = 0, .tv_usec = 0}
#define DEFAULT_TCP_SERVER_REUSEADDR 1

/**
 * posix linger structure
 */
struct linger{
	int l_onoff;  /* 0 = off; nonzero = on  */
	int l_linger; /* linger time. Posix.1g. in seconds */
};

/**
 * socket options state structure
 */
typedef struct socket_opt_state{
	int so_acceptconn;
	int so_broadcast;
	int so_debug;
	int so_dontroute;
	int so_error;
	int so_keepalive;
	struct linger so_linger;
	int so_oobinline;
	int so_rcvbuf;
	int so_rcvlowat;
	struct timeval so_rcvtimeo;
	int so_reuseaddr;
	int so_sndbuf;
	int so_sndlowat;
	struct timeval so_sndtimeo;
	int so_type;
} socket_opt_state_t;


/**
 * @param SO_ACCEPTCONN int Non-zero indicates that socket listening is enabled ( getsockopt() only).
 * @param SO_BROADCAST int Non-zero requests permission to transmit broadcast datagrams (SOCK_DGRAM sockets only).
 * @param SO_DEBUG int Non-zero requests debugging in underlying protocol modules.
 * @param SO_DONTROUTE int Non-zero requests bypass of normal routing; route based on destination address only.
 * @param SO_ERROR int Requests and clears pending error information on the socket ( getsockopt() only).
 * @param SO_KEEPALIVE int Non-zero requests periodic transmission of keepalive messages (protocol-specific).
 * @param SO_LINGER struct linger Specify actions to be taken for queued, unsent data on close(): linger on/off and linger time in seconds.
 * @param SO_OOBINLINE int Non-zero requests that out-of-band data be placed into normal data input queue as received.
 * @param SO_RCVBUF int Size of receive buffer (in bytes).
 * @param SO_RCVLOWAT int Minimum amount of data to return to application for input operations (in bytes).
 * @param SO_RCVTIMEO struct timeval Timeout value for a socket receive operation.
 * @param SO_REUSEADDR int Non-zero requests reuse of local addresses in bind() (protocol-specific).
 * @param SO_SNDBUF int Size of send buffer (in bytes).
 * @param SO_SNDLOWAT int Minimum amount of data to send for output operations (in bytes).
 * @param SO_SNDTIMEO struct timeval Timeout value for a socket send operation.
 * @param SO_TYPE int Identify socket type ( getsockopt() only).
 */
enum socket_options{
	SO_ACCEPTCONN,/* int *//* Non-zero indicates that socket listening is enabled ( getsockopt() only). */
	SO_BROADCAST,/* int *//* Non-zero requests permission to transmit broadcast datagrams (SOCK_DGRAM sockets only). */
	SO_DEBUG,/* int *//* Non-zero requests debugging in underlying protocol modules. */
	SO_DONTROUTE,/* int *//* Non-zero requests bypass of normal routing; route based on destination address only. */
	SO_ERROR,/* int *//* Requests and clears pending error information on the socket ( getsockopt() only). */
	SO_KEEPALIVE,/* int *//* Non-zero requests periodic transmission of keepalive messages (protocol-specific). */
	SO_LINGER,/* struct linger *//* Specify actions to be taken for queued, unsent data on close(): linger on/off and linger time in seconds. */
	SO_OOBINLINE,/* int *//* Non-zero requests that out-of-band data be placed into normal data input queue as received. */
	SO_RCVBUF,/* int *//* Size of receive buffer (in bytes). */
	SO_RCVLOWAT,/* int *//* Minimum amount of data to return to application for input operations (in bytes). */
	SO_RCVTIMEO,/* struct timeval *//* Timeout value for a socket receive operation. */
	SO_REUSEADDR,/* int *//* Non-zero requests reuse of local addresses in bind() (protocol-specific). */
	SO_SNDBUF,/* int *//* Size of send buffer (in bytes). */
	SO_SNDLOWAT, /* int *//* Minimum amount of data to send for output operations (in bytes). */
	SO_SNDTIMEO, /* struct timeval *//* Timeout value for a socket send operation. */
	SO_TYPE, /* int *//* Identify socket type ( getsockopt() only). */
};


/* methods */

/**
 * socket level option setting. called for SOL_SOCKET from setsockopt
 **/
int so_set_socket_option(struct socket_opt_state *opts, unsigned int option,
												 const void *option_value, socklen_t option_len);

/**
 * socket level option getting. called for SOL_SOCKET from getsockopt
 **/
int so_get_socket_option(struct socket_opt_state *opts, unsigned int option,
												 const void *option_value, socklen_t *option_len);

#endif
