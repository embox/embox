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
#include <netinet/in.h>
#include <arpa/inet.h>

#include <util/macro.h>

#include <net/l2/ethernet.h>
#include <net/l0/net_entry.h>
#include <net/netdevice.h>
#include <net/inetdevice.h>
#include <net/skbuff.h>
#include <net/cfg80211.h>
#include <net/sock.h>

#include <libs/ism43362.h>

#include "eswifi_drv.h"

#include <framework/mod/options.h>

#define ESWIFI_TYPE_PROTOCOL_TCP        0
#define ESWIFI_TYPE_PROTOCOL_UDP        1
#define ESWIFI_TYPE_PROTOCOL_UDP_LITE   2
#define ESWIFI_TYPE_PROTOCOL_TCP_SSL    3

int __eswifi_socket_select(struct sock *sk, int idx) {
    int err;
    char buf[64];
	char rx_buffer[64];

    snprintf(buf, sizeof(buf), "P0=%d\r", idx);
    err = ism43362_exchange((char *)buf, strlen(buf), rx_buffer, sizeof(rx_buffer));

    return err;
}

int __eswifi_socket_init(struct sock *sk) {
    int err;
    char buf[64];
	char rx_buffer[64];
    int proto;

	__eswifi_socket_select(sk, 0);

    if (sk->opt.so_type == SOCK_STREAM && sk->opt.so_protocol == IPPROTO_TCP) {
        proto = ESWIFI_TYPE_PROTOCOL_TCP;
    } else {
        proto = ESWIFI_TYPE_PROTOCOL_TCP;
    }

    snprintf(buf, sizeof(buf), "P1=%d\r", proto);
    err = ism43362_exchange((char *)buf, strlen(buf), rx_buffer, sizeof(rx_buffer));

    return err;
}

int __eswifi_sock_bind(struct sock *sk, const struct sockaddr *addr,
                            socklen_t addrlen) {
    int err;
    char buf[64];
	char rx_buffer[64];
    uint16_t port;

	__eswifi_socket_select(sk, 0);

	// if (addr->sa_family != AF_INET) {
	// 	log_error("Only AF_INET is supported!");
	// 	return -EPFNOSUPPORT;
	// }

	port = ntohs(((struct sockaddr_in *)addr)->sin_port);

	/* Set Local Port */
	snprintf(buf, sizeof(buf), "P2=%d\r", port);
    err = ism43362_exchange((char *)buf, strlen(buf), rx_buffer, sizeof(rx_buffer));
	if (err < 0) {
		log_error("Unable to set local port");
		return -EIO;
	}
#if 0
	if (socket->type == ESWIFI_TRANSPORT_UDP) {
		/* No listen or accept, so start UDP server now */
		snprintf(->buf, sizeof(buf), "P5=1\r");
		err = eswifi_at_cmd(eswifi, buf);
		if (err < 0) {
			LOG_ERR("Unable to start UDP server");
			return -EIO;
		}
	}
#endif
    return 0;
 }

int __eswifi_sock_connect(struct sock *sk, const struct sockaddr *addr,
                            socklen_t addrlen, int flags) {
    return 0;
}

int __eswifi_sock_listen(struct sock *sk, int len) {
    int err;
    char buf[64];
	char rx_buffer[64];

	__eswifi_socket_select(sk, 0);

	/* Set backlog */
	snprintf(buf, sizeof(buf), "P8=%d\r", len);
    err = ism43362_exchange((char *)buf, strlen(buf), rx_buffer, sizeof(rx_buffer));
    if (err < 0) {
		log_error("Unable to listen");
		return -EIO;
	}

    return 0;
}

int __eswifi_sock_accept(struct sock *sk, struct sockaddr *addr,
                            socklen_t *addrlen, int flags) {
    int err;
    char buf[64];
	char rx_buffer[64];

	__eswifi_socket_select(sk, 0);

	/* Start TCP server */
	snprintf(buf, sizeof(buf), "P5=%d\r", 1);
    err = ism43362_exchange((char *)buf, strlen(buf), rx_buffer, sizeof(rx_buffer));
    if (err < 0) {
		log_error("Unable to accept");
		return -EIO;
	}

    return 0;
}
