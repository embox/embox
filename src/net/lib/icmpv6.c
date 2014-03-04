/**
 * @file
 * @brief
 *
 * @date 24.10.13
 * @author Ilia Vaprol
 */

#include <arpa/inet.h>
#include <assert.h>
#include <net/l3/icmpv6.h>
#include <net/lib/icmpv6.h>
#include <net/lib/ipv6.h>
#include <net/util/checksum.h>
#include <string.h>

void icmp6_build(struct icmp6hdr *icmp6h, uint8_t type,
		uint8_t code, const void *body, size_t body_sz) {
	assert(icmp6h != NULL);
	icmp6h->type = type;
	icmp6h->code = code;
	icmp6h->check = 0; /* use icmp6_set_check_field */
	memcpy(icmp6h->body, body, body_sz);
}

void icmp6_set_check_field(struct icmp6hdr *icmp6h,
		const struct ip6hdr *ip6h) {
	struct ip6_pseudohdr ip6ph;

	assert(icmp6h != NULL);

	ip6_pseudo_build(ip6h, &ip6ph);

	icmp6h->check = 0;
	icmp6h->check = ~fold_short(partial_sum(&ip6ph, sizeof ip6ph) +
			partial_sum(icmp6h, ntohl(ip6ph.len))) & 0xFFFF;
}
