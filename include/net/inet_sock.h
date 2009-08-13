/**
 * \file ip_sock.h
 *
 * \date Mar 18, 2009
 * \author anton
 * \brief Definitions for inet_sock.
 */
#ifndef INET_SOCK_H_
#define INET_SOCK_H_

#include "net/sock.h"

/** struct ip_options - IP Options
*
* @faddr - Saved first hop address
* @is_data - Options in __data, rather than skb
* @is_strictroute - Strict source route
* @srr_is_hit - Packet destination addr was our one
* @is_changed - IP checksum more not valid
* @rr_needaddr - Need to record addr of outgoing dev
* @ts_needtime - Need to record timestamp
* @ts_needaddr - Need to record addr of outgoing dev
*/
struct ip_options {
    unsigned char   faddr;
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
    __extension__ unsigned char   __data[0];
};


/** struct inet_sock - representation of INET sockets
 *
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

struct inet_sock{
	/* sk have to be the first member of inet_sock */
	struct sock sk;

	unsigned char daddr [4];
	unsigned char rcv_saddr[4];
	unsigned char saddr [4];
	struct ip_options  *opt;
	unsigned short sport;
	unsigned char uc_ttl;
	unsigned char num;

	unsigned short dport;
	unsigned short id;
	unsigned char tos;
	unsigned char mc_ttl;
};

static inline struct inet_sock *inet_sk(const struct sock *sk) {
        return (struct inet_sock *)sk;
}

#endif /* INET_SOCK_H_ */

