/**
 * @file
 * @brief Definitions for the IP protocol.
 * @details RFC 791
 *
 * @date 11.03.09
 * @author Anton Bondarev
 */

#ifndef NET_L3_IPV4_IP_H_
#define NET_L3_IPV4_IP_H_

#include <arpa/inet.h>
#include <netinet/in.h>
#include <endian.h>

#include <net/skbuff.h>
#include <net/socket/inet_sock.h>
#include <net/util/checksum.h>
#include <net/netdevice.h>
#include <net/sock.h>
#include <linux/types.h>


/**
 * Prototypes
 */
struct sk_buff;
struct packet_type;
struct net_device;
struct sock;

#define IP_ADDR_LEN      4
#define IPv6_ADDR_LEN    16

/* IP flags. */
#define IP_CE           0x8000	/* Flag: "Congestion"       */
#define IP_DF           0x4000	/* Flag: "Don't Fragment"   */
#define IP_MF           0x2000	/* Flag: "More Fragments"   */
#define IP_OFFSET       0x1FFF	/* "Fragment Offset" part   */

typedef struct iphdr {
#if  __BYTE_ORDER == __LITTLE_ENDIAN
	__u8 ihl:4,  /* ihl = 5 */
	version: 4;  /* version = 4 */
#elif  __BYTE_ORDER == __BIG_ENDIAN
	__u8 version:4, /* version = 4 */
		 ihl:4; /* ihl = 5 */
#endif
	__u8        tos;          /**< Type of Services, always 0 */
	__be16      tot_len;      /**< packet length */
	__be16      id;           /**< for packet fragmentation */
	/** ________________________________________________________________
	 * |15_________________|14___________________|13______|12____________0|
	 * |MF (more fragments)|DF (don’t fragment)  |always 0|fragment offset|
	 * |___________________|_____________________|________|_______________|
	 */
	__be16      frag_off;     /**< flags + position of the fragment in the data flow */
	__u8        ttl;          /**< Time to live */
	__u8        proto;        /**< next header */
	__be16    check;        /**< header's checksum */
	in_addr_t   saddr;        /**< source address */
	in_addr_t   daddr;        /**< destination address */
} __attribute__((packed)) iphdr_t;



#define IP_MIN_HEADER_SIZE   (sizeof(struct iphdr))
#define IP_HEADER_SIZE(iph) (((iph)->ihl) << 2)

static inline iphdr_t *ip_hdr(const struct sk_buff *skb) {
	return skb->nh.iph;
}

/* Generate a checksum for an outgoing IP datagram. */
/* NOTE: maybe rename ip_send_check to ip_send_cheksum for clarity?
   current name is ambiguous*/
static inline void ip_send_check(iphdr_t *iph) {
	iph->check = 0;
	iph->check = ptclbsum((void *) iph, IP_HEADER_SIZE(iph));
}

/* Init IP header with given parameters */
static inline void init_ip_header(iphdr_t *hdr, uint8_t proto, __be16 ip_id, __be16 tot_len, __u8 tos,
		in_addr_t saddr, in_addr_t daddr) {
	hdr->version = 4;
	hdr->ihl = IP_MIN_HEADER_SIZE >> 2;
	hdr->saddr = saddr;
	hdr->daddr = daddr;
	hdr->tot_len = tot_len;
	hdr->ttl = 255;
	hdr->id = ip_id;
	hdr->tos = tos;
	hdr->frag_off = 0;
	hdr->proto = proto;
	ip_send_check(hdr);
}

/**
 * Functions provided by ip.c
 */

/**
 * Add an ip header to a net_packet and send it out.
 * It mignt be called with/without socket-related data.
 * Whan socket-related info is present we may use it to build some headers.
 * Current usage:
 *	ICMP: no socket, IP/ICMP headers are build, LL header unknown
 *	UDP: socket, UDP header is built, IP header is placed, LL header unknown
 *	RAW: socket, IP header is ready, but LL header unknown
 *	TCP: socket, TCP header is built, IP header is placed, LL header unknown
 */
extern int ip_send_packet(struct inet_sock *sk,
		struct sk_buff *pack, const struct sockaddr_in *to);

/**
 * Perform forwarding of obtained packet
 */
extern int ip_forward_packet(struct sk_buff *skb);

/**
 * notify an ip socket about icmp error
 **/
extern void ip_v4_icmp_err_notify(struct sock *sk, int type, int code);

#endif /* NET_L3_IPV4_IP_H_ */
