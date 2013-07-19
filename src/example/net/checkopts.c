/**
 * @file
 * @brief Example showing the use of getsockopt.
 * displays default socket options. sets them to inverse and displays again
 *
 * @date 23.04.12
 * @author Timur Abdukadyrov
 */

#include <util/macro.h>
#include <framework/example/self.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>

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
#ifdef	SO_ACCEPTCONN
	{"SO_ACCEPTCONN", SOL_SOCKET, SO_ACCEPTCONN, OT_INT},
#else
	{"SO_ACCEPTCONN", 0, 0, 0},
#endif
#ifdef SO_BROADCAST
	{"SO_BROADCAST", SOL_SOCKET, SO_BROADCAST, OT_INT},
#else
	{"SO_BROADCAST", 0, 0, 0},
#endif
#ifdef SO_DEBUG
	{"SO_DEBUG", SOL_SOCKET, SO_DEBUG, OT_INT},
#else
	{"SO_DEBUG", 0, 0, 0},
#endif
#ifdef SO_DONTROUTE
	{"SO_DONTROUTE", SOL_SOCKET, SO_DONTROUTE, OT_INT},
#else
	{"SO_DONTROUTE", 0, 0, 0},
#endif
#ifdef SO_ERROR
	{"SO_ERROR", SOL_SOCKET, SO_ERROR, OT_INT},
#else
	{"SO_ERROR", 0, 0, 0},
#endif
#ifdef SO_KEEPALIVE
	{"SO_KEEPALIVE", SOL_SOCKET, SO_KEEPALIVE, OT_INT},
#else
	{"SO_KEEPALIVE", 0, 0, 0},
#endif
#ifdef SO_LINGER
	{"SO_LINGER", SOL_SOCKET, SO_LINGER, OT_LINGER},
#else
	{"SO_LINGER", 0, 0, 0},
#endif
#ifdef SO_OOBINLINE
	{"SO_OOBINLINE", SOL_SOCKET, SO_OOBINLINE, OT_INT},
#else
	{"SO_OOBINLINE", 0, 0, 0},
#endif
#ifdef SO_RCVBUF
	{"SO_RCVBUF", SOL_SOCKET, SO_RCVBUF, OT_INT},
#else
	{"SO_RCVBUF", 0, 0, 0},
#endif
#ifdef SO_SNDBUF
	{"SO_SNDBUF", SOL_SOCKET, SO_SNDBUF, OT_INT},
#else
	{"SO_SNDBUF", 0, 0, 0},
#endif
#ifdef SO_RCVLOWAT
	{"SO_RCVLOWAT", SOL_SOCKET, SO_RCVLOWAT, OT_INT},
#else
	{"SO_RCVLOWAT", 0, 0, 0},
#endif
#ifdef SO_SNDLOWAT
	{"SO_SNDLOWAT", SOL_SOCKET, SO_SNDLOWAT, OT_INT},
#else
	{"SO_SNDLOWAT", 0, 0, 0},
#endif
#ifdef SO_RCVTIMEO
	{"SO_RCVTIMEO", SOL_SOCKET, SO_RCVTIMEO, OT_TIMEVAL},
#else
	{"SO_RCVTIMEO", 0, 0, 0},
#endif
#ifdef SO_SNDTIMEO
	{"SO_SNDTIMEO", SOL_SOCKET, SO_SNDTIMEO, OT_TIMEVAL},
#else
	{"SO_SNDTIMEO", 0, 0, 0},
#endif
#ifdef SO_REUSEADDR
	{"SO_REUSEADDR", SOL_SOCKET, SO_REUSEADDR, OT_INT},
#else
	{"SO_REUSEADDR", 0, 0, 0},
#endif
#ifdef	SO_REUSEPORT
	{"SO_REUSEPORT", SOL_SOCKET, SO_REUSEPORT, OT_INT},
#else
	{"SO_REUSEPORT", 0, 0, 0},
#endif
#ifdef SO_TYPE
	{"SO_TYPE", SOL_SOCKET, SO_TYPE, OT_INT},
