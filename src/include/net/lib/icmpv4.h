/**
 * @file
 * @brief
 *
 * @date 11.01.14
 * @author Ilia Vaprol
 */

#ifndef NET_LIB_ICMPV4_H_
#define NET_LIB_ICMPV4_H_

#include <stddef.h>
#include <stdint.h>

/**
 * Prototypes
 */
struct icmphdr;
struct iphdr;

/**
 * Build ICMPv4 header
 */
extern void icmp_build(struct icmphdr *icmph, uint8_t type,
		uint8_t code, const void *body, size_t body_sz);

/**
 * Set ICMPv4 check field
 */
extern void icmp_set_check_field(struct icmphdr *icmph,
		const struct iphdr *iph);

#endif /* NET_LIB_ICMPV4_H_ */
