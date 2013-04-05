/**
 * @file
 * @brief Definitions for inet_sock.
 *
 * @date 18.03.09
 * @author Anton Bondarev
 */

#ifndef NET_INET_SOCK_H_
#define NET_INET_SOCK_H_

#include <net/sock.h>
#include <arpa/inet.h>
#include <stdint.h>

/**
 * @struct ip_options
 * @brief IP Options
 *
 * @var faddr - Saved first hop address
 * @var is_data - Options in __data, rather than skb
 * @var is_strictroute - Strict source route
 * @var srr_is_hit - Packet destination addr was our one
 * @var is_changed - IP checksum more not valid
 * @var rr_needaddr - Need to record addr of outgoing dev
 * @var ts_needtime - Need to record timestamp
 * @var ts_needaddr - Need to record addr of outgoing dev
 */
typedef struct ip_options {
	in_addr_t       faddr;
	unsigned char   optlen;
	unsigned char   srr;
	unsigned char   rr;
	unsigned char   ts;
	unsigned char
			is_strictroute:1,
			srr_is_hit:1,
			is_changed:1,
			rr_needaddr:1,
			ts_needtime:1,
			ts_needaddr:1;
	unsigned char   router_alert;
	unsigned char   cipso;
	unsigned char   __pad2;
	unsigned char __data[0];
} ip_options_t;

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
	int16_t        uc_ttl;
	uint16_t       id;
	uint8_t        tos;
	uint8_t        mc_ttl;
} inet_sock_t;

static inline struct inet_sock * inet_sk(struct sock *sk) {
	return (struct inet_sock *)sk;
}

extern struct sock * inet_create_sock(struct proto *prot,
		int type, int protocol);

#endif /* NET_INET_SOCK_H_ */
