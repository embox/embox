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

void udp_set_check_field(struct udphdr *udph) {
	assert(udph != NULL);
	udph->check = 0;
	udph->check = ptclbsum(udph, ntohs(udph->len));
}
