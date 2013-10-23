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

void tcp_set_check_field(struct tcphdr *tcph,
		const struct iphdr *iph) {
	struct ip_pseudohdr ipph;

	assert(tcph != NULL);
	assert(iph != NULL);

	ip_pseudo_build(iph, &ipph);

	tcph->check = 0;
	tcph->check = ~fold_short(partial_sum(&ipph, sizeof ipph) +
			partial_sum(tcph, ntohs(ipph.data_len))) & 0xFFFF;
}
