/**
 * @file
 * @brief Global definitions for the ARP protocol and its extensions
 * @details RFC 826 (ARP), RFC 903 (RARP), RFC 2390 (InARP)
 *
 * @date 10.08.09
 * @author Nikolay Korotky
 * @author Ilia Vaprol
 */

#ifndef NET_IF_ARP_H
#define NET_IF_ARP_H

#include <types.h>
#include <stdlib.h>
#include <net/skbuff.h>

/**
 * Prototypes
 */
struct sk_buff;

/**
 * Global protocol hardware address space identifiers
 */
#define ARPG_HRD_ETHERNET 1   /* Ethernet 10Mbps */
#define ARPG_HRD_LOOPBACK 772 /* Loopback device */

/**
 * Global protocol operation codes
 */
/* ARP operation codes */
#define	ARP_OPER_REQUEST  1 /* ARP request */
#define	ARP_OPER_REPLY    2 /* ARP reply */
/* Reverse ARP operation codes */
#define	RARP_OPER_REQUEST 3 /* RARP request */
#define	RARP_OPER_REPLY   4 /* RARP reply */

#if 0
#define	ARPOP_InREQUEST   8  /* InARP request */
#define	ARPOP_InREPLY     9  /* InARP reply */
#define	ARPOP_NAK         10 /* (ATM)ARP NAK */
#endif

/**
 * Generic protocol header
 */
typedef struct arpghdr {
	__be16 ha_space;        /* Hardware address space */
	__be16 pa_space;        /* Protocol address space */
	unsigned char ha_len;   /* Length of hardware address */
	unsigned char pa_len;   /* Length of protocol address */
	__be16 oper;            /* Operation code */
	unsigned char stuff[1]; /* Additional information with variable-length */
} __attribute__((packed)) arpghdr_t;

/**
 * Additional information of variable length
 */
typedef struct arpg_stuff {
	unsigned char *sha; /* Sender hardware address */
	unsigned char *spa; /* Sender protocol address */
	unsigned char *tha; /* Target hardware address */
	unsigned char *tpa; /* Target protocol address */
} arpg_stuff_t;

/**
 * Generic ARP macros for getting header size
 */
#define ARPG_MIN_HEADER_SIZE       ((size_t)&((struct arpghdr *)0)->stuff[0])
#define ARPG_CALC_HDR_SZ(hln, pln) (ARPG_MIN_HEADER_SIZE + (hln + pln) * 2)
#define ARPG_HEADER_SIZE(arpgh)    ARPG_CALC_HDR_SZ(arpgh->ha_len, arpgh->pa_len)

/**
 * Make arp_stuff structure (setup pointers to raw data)
 */
static inline struct arpg_stuff * arpg_make_stuff(struct arpghdr *arpgh,
		struct arpg_stuff *out_stuff) {
	out_stuff->sha = &arpgh->stuff[0];
	out_stuff->spa = out_stuff->sha + arpgh->ha_len;
	out_stuff->tha = out_stuff->spa + arpgh->pa_len;
	out_stuff->tpa = out_stuff->tha + arpgh->ha_len;
	return out_stuff;
}

/**
 * Get ARP generic header
 */
static inline struct arpghdr * arpg_hdr(const struct sk_buff *skb) {
	return skb->nh.arpgh;
}

#endif /* NET_IF_ARP_H */
