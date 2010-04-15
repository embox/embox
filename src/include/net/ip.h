/**
 * @file
 * @brief Definitions for the IP protocol.
 * @details RFC 791
 *
 * @date 11.03.2009
 * @author Anton Bondarev
 */
#ifndef IP_H_
#define IP_H_

#include <net/in.h>
#include <net/skbuff.h>
#include <net/inet_sock.h>

#define __BIG_ENDIAN 1

#define IP_ADDR_LEN      4
#define ICMP_PROTO_TYPE  (unsigned short)0x01
#define UDP_PROTO_TYPE   (unsigned short)0x11


/* IP options */
#define IPOPT_COPY		0x80
#define IPOPT_CLASS_MASK	0x60
#define IPOPT_NUMBER_MASK	0x1f

#define	IPOPT_COPIED(o)		((o)&IPOPT_COPY)
#define	IPOPT_CLASS(o)		((o)&IPOPT_CLASS_MASK)
#define	IPOPT_NUMBER(o)		((o)&IPOPT_NUMBER_MASK)

#define	IPOPT_CONTROL		0x00
#define	IPOPT_RESERVED1		0x20
#define	IPOPT_MEASUREMENT	0x40
#define	IPOPT_RESERVED2		0x60

#define IPOPT_END	(0 |IPOPT_CONTROL)
#define IPOPT_NOOP	(1 |IPOPT_CONTROL)
#define IPOPT_SEC	(2 |IPOPT_CONTROL | IPOPT_COPY)
#define IPOPT_LSRR	(3 |IPOPT_CONTROL | IPOPT_COPY)
#define IPOPT_TIMESTAMP	(4 |IPOPT_MEASUREMENT)
//#define IPOPT_CIPSO	(6 |IPOPT_CONTROL|IPOPT_COPY)
#define IPOPT_RR	(7 |IPOPT_CONTROL)
#define IPOPT_SID	(8 |IPOPT_CONTROL | IPOPT_COPY)
#define IPOPT_SSRR	(9 |IPOPT_CONTROL | IPOPT_COPY)
//#define IPOPT_RA	(20|IPOPT_CONTROL |IPOPT_COPY)

#define	IPOPT_TS_TSONLY		0		/* timestamps only */
#define	IPOPT_TS_TSANDADDR	1		/* timestamps and addresses */
#define	IPOPT_TS_PRESPEC	3		/* specified modules only */


/* IP flags. */
#define IP_CE           0x8000	/* Flag: "Congestion"       */
#define IP_DF           0x4000	/* Flag: "Don't Fragment"   */
#define IP_MF           0x2000	/* Flag: "More Fragments"   */
#define IP_OFFSET       0x1FFF	/* "Fragment Offset" part   */

typedef struct iphdr {
#if defined(__LITTLE_ENDIAN)
	__extension__ __u8 ihl:4,  /* ihl = 5 */
					version:4; /* version = 4 */
#elif defined (__BIG_ENDIAN)
	__extension__ __u8 version:4, /* version = 4 */
						ihl:4;    /* ihl = 5 */
#endif
	__u8        tos;          /**< Type of Services, always 0 */
	__be16      tot_len;      /**< packet length */
	__be16      id;           /**< for packet fragmentation */
	/** ________________________________________________________________
	 * |15_________________|14_________________|13______|12____________0|
	 * |MF (more fragments)|DF (don’t fragment)|always 0|fragment offset|
	 * |___________________|___________________|________|_______________|
	 */
	__be16      frag_off;     /**< flags + position of the fragment in the data flow */
	__u8        ttl;          /**< Time to live */
	__u8        proto;        /**< next header */
	uint16_t    check;        /**< header's checksum */
	in_addr_t   saddr;        /**< source address */
	in_addr_t   daddr;        /**< destination address */
} __attribute__((packed)) iphdr_t;

#define IP_MIN_HEADER_SIZE   (sizeof(struct iphdr))
#define IP_HEADER_SIZE(iph) (((iph)->ihl) << 2)

static inline iphdr_t *ip_hdr(const sk_buff_t *skb) {
	return skb->nh.iph;
}

/**
 * Functions provided by ip.c
 */

/**
 * Init.
 */
extern void ip_init(void);

/**
 * Main IP Receive routine.
 */
extern int ip_rcv(sk_buff_t *pack, net_device_t *dev,
				packet_type_t *pt, net_device_t *orig_dev);

/**
 * Add an ip header to a net_packet and send it out.
 */
extern int ip_send_packet(inet_sock_t *sk, sk_buff_t *pack);

extern int ip_queue_xmit(sk_buff_t *skb, int ipfragok);

extern void ip_send_reply(struct sock *sk, in_addr_t saddr, in_addr_t daddr,
			sk_buff_t *skb, unsigned int len);

extern int rebuild_ip_header(sk_buff_t *pack, unsigned char ttl,
			unsigned char proto, unsigned short id, unsigned short len,
			in_addr_t saddr, in_addr_t daddr/*, ip_options_t *opt*/);

/**
 * Functions provided by ip_options.c
 */

/*
 * Parses a block of options from an IP header
 * and initializes an instance of an ip_options structure accordingly
 */
extern int ip_options_compile(sk_buff_t *skb, ip_options_t *opt);

/*
 * Handles socket buffer route info due to SRR options
 */
extern int ip_options_handle_srr(sk_buff_t *skb);

#endif /* IP_H_ */
