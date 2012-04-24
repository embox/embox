/**
 * @file
 * @brief Socket options handling.
 *
 * @date 16.04.12
 * @author Timur Abdukadyrov
 */


#include <errno.h>
#include <string.h>
#include <net/socket.h>
#include <net/net.h>

/**
 * default stream type socket options set
 */
static const struct socket_opt_state DEFAULT_STREAM_OPTS = {
	.so_acceptconn = 1,  /* by default stream socket should be able to accept */
	.so_broadcast = 0,   /* and it shouldn't be able to broadcast. dgram should */
	.so_debug = 0,       /* no debug */
	.so_dontroute = 0,	 /* no dontroute flag */
	.so_error = 0,       /* no error happened yet =) */
	.so_keepalive = 0,   /* no keepalive's */
	.so_linger = {.l_onoff = 0, .l_linger = 0}, /* linger is off */
	.so_oobinline = 0,   /*  */
	.so_rcvbuf = DEFAULT_RCVBUF,
	.so_rcvlowat = DEFAULT_RCVLOWAT,
	.so_rcvtimeo = DEFAULT_TIMEO,  /* default timeo zeros */
	.so_reuseaddr = 0,  /* TCP servers should set */
	.so_sndbuf = DEFAULT_SNDBUF,
	.so_sndlowat = DEFAULT_SNDLOWAT,
	.so_rcvtimeo = DEFAULT_TIMEO,  /* default timeo zeros */
	.so_type = SOCK_STREAM,  /* stream socket =) */
};

/**
 * default dgram type socket options set
 */
static const struct socket_opt_state DEFAULT_DGRAM_OPTS = {
	.so_acceptconn = 0,  /* by default dgram socket shouldn't be able to accept */
	.so_broadcast = 1,   /* and it should be able to broadcast.*/
	.so_debug = 0,       /* no debug */
	.so_dontroute = 0,	 /* no dontroute flag */
	.so_error = 0,       /* no error happened yet =) */
	.so_keepalive = 0,   /* no keepalive's */
	.so_linger = {.l_onoff = 0, .l_linger = 0}, /* linger is off */
	.so_oobinline = 0,   /*  */
	.so_rcvbuf = DEFAULT_RCVBUF,
	.so_rcvlowat = DEFAULT_RCVLOWAT,
	.so_rcvtimeo = DEFAULT_TIMEO,  /* default timeo zeros */
	.so_reuseaddr = 0,  /* TCP servers should set */
	.so_sndbuf = DEFAULT_SNDBUF,
	.so_sndlowat = DEFAULT_SNDLOWAT,
	.so_rcvtimeo = DEFAULT_TIMEO,  /* default timeo zeros */
	.so_type = SOCK_DGRAM,  /* stream socket =) */
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
		if(*option_len != sizeof(unsigned int))
			return -EINVAL;
		*((unsigned int*)option_value) = opts->so_acceptconn;
		*option_len = sizeof(unsigned int);
		break;
	case SO_ERROR:
		if(*option_len != sizeof(unsigned int))
			return -EINVAL;
		*((unsigned int*)option_value) = opts->so_error;
		*option_len = sizeof(unsigned int);
		break;
	case SO_TYPE:
		if(*option_len != sizeof(unsigned int))
			return -EINVAL;
		*((unsigned int*)option_value) = opts->so_type;
		*option_len = sizeof(unsigned int);
		break;
	case SO_BROADCAST:
		if(*option_len != sizeof(unsigned int))
			return -EINVAL;
		*((unsigned int*)option_value) = opts->so_broadcast;
		*option_len = sizeof(unsigned int);
		break;
	case SO_DEBUG:
		if(*option_len != sizeof(unsigned int))
			return -EINVAL;
		*((unsigned int*)option_value) = opts->so_debug;
		*option_len = sizeof(unsigned int);
		break;
	case SO_DONTROUTE:
		if(*option_len != sizeof(unsigned int))
			return -EINVAL;
		*((unsigned int*)option_value) = opts->so_dontroute;
		*option_len = sizeof(unsigned int);
		break;
	case SO_KEEPALIVE:
		if(*option_len != sizeof(unsigned int))
			return -EINVAL;
		*((unsigned int*)option_value) = opts->so_keepalive;
		*option_len = sizeof(unsigned int);
		break;
	case SO_OOBINLINE:
		if(*option_len != sizeof(unsigned int))
			return -EINVAL;
		*((unsigned int*)option_value) = opts->so_oobinline;
		*option_len = sizeof(unsigned int);
		break;
	case SO_RCVBUF:
		if(*option_len != sizeof(unsigned int))
			return -EINVAL;
		*((unsigned int*)option_value) = opts->so_rcvbuf;
		*option_len = sizeof(unsigned int);
		break;
	case SO_RCVLOWAT:
		if(*option_len != sizeof(unsigned int))
			return -EINVAL;
		*((unsigned int*)option_value) = opts->so_rcvlowat;
		*option_len = sizeof(unsigned int);
		break;
	case SO_REUSEADDR:
		if(*option_len != sizeof(unsigned int))
			return -EINVAL;
		*((unsigned int*)option_value) = opts->so_reuseaddr;
		*option_len = sizeof(unsigned int);
		break;
	case SO_SNDBUF:
		if(*option_len != sizeof(unsigned int))
			return -EINVAL;
		*((unsigned int*)option_value) = opts->so_sndbuf;
		*option_len = sizeof(unsigned int);
		break;
	case SO_SNDLOWAT:
		if(*option_len != sizeof(unsigned int))
			return -EINVAL;
		*((unsigned int*)option_value) = opts->so_sndlowat;
		*option_len = sizeof(unsigned int);
		break;
		/* non-integer valued options */
	case SO_LINGER:
		if(*option_len != sizeof(struct linger))
			return -EINVAL;
		((struct linger*)option_value)->l_onoff = opts->so_linger.l_onoff;
		((struct linger*)option_value)->l_linger = opts->so_linger.l_linger;
		*option_len = sizeof(struct linger);
		break;
	case SO_RCVTIMEO:
		if(*option_len < sizeof(struct timeval))
			return -EINVAL;
		if(*option_len > sizeof(struct timeval))
			return -EDOM;
		((struct timeval*)option_value)->tv_sec = opts->so_rcvtimeo.tv_sec;
		((struct timeval*)option_value)->tv_usec = opts->so_rcvtimeo.tv_usec;
		*option_len = sizeof(struct timeval);
		break;
	case SO_SNDTIMEO:
		if(*option_len < sizeof(struct timeval))
			return -EINVAL;
		if(*option_len > sizeof(struct timeval))
			return -EDOM;
		((struct timeval*)option_value)->tv_sec = opts->so_sndtimeo.tv_sec;
		((struct timeval*)option_value)->tv_usec = opts->so_sndtimeo.tv_usec;
		*option_len = sizeof(struct timeval);
		break;
	default:
		return -ENOPROTOOPT;
		break;
	};
	return ENOERR;
}
