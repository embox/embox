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

#include <util/macro.h>

#include <net/l2/ethernet.h>
#include <net/l0/net_entry.h>
#include <net/netdevice.h>
#include <net/inetdevice.h>
#include <net/skbuff.h>
#include <net/cfg80211.h>
#include <net/sock.h>

#include <libs/ism43362.h>

#include <framework/mod/options.h>


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

static int eswifi_sock_accept(struct sock *sk, struct sockaddr *addr,
                            socklen_t *addrlen, int flags, struct sock **out_sk) {
    __eswifi_sock_accept(sk, addr, addrlen, flags);
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