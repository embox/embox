/**
 * @file
 * @brief Socket options handling.
 *
 * @date 16.04.12
 * @author Timur Abdukadyrov
 */


#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <net/socket_options.h>

/**
 * default stream type socket options set
 */
static const struct socket_opt_state DEFAULT_STREAM_OPTS = {
	.so_acceptconn = 0,  /* posix doesn't specify default value, we'll turn it off */
	.so_broadcast = 0,   /* posix default */
	.so_debug = 0,       /* no debug. posix default */
	.so_dontroute = 0,	 /* no dontroute flag */
	.so_error = 0,       /* no error happened yet =) */
	.so_keepalive = 0,   /* no keepalive's. posix default */
	.so_linger = {.l_onoff = 0, .l_linger = 0}, /* linger is off. posix default */
	.so_oobinline = 0,   /* out of band data placement off. posix default */
	.so_rcvbuf = DEFAULT_RCVBUF,
	.so_rcvlowat = DEFAULT_RCVLOWAT,
	.so_rcvtimeo = DEFAULT_TIMEO,  /* no timeout. posix default*/
	.so_reuseaddr = 0,  /* TCP servers should set.  posix default */
	.so_sndbuf = DEFAULT_SNDBUF,
	.so_sndlowat = DEFAULT_SNDLOWAT,
	.so_rcvtimeo = DEFAULT_TIMEO,  /* no timeout.  posix default */
	.so_type = SOCK_STREAM,  /* stream socket =) */
	.so_bindtodev = NULL
};

/**
 * default dgram type socket options set
 */
static const struct socket_opt_state DEFAULT_DGRAM_OPTS = {
	.so_acceptconn = 0,
	.so_broadcast = 0,   /* posix specifies default value as 0*/
	.so_debug = 0,       /* no debug. posix default */
	.so_dontroute = 0,	 /* no dontroute flag */
	.so_error = 0,       /* no error happened yet =) */
	.so_keepalive = 0,   /* no keepalive's. posix default */
	.so_linger = {.l_onoff = 0, .l_linger = 0}, /* linger is off. posix default */
	.so_oobinline = 0,   /* out of band data placement off. posix default */
	.so_rcvbuf = DEFAULT_RCVBUF,
	.so_rcvlowat = DEFAULT_RCVLOWAT,
	.so_rcvtimeo = DEFAULT_TIMEO,  /* no timeout.  posix default */
	.so_reuseaddr = 0,  /* TCP servers should set.  posix default */
	.so_sndbuf = DEFAULT_SNDBUF,
	.so_sndlowat = DEFAULT_SNDLOWAT,
	.so_rcvtimeo = DEFAULT_TIMEO,  /* no timeout.  posix default */
	.so_type = SOCK_DGRAM,  /* stream socket =) */
	.so_bindtodev = NULL
};

/**
 * Parse and set socket option
 */
int so_set_socket_option(struct socket_opt_state *opts, unsigned int option,
												 const void *option_value, socklen_t option_len){

	/* the best would be to check the accessibility of addresses pointed to by
	   option_value and option_len*/
	if(!option_value || !option_len){
		return -EFAULT;
	}

	switch(option){
	case SO_ACCEPTCONN:
	case SO_ERROR:
	case SO_TYPE:
		/* cannot set these options, only read */
		return -EINVAL;
		break;
		/* integer valued options */
	case SO_BROADCAST:
		/* in a specific protocol realization when the address is checked
			 to be broadcast it is also a good idea to check wheather it can
		   broadcast at all (like socket type is SOCK_DGRAM)
		   This check isn't done here, because posix dosn't lay any
		   restrictions on setting this option for sockets with types
		   other than SOCK_DGRAM */
		if(option_len != sizeof(unsigned int))
			return -EINVAL;
		opts->so_broadcast = *((unsigned int*)option_value);
		break;
	case SO_DEBUG:
		if(option_len != sizeof(unsigned int))
			return -EINVAL;
		opts->so_debug = *((unsigned int*)option_value);
		break;
	case SO_DONTROUTE:
		if(option_len != sizeof(unsigned int))
			return -EINVAL;
		opts->so_dontroute = *((unsigned int*)option_value);
		break;
	case SO_KEEPALIVE:
		if(option_len != sizeof(unsigned int))
			return -EINVAL;
		opts->so_keepalive = *((unsigned int*)option_value);
		break;
	case SO_OOBINLINE:
		if(option_len != sizeof(unsigned int))
			return -EINVAL;
		opts->so_oobinline = *((unsigned int*)option_value);
		break;
	case SO_RCVBUF:
		if(option_len != sizeof(unsigned int))
			return -EINVAL;
		opts->so_rcvbuf = *((unsigned int*)option_value);
		break;
	case SO_RCVLOWAT:
		if(option_len != sizeof(unsigned int))
			return -EINVAL;
		opts->so_rcvlowat = *((unsigned int*)option_value);
		break;
	case SO_REUSEADDR:
		if(option_len != sizeof(unsigned int))
			return -EINVAL;
		opts->so_reuseaddr = *((unsigned int*)option_value);
		break;
	case SO_SNDBUF:
		if(option_len != sizeof(unsigned int))
			return -EINVAL;
		opts->so_sndbuf = *((unsigned int*)option_value);
		break;
	case SO_SNDLOWAT:
		if(option_len != sizeof(unsigned int))
			return -EINVAL;
		opts->so_sndlowat = *((unsigned int*)option_value);
		break;
		/* non-integer valued options */
	case SO_LINGER:
		if(option_len != sizeof(struct linger))
			return -EINVAL;
		memcpy(&opts->so_linger, option_value, option_len);
		break;
	case SO_RCVTIMEO:
		if(option_len < sizeof(struct timeval))
			return -EINVAL;
		if(option_len > sizeof(struct timeval))
			return -EDOM;
		memcpy(&opts->so_rcvtimeo, option_value, option_len);
		break;
	case SO_SNDTIMEO:
		if(option_len < sizeof(struct timeval))
			return -EINVAL;
		if(option_len > sizeof(struct timeval))
			return -EDOM;
		memcpy(&opts->so_sndtimeo, option_value, option_len);
		break;
	default:
		return -ENOPROTOOPT;
		break;
	};
	return ENOERR;
}

