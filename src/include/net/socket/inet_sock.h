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
	struct sock    sk;
	in_addr_t      saddr;     /* really source address of socket */
	in_addr_t      rcv_saddr; /* address from which the socket receives packets
								 (this equals to saddr or INADDR_ANY) */
	in_addr_t      daddr;     /* really address of destonation host */
	struct ip_options *opt;
	in_port_t      dport;
	in_port_t      sport;
	int sport_is_alloced;         /* non-zero if port was alloced */
	int16_t        uc_ttl;
	uint16_t       id;
	uint8_t        tos;
	uint8_t        mc_ttl;
} inet_sock_t;

static inline struct inet_sock * inet_sk(struct sock *sk) {
	return (struct inet_sock *)sk;
}

#endif /* NET_SOCKET_INET_SOCK_H_ */
