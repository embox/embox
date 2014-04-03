/**
 * @file
 * @brief
 *
 * @date 20.10.13
 * @author Ilia Vaprol
 */

#include <arpa/inet.h>
#include <assert.h>
#include <net/l4/udp.h>
#include <net/lib/ipv4.h>
#include <net/lib/ipv6.h>
#include <net/lib/udp.h>
#include <net/util/checksum.h>
#include <netinet/in.h>

void udp_build(struct udphdr *udph, in_port_t src_prt,
		in_port_t dst_prt, uint16_t len) {
	assert(udph != NULL);
	udph->source = src_prt;
	udph->dest = dst_prt;
	udph->len = htons(len);
	udph->check = 0; /* use udp_set_check_field */
}

void udp4_set_check_field(struct udphdr *udph,
		const struct iphdr *iph) {
	struct ip_pseudohdr ipph;

	assert(udph != NULL);

	ip_pseudo_build(iph, &ipph);

	udph->check = 0;
	udph->check = ~fold_short(partial_sum(&ipph, sizeof ipph) +
			partial_sum(udph, ntohs(ipph.data_len))) & 0xFFFF;
}

void udp6_set_check_field(struct udphdr *udph,
		const struct ip6hdr *ip6h) {
	struct ip6_pseudohdr ip6ph;

	assert(udph != NULL);

	ip6_pseudo_build(ip6h, &ip6ph);

	udph->check = 0;
	udph->check = ~fold_short(partial_sum(&ip6ph, sizeof ip6ph) +
			partial_sum(udph, ntohl(ip6ph.len))) & 0xFFFF;
}

void udp_set_check_field(struct udphdr *udph, const void *nhhdr) {
	if (ip_check_version((const struct iphdr *)nhhdr)) {
		udp4_set_check_field(udph, (const struct iphdr *)nhhdr);
	}
	else {
		assert(ip6_check_version((const struct ip6hdr *)nhhdr));
		udp6_set_check_field(udph, (const struct ip6hdr *)nhhdr);
	}
}

size_t udp_data_length(const struct udphdr *udph) {
	assert(udph != NULL);
	assert(ntohs(udph->len) >= UDP_HEADER_SIZE);

	return ntohs(udph->len) - UDP_HEADER_SIZE;
}
