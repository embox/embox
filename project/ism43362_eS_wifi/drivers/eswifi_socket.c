/**
 * @file 
 * @brief ISM43362-M3G-L44-SPI Inventek eS-WiFi module library
 * @author Daria Pechenova
 * @version
 * @date 18.07.2025
 */

 #include <util/log.h>

#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>

#include <util/macro.h>

#include <kernel/time/timer.h>
#include <kernel/time/time.h>

#include <net/l2/ethernet.h>
#include <net/l0/net_entry.h>
#include <net/netdevice.h>
#include <net/inetdevice.h>
#include <net/skbuff.h>
#include <net/cfg80211.h>
#include <net/sock.h>
#include <net/sock_wait.h>
#include <net/l4/tcp.h>

#include <libs/ism43362.h>

#include <framework/mod/options.h>


#include "eswifi_drv.h"

extern int __eswifi_socket_select(struct sock *sk, int idx);

extern int __eswifi_socket_init(struct sock *sk);

extern int __eswifi_sock_bind(struct sock *sk, const struct sockaddr *addr,
                            socklen_t addrlen) ;

extern int __eswifi_sock_connect(struct sock *sk, const struct sockaddr *addr,
                            socklen_t addrlen, int flags);
extern int __eswifi_sock_listen(struct sock *sk, int len); 
extern int __eswifi_sock_accept(struct sock *sk, struct sockaddr *addr,
                            socklen_t *addrlen, int flags);
extern int __eswifi_sock_recvmsg(struct sock *sk, char *buf, int len);

static int eswifi_sock_init(struct sock *sk) {
    __eswifi_socket_init(sk);
    return 0;
}

static int eswifi_sock_close(struct sock *sk) {
    return 0;
}

static int eswifi_sock_bind(struct sock *sk, const struct sockaddr *addr,
                            socklen_t addrlen) {
	__eswifi_sock_bind(sk, addr, addrlen);

	eswifi_dev.src_in.sin_port = ((struct sockaddr_in *)addr)->sin_port;

    return 0;
 }

static int eswifi_sock_connect(struct sock *sk, const struct sockaddr *addr,
                            socklen_t addrlen, int flags) {
    return 0;
}

static int eswifi_sock_listen(struct sock *sk, int backlog) {
    __eswifi_sock_listen(sk, backlog);
    return 0;
}


static void eswifi_lock(struct eswifi_dev *dev) {

}

static void eswifi_unlock(struct eswifi_dev *dev) {

}

/* From tcp_sock layer */
extern int tcp_sock_listen_alloc(struct tcp_sock *tcp_sk);
extern struct tcp_sock *tcp_sock_listen_fetch(struct tcp_sock *tcp_s);

int tcp_sock_ip4_accepted(struct tcp_sock *tcp_newsk,
				struct tcp_sock *parent_sk,
				struct sockaddr_in *src_in,
				struct sockaddr_in *dst_in) {

	struct inet_sock *in;

	in = to_inet_sock(to_sock(tcp_newsk));
	memcpy(&in->src_in, src_in, sizeof(struct sockaddr_in) );
	memcpy(&in->dst_in, dst_in, sizeof(struct sockaddr_in) );
	tcp_newsk->state = TCP_ESTABIL;
	tcp_newsk->parent = parent_sk;
	/* Save new socket to accept queue */
	tcp_sock_lock(parent_sk, TCP_SYNC_CONN_QUEUE);
	{
		list_move_tail(&tcp_newsk->conn_lnk, &parent_sk->conn_ready);
		to_sock(parent_sk)->rx_data_len++;
		sock_notify(to_sock(parent_sk), POLLIN);
	}
	tcp_sock_unlock(parent_sk, TCP_SYNC_CONN_QUEUE);

	return 0;

}

