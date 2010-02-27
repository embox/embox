/**
 * @file
 * @brief Global definitions for the ARP (RFC 826) protocol.
 *
 * @date 10.08.2009
 * @author Nikolay Korotky
 */
#ifndef _IF_ARP_H
#define _IF_ARP_H

#include <net/netdevice.h>

/* ARP protocol HARDWARE identifiers. */
#define ARPHRD_ETHER 	1      /* Ethernet 10Mbps */
#define ARPHRD_LOOPBACK 772    /* Loopback device */

/* ARP protocol opcodes. */
#define	ARPOP_REQUEST	1		/* ARP request   */
#define	ARPOP_REPLY     2		/* ARP reply     */
#define	ARPOP_RREQUEST	3		/* RARP request  */
#define	ARPOP_RREPLY	4		/* RARP reply    */
#define	ARPOP_InREQUEST	8		/* InARP request */
#define	ARPOP_InREPLY	9		/* InARP reply   */
#define	ARPOP_NAK       10		/* (ATM)ARP NAK  */

/**
 * This structure defines an ethernet arp header.
 */
typedef struct arphdr {
	__be16         ar_hrd;           /**< format of hardware address = 0x0001;//ethernet */
	__be16         ar_pro;           /**< format of protocol address = 0x0800;//ip */
	unsigned char  ar_hln;           /**< hardware addr len */
	unsigned char  ar_pln;           /**< protocol addr len */
	__be16         ar_op;            /**< ARP opcode (command)    */
	unsigned char  ar_sha[ETH_ALEN]; /**< Sender hardware address */
	in_addr_t      ar_sip;           /**< Sender protocol address */
	unsigned char  ar_tha[ETH_ALEN]; /**< Target hardware address */
	in_addr_t      ar_tip;           /**< Target protocol address */
} __attribute__((packed)) arphdr_t;

#define ARP_HEADER_SIZE (sizeof(struct arphdr))

static inline arphdr_t *arp_hdr(const sk_buff_t *skb) {
	return (arphdr_t *)skb->nh.raw;
}

#endif	/* _IF_ARP_H */
