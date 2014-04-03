/**
 * @file
 * @brief
 *
 * @date 20.10.13
 * @author Ilia Vaprol
 */

#include <arpa/inet.h>
#include <assert.h>
#include <net/l4/tcp.h>
#include <net/lib/ipv4.h>
#include <net/lib/ipv6.h>
#include <net/lib/tcp.h>
#include <net/util/checksum.h>
#include <netinet/in.h>
#include <util/binalign.h>

void tcp_build(struct tcphdr *tcph, in_port_t dst_prt,
		in_port_t src_prt, uint8_t data_off, uint16_t window) {
	assert(tcph != NULL);
	tcph->dest = dst_prt;
	tcph->source = src_prt;
	tcph->seq = 0; /* use tcp_set_seq_field */
	tcph->ack_seq = 0; /* use tcp_set_ack_field */
	assert(binalign_check_bound(data_off, 4));
	tcph->doff = data_off / 4;
	tcph->res1 = tcph->cwr = tcph->ece = tcph->urg = tcph->ack
			= tcph->psh = tcph->rst = tcph->syn = tcph->fin = 0;
	tcph->window = htons(window);
	tcph->check = 0; /* use tcp_set_check_field */
	tcph->urg_ptr = 0;
}

void tcp_set_seq_field(struct tcphdr *tcph, uint32_t seq) {
	assert(tcph != NULL);
	tcph->seq = htonl(seq);
}

void tcp_set_ack_field(struct tcphdr *tcph, uint32_t ack_seq) {
	assert(tcph != NULL);
	tcph->ack_seq = htonl(ack_seq);
	tcph->ack = 1;
}

void tcp4_set_check_field(struct tcphdr *tcph,
		const struct iphdr *iph) {
	struct ip_pseudohdr ipph;

	assert(tcph != NULL);

	ip_pseudo_build(iph, &ipph);

	tcph->check = 0;
	tcph->check = ~fold_short(partial_sum(&ipph, sizeof ipph) +
			partial_sum(tcph, ntohs(ipph.data_len))) & 0xFFFF;
}

void tcp6_set_check_field(struct tcphdr *tcph,
		const struct ip6hdr *ip6h) {
	struct ip6_pseudohdr ip6ph;

	assert(tcph != NULL);

	ip6_pseudo_build(ip6h, &ip6ph);

	tcph->check = 0;
	tcph->check = ~fold_short(partial_sum(&ip6ph, sizeof ip6ph) +
			partial_sum(tcph, ntohl(ip6ph.len))) & 0xFFFF;
}

void tcp_set_check_field(struct tcphdr *tcph,
		const void *nhhdr) {
	if (ip_check_version((const struct iphdr *)nhhdr)) {
		tcp4_set_check_field(tcph, (const struct iphdr *)nhhdr);
	}
	else {
		assert(ip6_check_version((const struct ip6hdr *)nhhdr));
		tcp6_set_check_field(tcph, (const struct ip6hdr *)nhhdr);
	}
}

size_t tcp4_data_length(const struct tcphdr *tcph,
		const struct iphdr *iph) {
	assert(tcph != NULL);
	assert(ip_data_length(iph) >= TCP_HEADER_SIZE(tcph));

	return ip_data_length(iph) - TCP_HEADER_SIZE(tcph);
}

size_t tcp6_data_length(const struct tcphdr *tcph,
		const struct ip6hdr *ip6h) {
	assert(tcph != NULL);
	assert(ip6_data_length(ip6h) >= TCP_HEADER_SIZE(tcph));

	return ip6_data_length(ip6h) - TCP_HEADER_SIZE(tcph);
}

size_t tcp_data_length(const struct tcphdr *tcph,
		const void *nhhdr) {
	if (ip_check_version((const struct iphdr *)nhhdr)) {
		return tcp4_data_length(tcph,
				(const struct iphdr *)nhhdr);
	}

	assert(ip6_check_version((const struct ip6hdr *)nhhdr));
	return tcp6_data_length(tcph, (const struct ip6hdr *)nhhdr);
}

size_t tcp4_seq_length(const struct tcphdr *tcph,
		const struct iphdr *iph) {
	assert(tcph != NULL);

	return tcp4_data_length(tcph, iph) + (tcph->syn | tcph->fin);
}

size_t tcp6_seq_length(const struct tcphdr *tcph,
		const struct ip6hdr *ip6h) {
	assert(tcph != NULL);

	return tcp6_data_length(tcph, ip6h) + (tcph->syn | tcph->fin);
}

size_t tcp_seq_length(const struct tcphdr *tcph,
		const void *nhhdr) {
	assert(tcph != NULL);

	return tcp_data_length(tcph, nhhdr) + (tcph->syn | tcph->fin);
}