#else
	{"SO_TYPE", 0, 0, 0},
#endif
#ifdef SO_USELOOPBACK
	{"SO_USELOOPBACK", SOL_SOCKET, SO_USELOOPBACK, OT_INT},
#else
	{"SO_USELOOPBACK", 0, 0, 0},
#endif
#if 0
	{"IP_TOS", IPPROTO_IP, IP_TOS},
	{"IP_TTL", IPPROTO_IP, IP_TTL},
	{"TCP_MAXSEG", IPPROTO_TCP, TCP_MAXSEG},
	{"TCP_NODELAY", IPPROTO_TCP, TCP_NODELAY},
#endif
	{NULL,				0,			0}
};

static void display_options_and_set_inverse(struct sock_opts *ptr, int sockfd){
	int len;
	struct linger *ling_ptr;
	struct timeval *time_ptr;
	char mem[sizeof(struct timeval)];

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
		if(getsockopt(sockfd, ptr->opt_level, ptr->opt_name, &mem[0], &len) == 0){
			switch(ptr->type){
			case OT_INT:
				if(ptr->opt_name!=SO_TYPE)
					printf("%s:\t%d\n", ptr->opt_str, *((int*)&mem));
				else
					printf("%s:\t%s\n", ptr->opt_str, sock_type_name_buf[*((int*)&mem)].name);
				/* set integer to 1 if 0 and reverse */
				*((int*)&mem) = *((int*)&mem) == 0 ? 1 : 0;
				if((setsockopt(sockfd, ptr->opt_level, ptr->opt_name, *(&mem), len))!=0)
					printf("%s: read only\n", ptr->opt_str);
				break;
			case OT_LINGER:
				ling_ptr = ((struct linger*)&mem[0]);
				printf("%s:\tl_onoff=%d, l_linger=%d\n", ptr->opt_str,
							 ling_ptr->l_onoff, ling_ptr->l_linger);
				/* set linger to l_onoff = 1 and l_linger = 500 if l_onoff = 0 and reverse */
				((struct linger*)&mem)->l_onoff = ((struct linger*)&mem)->l_onoff == 0 ? 1 : 0;
				((struct linger*)&mem)->l_linger = ((struct linger*)&mem)->l_onoff == 0 ? 0 : 500;
				if((setsockopt(sockfd, ptr->opt_level, ptr->opt_name, *(&mem), len))!=0)
					printf("%s: read only\n", ptr->opt_str);
				break;
			case OT_TIMEVAL:
				time_ptr = ((struct timeval*)&mem[0]);
				printf("%s:\tsec=%ld, usec=%ld\n", ptr->opt_str,
							 time_ptr->tv_sec, time_ptr->tv_usec);
				/* set timeval to 500 500 if 0 0 and reverse */
				((struct timeval*)&mem)->tv_sec = ((struct timeval*)&mem)->tv_sec == 0 ? 500 : 0;
				((struct timeval*)&mem)->tv_usec = ((struct timeval*)&mem)->tv_usec == 0 ? 500 : 0;
				if((setsockopt(sockfd, ptr->opt_level, ptr->opt_name, *(&mem), len))!=0)
					printf("%s: read only\n", ptr->opt_str);
				break;
			default:
				break;
			}
		}else{
			printf("%s:\tnot supported\n", ptr->opt_str);
		}
		ptr++;
	}
}

static int exec(int argc, char **argv)
{
	int	sockfd;

	/* display message */
	printf("Check default socket options example. Module is %s\n", MACRO_STRING(__EMBUILD_MOD__));
	printf("shows the functioning of berkeley sockets getsockopt and setsockopt methods.");
	printf("Get and display default socket options for TCP socket.\n");
	printf("Set them to inverse and display again.\n");

	sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	display_options_and_set_inverse(&sock_opts[0], sockfd);
	printf("\nInversed options:\n");
	display_options_and_set_inverse(&sock_opts[0], sockfd);

	close(sockfd);

	return 0;
}
