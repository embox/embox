/**
 * @file
 * @brief
 *
 * @date 25.10.13
 * @author Ilia Vaprol
 */

#ifndef NET_LIB_ICMPV6_H_
#define NET_LIB_ICMPV6_H_

#include <stddef.h>
#include <stdint.h>

/**
 * Prototypes
 */
struct icmp6hdr;
struct ip6hdr;

/**
 * Build ICMPv6 header
 */
extern void icmp6_build(struct icmp6hdr *icmp6h, uint8_t type,
		uint8_t code, const void *body, size_t body_sz);

/**
 * Set ICMPv6 check field
 */
extern void icmp6_set_check_field(struct icmp6hdr *icmp6h,
		const struct ip6hdr *ip6h);

#endif /* NET_LIB_ICMPV6_H_ */
