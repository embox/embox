/**
 * @file
 * @brief Example showing the use of getsockopt.
 * displays default socket options
 *
 * @date 13.01.10
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <framework/example/self.h>
#include <stdio.h>
#include <net/net.h>
#include <net/ip.h>

EMBOX_EXAMPLE(exec);

struct sock_type_name{
	char *name;
} sock_type_name_buf[SOCK_TYPE_MAX] = {
	{"UNKNOWN"}, {"SOCK_STREAM"}, {"SOCK_DGRAM"}, {"SOCK_RAW"}
};

enum opt_type{
	OT_INT,
	OT_LINGER,
	OT_TIMEVAL
};

struct sock_opts {
  char	   *opt_str;
  int		opt_level;
  int		opt_name;
	enum opt_type type;
} sock_opts[] = {
	{"SO_ACCEPTCONN", SOL_SOCKET, SO_ACCEPTCONN, OT_INT},
	{"SO_BROADCAST",		SOL_SOCKET,	SO_BROADCAST, OT_INT},
	{"SO_DEBUG",			SOL_SOCKET,	SO_DEBUG, OT_INT},
	{"SO_DONTROUTE",		SOL_SOCKET,	SO_DONTROUTE, OT_INT},
	{"SO_ERROR",			SOL_SOCKET,	SO_ERROR, OT_INT},
	{"SO_KEEPALIVE",		SOL_SOCKET,	SO_KEEPALIVE, OT_INT},
	{"SO_LINGER",		SOL_SOCKET,	SO_LINGER, OT_LINGER},
	{"SO_OOBINLINE",		SOL_SOCKET,	SO_OOBINLINE, OT_INT},
	{	"SO_RCVBUF",		SOL_SOCKET,	SO_RCVBUF, OT_INT},
	{"SO_SNDBUF",		SOL_SOCKET,	SO_SNDBUF, OT_INT},
	{"SO_RCVLOWAT",		SOL_SOCKET,	SO_RCVLOWAT, OT_INT},
	{"SO_SNDLOWAT",		SOL_SOCKET,	SO_SNDLOWAT, OT_INT},
	{"SO_RCVTIMEO",		SOL_SOCKET,	SO_RCVTIMEO, OT_TIMEVAL},
	{"SO_SNDTIMEO",		SOL_SOCKET,	SO_SNDTIMEO, OT_TIMEVAL},
	{"SO_REUSEADDR",		SOL_SOCKET,	SO_REUSEADDR, OT_INT},
#ifdef	SO_REUSEPORT
	{"SO_REUSEPORT",		SOL_SOCKET,	SO_REUSEPORT, OT_INT},
#else
	{"SO_REUSEPORT",		0,			0, OT_INT},
#endif
	{"SO_TYPE",			SOL_SOCKET,	SO_TYPE, OT_INT},
#ifdef SO_USELOOPBACK
	{"SO_USELOOPBACK",	SOL_SOCKET,	SO_USELOOPBACK, OT_INT},
#else
	{"SO_USELOOPBACK",	0,	0, OT_INT},
#endif
#if 0
	{"IP_TOS",			IPPROTO_IP,	IP_TOS},
	{"IP_TTL",			IPPROTO_IP,	IP_TTL},
	{"TCP_MAXSEG",		IPPROTO_TCP,TCP_MAXSEG},
	{"TCP_NODELAY",		IPPROTO_TCP,TCP_NODELAY},
#endif
	{NULL,				0,			0}
};

static int exec(int argc, char **argv)
{
	int	fd, len;
	char mem[sizeof(struct timeval)];
	struct linger *ling_ptr;
	struct timeval *time_ptr;
	/* int *int_ptr; */
	struct sock_opts	*ptr;

	fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	ptr = &sock_opts[0];
	while(ptr->opt_str){
		switch(ptr->type){
		case OT_INT:
			len = sizeof(int);
			break;
		case OT_LINGER:
			len = sizeof(struct linger);
			break;
		case OT_TIMEVAL:
			len = sizeof(struct timeval);
			break;
		}
		if(getsockopt(fd, ptr->opt_level, ptr->opt_name, &mem[0], &len) == 0){
			switch(ptr->type){
			case OT_INT:
				if(ptr->opt_name!=SO_TYPE)
					printf("%s:\t%d\n", ptr->opt_str, *((int*)&mem));
				else
					printf("%s:\t%s\n", ptr->opt_str, sock_type_name_buf[*((int*)&mem)].name);
				break;
			case OT_LINGER:
				ling_ptr = ((struct linger*)&mem[0]);
				printf("%s:\tl_onoff=%d, l_linger=%d\n", ptr->opt_str,
							 ling_ptr->l_onoff, ling_ptr->l_linger);
				break;
			case OT_TIMEVAL:
				time_ptr = ((struct timeval*)&mem[0]);
				printf("%s:\tsec=%ld, usec=%ld\n", ptr->opt_str,
							 time_ptr->tv_sec, time_ptr->tv_usec);
				break;
			default:
				break;
			}
		}else{
			printf("%s:\tnot supported\n", ptr->opt_str);
		}
		ptr++;
	}

	socket_close(fd);

	return 0;
}
