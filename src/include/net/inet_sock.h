/**
 * @file
 * @brief Definitions for inet_sock.
 *
 * @date 18.03.2009
 * @author Anton Bondarev
 */
#ifndef INET_SOCK_H_
#define INET_SOCK_H_

#include <net/sock.h>
#include <net/in.h>

/**
* IP Options
* @faddr - Saved first hop address
* @is_data - Options in __data, rather than skb
* @is_strictroute - Strict source route
* @srr_is_hit - Packet destination addr was our one
* @is_changed - IP checksum more not valid
* @rr_needaddr - Need to record addr of outgoing dev
* @ts_needtime - Need to record timestamp
* @ts_needaddr - Need to record addr of outgoing dev
*/
typedef struct ip_options {
	in_addr_t       faddr;
	unsigned char   optlen;
	unsigned char   srr;
	unsigned char   rr;
	unsigned char   ts;
	__extension__ unsigned char   is_strictroute:1,
		srr_is_hit:1,
		is_changed:1,
		rr_needaddr:1,
		ts_needtime:1,
		ts_needaddr:1;
	unsigned char   router_alert;
	unsigned char   cipso;
	unsigned char   __pad2;
	__extension__ unsigned char __data[0];
} ip_options_t;

/**
* Representation of INET sockets
* @sk - ancestor class
* @pinet6 - pointer to IPv6 control block
* @daddr - Foreign IPv4 addr
* @rcv_saddr - Bound local IPv4 addr
* @dport - Destination port
* @num - Local port
* @saddr - Sending source
* @uc_ttl - Unicast TTL
* @sport - Source port
* @id - ID counter for DF pkts
* @tos - TOS
* @mc_ttl - Multicasting TTL
* @is_icsk - is this an inet_connection_sock?
* @mc_index - Multicast device index
* @mc_list - Group array
* @cork - info to build ip hdr on each ip frag while socket is corked
*/
typedef struct inet_sock {
	/* sk have to be the first member of inet_sock */
	sock_t         sk;

	in_addr_t      daddr;
	in_addr_t      rcv_saddr;
	struct ip_options *opt;
	__be16         dport;
	__u16          num;
	in_addr_t      saddr;
	__s16          uc_ttl;
	__be16         sport;
	__u16          id;
	__u8           tos;
	__u8           mc_ttl;
} inet_sock_t;

static inline inet_sock_t *inet_sk(const sock_t *sk) {
	return (inet_sock_t *) sk;
}

#endif /* INET_SOCK_H_ */
