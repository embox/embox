/*
 * \file if_arp.h
 * \date 10.08.09
 * \author sikmir
 * \brief Global definitions for the ARP (RFC 826) protocol.
 */
#ifndef _IF_ARP_H
#define _IF_ARP_H

#include "net/net.h"
#include "net/if_ether.h"

/* ARP protocol HARDWARE identifiers. */
#define ARPHRD_ETHER 	(unsigned short)0x1      /* Ethernet 10Mbps */

/* ARP protocol opcodes. */
#define	ARPOP_REQUEST	1		/* ARP request   */
#define	ARPOP_REPLY	    2		/* ARP reply     */
#define	ARPOP_RREQUEST	3		/* RARP request  */
#define	ARPOP_RREPLY	4		/* RARP reply    */
#define	ARPOP_InREQUEST	8		/* InARP request */
#define	ARPOP_InREPLY	9		/* InARP reply   */
#define	ARPOP_NAK	   10		/* (ATM)ARP NAK  */

/**
 * This structure defines an ethernet arp header.
 */
typedef struct arphdr {
    unsigned short htype;                   /**< format of hardware address = 0x0001;//ethernet */
    unsigned short ptype;                   /**< format of protocol address = 0x0800;//ip */
    unsigned char  hlen;                    /**< hardware addr len */
    unsigned char  plen;                    /**< protocol addr len */
    unsigned short oper;                    /**< ARP opcode (command)    */
    unsigned char  sha[ETH_ALEN];           /**< Sender hardware address */
    unsigned char  spa[IPV4_ADDR_LENGTH];   /**< Sender protocol address */
    unsigned char  tha[ETH_ALEN];           /**< Target hardware address */
    unsigned char  tpa[IPV4_ADDR_LENGTH];   /**< Target protocol address */
} __attribute__((packed)) arphdr_t;

#endif	/* _IF_ARP_H */
