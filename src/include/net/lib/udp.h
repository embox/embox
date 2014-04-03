/**
 * @file
 * @brief
 *
 * @date 20.10.13
 * @author Ilia Vaprol
 */

#ifndef NET_LIB_UDP_H_
#define NET_LIB_UDP_H_

#include <netinet/in.h>
#include <stdint.h>

/**
 * Prototypes
 */
struct iphdr;
struct ip6hdr;
struct udphdr;

/**
 * Build UDP header
 */
extern void udp_build(struct udphdr *udph, in_port_t src_prt,
		in_port_t dst_prt, uint16_t len);

/**
 * Set UDP check field
 */
extern void udp4_set_check_field(struct udphdr *udph,
		const struct iphdr *iph);
extern void udp6_set_check_field(struct udphdr *udph,
		const struct ip6hdr *ip6h);
extern void udp_set_check_field(struct udphdr *udph,
		const void *nhhdr);

/**
 * Calculate UDP data length
 */
extern size_t udp_data_length(const struct udphdr *udph);

#endif /* NET_LIB_UDP_H_ */
