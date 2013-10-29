/**
 * @file
 * @brief
 *
 * @date 24.10.13
 * @author Ilia Vaprol
 */

#include <arpa/inet.h>
#include <assert.h>
#include <net/l3/ipv6.h>
#include <net/lib/ipv6.h>
#include <net/util/checksum.h>
#include <netinet/in.h>
#include <string.h>

void ip6_build(struct ip6hdr *ip6h, uint16_t payload_len,
		uint8_t nexthdr, uint8_t hop_limit,
		const struct in6_addr *src_ip6,
		const struct in6_addr *dst_ip6) {
	assert(ip6h != NULL);
	ip6h->version = 6;
	ip6h->priority = 0;
	memset(ip6h->flow_lbl, 0, sizeof ip6h->flow_lbl);
	ip6h->payload_len = htons(payload_len);
	ip6h->nexthdr = nexthdr;
	ip6h->hop_limit = hop_limit;
	memcpy(&ip6h->saddr, src_ip6, sizeof ip6h->saddr);
	memcpy(&ip6h->daddr, dst_ip6, sizeof ip6h->daddr);
}

size_t ip6_data_length(const struct ip6hdr *ip6h) {
	assert(ip6h != NULL);

	return ntohs(ip6h->payload_len);
}

void ip6_pseudo_build(const struct ip6hdr *ip6h,
		struct ip6_pseudohdr *out_ip6ph) {
	assert(ip6h != NULL);
	assert(out_ip6ph != NULL);
	memcpy(&out_ip6ph->src_ip6, &ip6h->saddr, sizeof ip6h->saddr);
	memcpy(&out_ip6ph->dst_ip6, &ip6h->daddr, sizeof ip6h->daddr);
	out_ip6ph->len = htonl(ntohs(ip6h->payload_len));
	memset(out_ip6ph->zero, 0, sizeof out_ip6ph->zero);
	out_ip6ph->hdr = ip6h->nexthdr;
}
