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
#include <sys/socket.h>


#define DEFAULT_DEBUG 0
#define DEFAULT_RCVBUF 9000
#define DEFAULT_SNDBUF 9000
#define DEFAULT_RCVLOWAT 1
/* normally this default is for TCP, but for the first time
   it doesn't matter for UDP */
#define DEFAULT_SNDLOWAT 2048
#define DEFAULT_TIMEO {.tv_sec = 0, .tv_usec = 0}
#define DEFAULT_TCP_SERVER_REUSEADDR 1

///**
// * posix linger structure
// */
//struct linger {
//	int l_onoff;  /* 0 = off; nonzero = on  */
//	int l_linger; /* linger time. Posix.1g. in seconds */
//};

struct net_device;

/**
 * socket options state structure
 */
typedef struct socket_opt_state {
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
	struct net_device *so_bindtodev; /* name of interface socket bind to*/
} socket_opt_state_t;



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

/**
 * initialize socket options values
 **/
void so_options_init(struct socket_opt_state *opts, int socket_type);

/* simple wrap to ask if option is set(senseless for linger and timeval structs)*/
#define SO_IS_OPTION_SET(sk, option) sk->sk_socket->socket_node->options.option ? true : false

#endif
