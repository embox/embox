/**
 * @file
 * @brief
 *
 * @date 24.10.13
 * @author Ilia Vaprol
 */

#ifndef NET_LIB_IPV6_H_
#define NET_LIB_IPV6_H_

#include <netinet/in.h>
#include <stddef.h>
#include <stdint.h>

/**
 * Prototype
 */
struct ip6hdr;

/**
 * Build IPv6 header
 */
extern void ip6_build(struct ip6hdr *ip6h, uint16_t payload_len,
		uint8_t nexthdr, uint8_t hop_limit,
		const struct in6_addr *src_ip6,
		const struct in6_addr *dst_ip6);

/**
 * Calculate IPv6 data length
 */
extern size_t ip6_data_length(const struct ip6hdr *ip6h);

/**
 * IPv6 pseudo header
 */
struct ip6_pseudohdr {
	struct in6_addr src_ip6;
	struct in6_addr dst_ip6;
	uint32_t len;
	uint8_t zero[3];
	uint8_t hdr;
} __attribute__((packed));

/**
 * Build IPv6 pseudo header
 */
extern void ip6_pseudo_build(const struct ip6hdr *ip6h,
		struct ip6_pseudohdr *out_ip6ph);

#endif /* NET_LIB_IPV6_H_ */
