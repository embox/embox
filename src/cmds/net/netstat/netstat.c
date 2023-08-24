/**
 * @file
 * @brief Print network statistic
 *
 * @date 30.12.11
 * @author Alexander Kalmuk
 * @author Alexandr Chernakov
 */

#include "stdint.h"
#include <getopt.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#include <net/sock.h>
#include <net/l4/udp.h>
#include <net/l4/tcp.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define NETSTAT_CONT			0x0100

#define NETSTAT_LISTENING		0x0001
#define NETSTAT_NONLISTENING	0x0002
#define NETSTAT_HIER            0x0003
#define NETSTAT_OUTPUT_FLAGS	0x000F

static int netstat_flags;

#define MAX_STATE_STR_LEN 13+1
#define MAX_ADDR_STR_LEN INET_ADDRSTRLEN+1
#define MAX_PORT_STR_LEN 5+1
#define MAX_PROTO_STR_LEN 5+1

static const char * sock_state_str(enum sock_state st) {
	switch (st) {
	case SS_UNKNOWN:       return "UNKNOWN";
	case SS_UNCONNECTED:   return "UNCONNECTED";
	case SS_BOUND:         return "BOUND";
	case SS_LISTENING:     return "LISTENING";
	case SS_CONNECTING:    return "CONNECTING";
	case SS_CONNECTED:     return "CONNECTED";
	case SS_ESTABLISHED:   return "ESTABLISHED";
	case SS_DISCONNECTING: return "DISCONNECTING";
	case SS_CLOSED:        return "CLOSED";
	}

	assertf(0, "illegal socket state %d", st);
	return NULL;
}

static const char * tcp_sock_state_str(enum tcp_sock_state st) {
	switch (st) {
	case TCP_MAX_STATE:    break;
	case TCP_SYN_RECV_PRE: return "SYN_RECV_PRE";
	case TCP_CLOSED:       return "CLOSED";
	case TCP_LISTEN:       return "LISTEN";
	case TCP_SYN_SENT:     return "SYN_SENT";
	case TCP_SYN_RECV:     return "SYN_RECV";
	case TCP_ESTABIL:      return "ESTABLISHED";
	case TCP_FINWAIT_1:    return "FIN_WAIT1";
	case TCP_FINWAIT_2:    return "FIN_WAIT2";
	case TCP_CLOSEWAIT:    return "CLOSE_WAIT";
	case TCP_CLOSING:      return "CLOSING";
	case TCP_LASTACK:      return "LAST_ACK";
	case TCP_TIMEWAIT:     return "TIME_WAIT";
	}

	assertf(0, "illegal tcp socket state %d", st);
	return NULL;
}

void print_inet_sock_info(const struct sock *sk, int hier_mode) {
	const struct inet_sock *in_sk;

	in_sk = (const struct inet_sock *)sk;
	assert(in_sk != NULL);
	assert(in_sk->sk.opt.so_domain == AF_INET);

    if (netstat_flags & NETSTAT_OUTPUT_FLAGS) {
        if (!(netstat_flags & NETSTAT_HIER)) {
            if ((sk->state == SS_LISTENING && ~(netstat_flags & NETSTAT_LISTENING)) ||
                    (sk->state != SS_LISTENING && ~(netstat_flags & NETSTAT_NONLISTENING))) {
                return;
            }
        }
    }

	printf("%*s", MAX_PROTO_STR_LEN, in_sk->sk.opt.so_protocol == IPPROTO_TCP ? "tcp"
			: in_sk->sk.opt.so_protocol == IPPROTO_UDP ? "udp"
			: "unknown");

	if (in_sk->src_in.sin_family == AF_INET) {
		printf("%*s:%*d", MAX_ADDR_STR_LEN, 
				inet_ntoa(in_sk->src_in.sin_addr),
                MAX_PORT_STR_LEN,
				ntohs(in_sk->src_in.sin_port));
	}
	else {
		/* FIXME */
		assert(in_sk->src_in.sin_family == AF_UNSPEC);
		printf("%*s:%*c", MAX_ADDR_STR_LEN, "0.0.0.0", MAX_PORT_STR_LEN, '*');
	}


	if (in_sk->dst_in.sin_family == AF_INET) {
		printf("%*s:%*d", MAX_ADDR_STR_LEN,
                inet_ntoa(in_sk->dst_in.sin_addr),
                MAX_PORT_STR_LEN,
				ntohs(in_sk->dst_in.sin_port));
	}
	else {
		/* FIXME */
		assert(in_sk->src_in.sin_family == AF_UNSPEC);
		printf("%*s:%*c", MAX_ADDR_STR_LEN, "0.0.0.0", MAX_PORT_STR_LEN, '*');
	}

	printf("%*s", MAX_STATE_STR_LEN,
            in_sk->sk.opt.so_protocol == IPPROTO_TCP
			? tcp_sock_state_str(to_tcp_sock(&(in_sk->sk))->state)
			: sock_state_str(sock_get_state(&in_sk->sk)));

    if(hier_mode) {
        if (to_tcp_sock(sk)->parent == NULL) {
            printf("\tParent");
        } else {
            printf("\t|-Child");
        }
    }

	printf("\n");
}

void print_proto_connections(const char *title,
		const struct sock_proto_ops *p_ops,
		void (*print_sock_info)(const struct sock *, int hier_mode),
        int hier_mode) {
	const struct sock *sk = NULL;
    const struct sock *child_sk = NULL;

	if (title != NULL) {
		printf("%s\n", title);
	}

    if (p_ops != NULL) {
        if(hier_mode) {
            sock_foreach(sk, p_ops) {
                struct tcp_sock *sk_tcp = to_tcp_sock(sk);
                /* looking for tcp listening sockets: */
                if((sk->state==SS_LISTENING) && sk_tcp) {
                    (*print_sock_info)(sk, hier_mode);
                    /* looking for it's child sockets */
                    sock_foreach(child_sk, p_ops) {
                        if (to_tcp_sock(child_sk)->parent == sk_tcp) {
                            (*print_sock_info)(child_sk, hier_mode);
                        }
                    }
                    printf("\n");
                }
            }
        } else {
            sock_foreach(sk, p_ops) {
                (*print_sock_info)(sk, hier_mode);
            }
        }
    }
}

int main(int argc, char **argv) {
	int c;

	netstat_flags = 0;

	while ((c = getopt(argc, argv, "cl0")) != -1) {
		switch (c) {
			case 'c':
				netstat_flags |= NETSTAT_CONT;
				break;
			case 'l':
				netstat_flags |= NETSTAT_LISTENING;
				break;
            case '0':
                netstat_flags &= ~NETSTAT_LISTENING; /* override this flag*/
                netstat_flags |= NETSTAT_HIER;
                break;
			default:
				break;
		}
	}

	do {
        printf("Active connections\n");
        printf("%*s%*s%*s%*s\n",
                MAX_PROTO_STR_LEN, "Proto",
                MAX_ADDR_STR_LEN+MAX_PORT_STR_LEN+1, "Local Address:port",
                MAX_ADDR_STR_LEN+MAX_PORT_STR_LEN+1, "Foreign Address:port",
                MAX_STATE_STR_LEN, "State");
		print_proto_connections(NULL, tcp_sock_ops, &print_inet_sock_info, netstat_flags&NETSTAT_HIER);
        /* No hierarchy possible for UDP, so print in normal mode anyway*/
		print_proto_connections(NULL, udp_sock_ops, &print_inet_sock_info, 0);
		if (netstat_flags & NETSTAT_CONT)
			sleep(1);
	} while (netstat_flags & NETSTAT_CONT);

	return 0;
}
