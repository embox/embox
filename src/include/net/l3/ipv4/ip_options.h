/**
 * @file
 * @brief
 *
 * @date 28.09.13
 * @author Ilia Vaprol
 */

#ifndef NET_L3_IPV4_IP_OPTIONS_H_
#define NET_L3_IPV4_IP_OPTIONS_H_

#include <net/skbuff.h>
#include <netinet/in.h>

struct sk_buff;

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
	unsigned char __data;
} ip_options_t;

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
#define IPOPT_RA	(20|IPOPT_CONTROL |IPOPT_COPY) /* ip router arert */

#define	IPOPT_TS_TSONLY		0		/* timestamps only */
#define	IPOPT_TS_TSANDADDR	1		/* timestamps and addresses */
#define	IPOPT_TS_PRESPEC	3		/* specified modules only */

/*
 * Parses a block of options from an IP header
 * and initializes an instance of an ip_options structure accordingly
 */
extern int ip_options_compile(struct sk_buff *skb, struct ip_options *opt);

/*
 * Handles socket buffer route info due to SRR options
 */
extern int ip_options_handle_srr(struct sk_buff *skb);

#endif /* NET_L3_IPV4_IP_OPTIONS_H_ */