void eswifi_async_msg(struct eswifi_dev *eswifi, char *msg, size_t len) {
	static const char msg_tcp_accept[] = "[TCP SVR] Accepted ";
	struct tcp_sock *tcp_newsk;
	struct tcp_sock *tcp_sk;
	struct sockaddr_in src_in;
	struct sockaddr_in dst_in;

	log_error("async msg: %s", msg);

	if (!strncmp(msg, msg_tcp_accept, sizeof(msg_tcp_accept) - 1)) {
		uint8_t ip[4];
		uint16_t port = 0;
		char *str;
		int i = 0;
		//int ret;

		/* extract client ip/port e.g. 192.168.1.1:8080 */
		/* TODO: use net_ipaddr_parse */
		str = msg + sizeof(msg_tcp_accept) - 1;
		while (*str) {
			if (i < 4) {
				ip[i++] = atoi(str);
			} else if (i < 5) {
				port = atoi(str);
				break;
			}

			while (*str && (*str != '.') && (*str != ':')) {
				str++;
			}

			str++;
		}

		log_debug("%u.%u.%u.%u connected to port %u",
			ip[0], ip[1], ip[2], ip[3], port);
		tcp_sk = to_tcp_sock(eswifi->sk);
		tcp_newsk = NULL;
		if (tcp_sock_listen_alloc(tcp_sk) > 0) {
            tcp_newsk = tcp_sock_listen_fetch(tcp_sk); /* will add it to conn_wait list */
		}
		if (!tcp_newsk) {
			log_error("couldn't alloc new sock");
			return;
		}

		to_sock(tcp_newsk)->sock_netdev = eswifi_dev.netdev;
	
		memcpy(&src_in.sin_addr, ip, 4);
		src_in.sin_port = htons(port);
		src_in.sin_family = AF_INET;

		memcpy(&dst_in.sin_addr, ip, 4);
		dst_in.sin_port = htons(port);
		dst_in.sin_family = AF_INET;

		tcp_sock_ip4_accepted(tcp_newsk, tcp_sk, &src_in, &dst_in);

		//__eswifi_sock_recvmsg(to_sock(tcp_newsk), eswifi_dev.rx_buf, 1460);
	}
}

static void eswifi_poll_handler(struct eswifi_dev *dev) {
	int ret;
    char *buf = "MR\r";

	const char startstr[] = "\r\n[SOMA]";
	const char endstr[] = "[EOMA]\r\nOK\r\n>";
	char rsp[256];
	size_t msg_len;
	

	//__eswifi_socket_select(sk, 0);

	eswifi_lock(dev);
    ret = ism43362_exchange((char *)buf, strlen(buf), rsp, sizeof(rsp));
    if (ret < 0) {
		log_error("Unable to MR");
		eswifi_unlock(dev);
		return ;
	}

	if (strncmp(rsp, startstr, sizeof(startstr) - 1)) {
		log_error("Malformed async msg");
		eswifi_unlock(dev);
		return;
	}

	//log_debug("%s", rsp);
	/* \r\n[SOMA]...[EOMA]\r\nOK\r\n> */
	msg_len = (ret - 1) - (sizeof(startstr) - 1) - (sizeof(endstr) - 1);
	if (msg_len > 0) {
		eswifi_async_msg(dev, rsp + sizeof(startstr) - 1, msg_len);
		dev->state = 0; /* turn off to not using SPI */
#if 0
		char *buf1 = "P?";
		ret = ism43362_exchange((char *)buf1, strlen(buf), rsp, sizeof(rsp));
    	if (ret < 0) {
			log_error("Unable to P?");
			eswifi_unlock(dev);
			return ;
		}
		log_debug("%s", rsp);
#endif
	}

	eswifi_unlock(dev);
	
	return;
}

static void *eswifi_poll_loop(void *param) {
	struct eswifi_dev *dev = param;
	while(1) {
		sleep(1);
		if (dev->state) {
			eswifi_poll_handler(dev);
		}
	}
	return NULL;
}


static int eswifi_sock_accept(struct sock *sk, struct sockaddr *addr,
                            socklen_t *addrlen, int flags, struct sock **out_sk) {
    int res;

    __eswifi_sock_accept(sk, addr, addrlen, flags);
    eswifi_dev.state = ESWIFI_STATE_SERVER_START;
    eswifi_dev.sk = sk;

	//sys_timer_start(&eswifi_dev.eswifi_timer, ms2jiffies(ESWIFI_TIMER_TICK));

    //sys_timer_init(&eswifi_dev.eswifi_timer, SYS_TIMER_PERIODIC, eswifi_poll_handler, &eswifi_dev);

	res = pthread_create(&eswifi_dev.eswifi_poll_thread, 0, eswifi_poll_loop, &eswifi_dev);
	if (res != 0) {
		return res;
	}
    pthread_detach(eswifi_dev.eswifi_poll_thread);
    return 0;
}

static int eswifi_sock_sendmsg(struct sock *sk, struct msghdr *msg, int flags) {
	return 0;

}

static int eswifi_sock_recvmsg(struct sock *sk, struct msghdr *msg, int flags) {
	__eswifi_sock_recvmsg(sk, msg->msg_iov->iov_base, msg->msg_iov->iov_len);

	return 0;
}

const struct sock_family_ops eswifi_sock_family_ops = {
    .init = eswifi_sock_init,
    .close = eswifi_sock_close,
    .bind = eswifi_sock_bind,
    .connect = eswifi_sock_connect,
    .listen = eswifi_sock_listen,
    .accept = eswifi_sock_accept,
	.sendmsg = eswifi_sock_sendmsg,
	.recvmsg = eswifi_sock_recvmsg,
};