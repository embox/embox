/**
 * @file
 * @brief
 *
 * @date 20.10.13
 * @author Ilia Vaprol
 */

#include <arpa/inet.h>
#include <assert.h>
#include <net/l3/ipv4/ip.h>
#include <net/lib/ipv4.h>
#include <net/util/checksum.h>
#include <netinet/in.h>

void ip_build(struct iphdr *iph, uint16_t total_len,
		uint8_t proto, in_addr_t src_ip, in_addr_t dst_ip) {
	assert(iph != NULL);
	iph->version = 4;
	iph->ihl = IP_MIN_HEADER_SIZE / 4;
	iph->tos = 0;
	iph->tot_len = htons(total_len);
	iph->id = 0; /* use ip_set_id_field */
	iph->frag_off = htons(IP_DF);
	iph->ttl = 64;
	iph->proto = proto;
	iph->check = 0; /* use ip_set_check_field */
	iph->saddr = src_ip;
	iph->daddr = dst_ip;
}

void ip_set_id_field(struct iphdr *iph, uint16_t id) {
	assert(iph != NULL);
	iph->id = htons(id);
}

void ip_set_check_field(struct iphdr *iph) {
	assert(iph != NULL);
	iph->check = 0;
	iph->check = ptclbsum(iph, IP_HEADER_SIZE(iph));
}

size_t ip_data_length(const struct iphdr *iph) {
	assert(iph != NULL);
	assert(ntohs(iph->tot_len) >= IP_HEADER_SIZE(iph));

	return ntohs(iph->tot_len) - IP_HEADER_SIZE(iph);
}

void ip_pseudo_build(const struct iphdr *iph,
		struct ip_pseudohdr *out_ipph) {
	assert(iph != NULL);
	assert(out_ipph != NULL);
	out_ipph->src_ip = iph->saddr;
	out_ipph->dst_ip = iph->daddr;
	out_ipph->zero = 0;
	out_ipph->protocol = iph->proto;
	out_ipph->data_len = htons(ntohs(iph->tot_len)
			- IP_HEADER_SIZE(iph));
}
