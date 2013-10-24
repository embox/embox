/**
 * @file
 * @brief
 *
 * @date 20.10.13
 * @author Ilia Vaprol
 */

#ifndef NET_LIB_IPV4_H_
#define NET_LIB_IPV4_H_

#include <netinet/in.h>
#include <stdint.h>

/**
 * Prototype
 */
struct iphdr;

/**
 * Build IPv4 header
 */
extern void ip_build(struct iphdr *iph, uint16_t total_len,
		uint8_t proto, in_addr_t src_ip, in_addr_t dst_ip);

/**
 * Set IPv4 id field
 */
extern void ip_set_id_field(struct iphdr *iph, uint16_t id);

/**
 * Set IPv4 check field
 */
extern void ip_set_check_field(struct iphdr *iph);

/**
 * Calculate IPv4 data length
 */
extern size_t ip_data_length(const struct iphdr *iph);

/**
 * IPv4 pseudo header
 */
struct ip_pseudohdr {
	in_addr_t src_ip;
	in_addr_t dst_ip;
	uint8_t zero;
	uint8_t protocol;
	uint16_t data_len;
} __attribute__((packed));

/**
 * Build IPv4 pseudo header
 */
extern void ip_pseudo_build(const struct iphdr *iph,
		struct ip_pseudohdr *out_ipph);

#endif /* NET_LIB_IPV4_H_ */
