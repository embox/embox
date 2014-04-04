/**
 * @file
 * @brief Definitions for inet_sock.
 *
 * @date 18.03.09
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#ifndef NET_SOCKET_INET_SOCK_H_
#define NET_SOCKET_INET_SOCK_H_

#include <net/sock.h>
#include <netinet/in.h>
#include <arpa/inet.h> /* TODO remove this */
#include <stdint.h>

struct sock;

/**
 * @struct inet_sock
 * @brief Representation of INET sockets (FixMe)
 *
 * @var sk - ancestor class
 * @var daddr - Foreign IPv4 addr
 * @var rcv_saddr - Bound local IPv4 addr
 * @var opt - IPv4 options
 * @var dport - Destination port
 * @var sport - Source port
 * @var num - Local port
 * @var saddr - Sending source
 * @var uc_ttl - Unicast TTL
 * @var id - ID counter for DF pkts
 * @var tos - TOS
 * @var mc_ttl - Multicasting TTL
 */
typedef struct inet_sock {
	struct sock sk;            /* Base socket class (MUST BE FIRST) */
	struct sockaddr_in src_in; /* address from which the socket
								  receives packets */
	struct sockaddr_in dst_in; /* really address of destonation host */
	int16_t uc_ttl;
	uint16_t id;
} inet_sock_t;

static inline struct inet_sock * to_inet_sock(struct sock *sk) {
	return (struct inet_sock *)sk;
}

static inline const struct inet_sock * to_const_inet_sock(
		const struct sock *sk) {
	return (const struct inet_sock *)sk;
}

#endif /* NET_SOCKET_INET_SOCK_H_ */
