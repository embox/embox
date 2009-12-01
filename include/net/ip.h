/**
 * @file ip.h
 *
 * @date 11.03.2009
 * @author Anton Bondarev
 * @brief Definitions for the IP protocol.
 */
#ifndef IP_H_
#define IP_H_

#include "lib/inet/netinet/in.h"

#define __BIG_ENDIAN 1

#define IP_ADDR_LEN      4
#define ICMP_PROTO_TYPE  (unsigned short)0x01
#define UDP_PROTO_TYPE   (unsigned short)0x11

/* IP flags. */
#define IP_CE           0x8000          /* Flag: "Congestion"       */
#define IP_DF           0x4000          /* Flag: "Don't Fragment"   */
#define IP_MF           0x2000          /* Flag: "More Fragments"   */
#define IP_OFFSET       0x1FFF          /* "Fragment Offset" part   */

typedef struct iphdr {
#if defined(__LITTLE_ENDIAN)
        __extension__ unsigned char ihl:4,     /* ihl = 5 */
    				    version:4; /* version = 4 */
#elif defined (__BIG_ENDIAN)
	__extension__ unsigned char version:4, /* version = 4 */
	                            ihl:4;     /* ihl = 5 */
#endif
	unsigned char     tos;                 /**< Type of Services, always 0 */
	unsigned short    tot_len;             /**< packet length */
	unsigned short    id;                  /**< for packet fragmentation */
	/** ________________________________________________________________
	 * |15_________________|14_________________|13______|12____________0|
	 * |MF (more fragments)|DF (don’t fragment)|always 0|fragment offset|
	 * |___________________|___________________|________|_______________|
	 */
	unsigned short    frag_off;         /**< flags + position of the fragment in the data flow */
	unsigned char     ttl;              /**< Time to live */
	unsigned char     proto;            /**< next header */
	unsigned short    check;            /**< header's checksum */
        in_addr_t         saddr;            /**< source address */
        in_addr_t         daddr;            /**< destination address */
} __attribute__((packed)) iphdr_t;

#define IP_HEADER_SIZE   (sizeof(struct iphdr))

/**
 * Functions provided by ip.c
 */

struct inet_sock;
struct sk_buff;

/**
 * Handle IP packet
 */
extern int ip_received_packet(struct sk_buff *pack);

/**
 * Add an ip header to a net_packet and send it out.
 */
extern int ip_send_packet(struct inet_sock *sk, struct sk_buff *pack);

#endif /* IP_H_ */
