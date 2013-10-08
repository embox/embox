/**
 * @file
 * @brief Definitions for inet_sock.
 *
 * @date 18.03.09
 * @author Anton Bondarev
 */

#ifndef NET_SOCKET_INET_SOCK_H_
#define NET_SOCKET_INET_SOCK_H_

#include <net/sock.h>
#include <arpa/inet.h>
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
	/* sk have to be the first member of inet_sock */
	struct sock sk;
	int src_port_alloced;      /* non-zero if port was alloced */
	struct sockaddr_in src_in; /* address from which the socket
								  receives packets */
	struct sockaddr_in dst_in; /* really address of destonation host */
	int16_t uc_ttl;
	uint16_t id;
} inet_sock_t;

static inline struct inet_sock * inet_sk(struct sock *sk) {
	return (struct inet_sock *)sk;
}

#endif /* NET_SOCKET_INET_SOCK_H_ */
