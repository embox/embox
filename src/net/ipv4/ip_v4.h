/**
 * \file ip_v4.h
 *
 * \date Mar 11, 2009
 * \author anton
 */

#ifndef IP_V4_H_
#define IP_V4_H_

#define IP_ADDR_LEN      4
#define IP_PROTOCOL_TYPE (unsigned short)0x0800
#define ICMP_PROTO_TYPE  (unsigned short)0x01
#define UDP_PROTO_TYPE   (unsigned short)0x11

typedef struct _iphdr {
	__extension__ unsigned char version:4, ihl:4; /* version = 4, ihl >= 5 */
	unsigned char     tos;              /**< Type of Services, always 0 */
	unsigned short    tot_len;          /**< packet length */
	unsigned short    id;               /**< for packet fragmentation */
	/** ________________________________________________________________
	 * |15____________3|2__________________|1__________________|0_______|
	 * |fragment offset|MF (more fragments)|DF (don’t fragment)|always 0|
	 * |_______________|___________________|___________________|________|
	 */
	unsigned short    frag_off;
	unsigned char     ttl;              /**< Time to live */
	unsigned char     proto;            /**< next header */
	unsigned short    check;            /**< header's checksum */
	unsigned char     saddr[4];         /**< source address */
	unsigned char     daddr[4];         /**< destination address */
} __attribute__((packed)) iphdr;

#define IP_HEADER_SIZE   (sizeof(iphdr))

/**
 * Handle IP packet
 */
int ip_received_packet(struct _net_packet *pack);

#endif /* IP_V4_H_ */