void so_options_init(struct socket_opt_state *opts, int socket_type){
	switch(socket_type){
	case SOCK_STREAM:
		memcpy(opts, &DEFAULT_STREAM_OPTS, sizeof(struct socket_opt_state));
		break;
	case SOCK_DGRAM:
	default:
		memcpy(opts, &DEFAULT_DGRAM_OPTS, sizeof(struct socket_opt_state));
	}
}
/**
 * Get socket option
 */
int so_get_socket_option(struct socket_opt_state *opts, unsigned int option,
												 const void *option_value, socklen_t *option_len){

	/* the best would be to check the accessibility of addresses pointed to by
	   option_value and option_len*/
	if(!option_value || !option_len){
		return -EFAULT;
	}

	switch(option){
	case SO_ACCEPTCONN:
		*((unsigned int*)option_value) = opts->so_acceptconn;
		break;
	case SO_ERROR:
		*((unsigned int*)option_value) = opts->so_error;
		opts->so_error = 0;  /* clear pending error. posix */
		break;
	case SO_TYPE:
		*((unsigned int*)option_value) = opts->so_type;
		break;
	case SO_BROADCAST:
		*((unsigned int*)option_value) = opts->so_broadcast;
		break;
	case SO_DEBUG:
		*((unsigned int*)option_value) = opts->so_debug;
		break;
	case SO_DONTROUTE:
		*((unsigned int*)option_value) = opts->so_dontroute;
		break;
	case SO_KEEPALIVE:
		*((unsigned int*)option_value) = opts->so_keepalive;
		break;
	case SO_OOBINLINE:
		*((unsigned int*)option_value) = opts->so_oobinline;
		break;
	case SO_RCVBUF:
		*((unsigned int*)option_value) = opts->so_rcvbuf;
		break;
	case SO_RCVLOWAT:
		*((unsigned int*)option_value) = opts->so_rcvlowat;
		break;
	case SO_REUSEADDR:
		*((unsigned int*)option_value) = opts->so_reuseaddr;
		break;
	case SO_SNDBUF:
		*((unsigned int*)option_value) = opts->so_sndbuf;
		break;
	case SO_SNDLOWAT:
		*((unsigned int*)option_value) = opts->so_sndlowat;
		break;
		/* non-integer valued options */
	case SO_LINGER:
		if(*option_len != sizeof(struct linger))
			return -EINVAL;
		((struct linger*)option_value)->l_onoff = opts->so_linger.l_onoff;
		((struct linger*)option_value)->l_linger = opts->so_linger.l_linger;
		*option_len = sizeof(struct linger);
		return ENOERR;
	case SO_RCVTIMEO:
		((struct timeval*)option_value)->tv_sec = opts->so_rcvtimeo.tv_sec;
		((struct timeval*)option_value)->tv_usec = opts->so_rcvtimeo.tv_usec;
		goto option_check_timeval;
	case SO_SNDTIMEO:
		((struct timeval*)option_value)->tv_sec = opts->so_sndtimeo.tv_sec;
		((struct timeval*)option_value)->tv_usec = opts->so_sndtimeo.tv_usec;
		goto option_check_timeval;
	case SO_BINDTODEVICE:
		option_value = opts->so_bindtodev;
		break;
	default:
		return -ENOPROTOOPT;
	};

	if(*option_len != sizeof(unsigned int))
		return -EINVAL;
	*option_len = sizeof(unsigned int);

	return ENOERR;

option_check_timeval:
	if (*option_len < sizeof(struct timeval))
		return -EINVAL;
	if (*option_len > sizeof(struct timeval))
		return -EDOM;
	*option_len = sizeof(struct timeval);

	return ENOERR;
}
