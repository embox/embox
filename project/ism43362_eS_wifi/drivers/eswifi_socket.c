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


void eswifi_async_msg(struct eswifi_dev *eswifi, char *msg, size_t len) {
	static const char msg_tcp_accept[] = "[TCP SVR] Accepted ";

	if (!strncmp(msg, msg_tcp_accept, sizeof(msg_tcp_accept) - 1)) {
		//struct in_addr *sin_addr;
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
#if 0
		for (i = 0; i < ESWIFI_OFFLOAD_MAX_SOCKETS; i++) {
			struct eswifi_off_socket *s = &eswifi->socket[i];
			if (s->context && s->port == port &&
			    s->state == ESWIFI_SOCKET_STATE_ACCEPTING) {
				socket = s;
				break;
			}
		}

		if (!socket) {
			log_error("No listening socket");
			return;
		}

		struct sockaddr_in *peer = (&socket->peer_addr);

		sin_addr = &peer->sin_addr;
		memcpy(&sin_addr->s4_addr, ip, 4);
		peer->sin_port = htons(port);
		peer->sin_family = AF_INET;
		socket->state = ESWIFI_SOCKET_STATE_CONNECTED;
		socket->usage++;

		/* Save information about remote. */
		socket->context->flags |= NET_CONTEXT_REMOTE_ADDR_SET;
		memcpy(&socket->context->remote, &socket->peer_addr,
		       sizeof(struct sockaddr));
#endif
		log_debug("%u.%u.%u.%u connected to port %u",
			ip[0], ip[1], ip[2], ip[3], port);
	}
}

static void eswifi_poll_handler(struct sys_timer *timer, void *param) {
	int err;
    char *buf = "MR\r";
	const char startstr[] = "[SOMA]";
	const char endstr[] = "[EOMA]";
	char rsp[128];
	size_t msg_len;
	

	//__eswifi_socket_select(sk, 0);

	eswifi_lock(param);
    err = ism43362_exchange((char *)buf, strlen(buf), rsp, sizeof(rsp));
    if (err < 0) {
		log_error("Unable to MR");
		eswifi_unlock(param);
		return ;
	}
	if (strncmp(rsp, startstr, sizeof(startstr) - 1)) {
		log_error("Malformed async msg");
		eswifi_unlock(param);
		return;
	}

	/* \r\n[SOMA]...[EOMA]\r\nOK\r\n> */
	msg_len = err - (sizeof(startstr) - 1) - (sizeof(endstr) - 1);
	if (msg_len > 0) {
		eswifi_async_msg(param, rsp + sizeof(startstr) - 1, msg_len);
	}

	eswifi_unlock(param);
	
	return;
}

static void *eswifi_poll_loop(void *param) {
	struct eswifi_dev *dev = param;
	while(1) {
		sleep(1);
		if (dev->state) {
			eswifi_poll_handler(NULL, dev);
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
    eswifi_dev.out_sk = out_sk;
	//timer_start(&eswifi_dev.eswifi_timer, ms2jiffies(ESWIFI_TIMER_TICK));

    //timer_init(&eswifi_dev.eswifi_timer, TIMER_PERIODIC, eswifi_poll_handler, &eswifi_dev);

	res = pthread_create(&eswifi_dev.eswifi_poll_thread, 0, eswifi_poll_loop, &eswifi_dev);
	if (res != 0) {
		return res;
	}
    pthread_detach(eswifi_dev.eswifi_poll_thread);
    return 0;
}

const struct sock_family_ops eswifi_sock_family_ops = {
    .init = eswifi_sock_init,
    .close = eswifi_sock_close,
    .bind = eswifi_sock_bind,
    .connect = eswifi_sock_connect,
    .listen = eswifi_sock_listen,
    .accept = eswifi_sock_accept,
};