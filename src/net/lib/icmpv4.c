/**
 * @file
 * @brief
 *
 * @date 11.01.14
 * @author Ilia Vaprol
 */

#include <arpa/inet.h>
#include <assert.h>
#include <net/l3/icmpv4.h>
#include <net/lib/icmpv4.h>
#include <net/lib/ipv4.h>
#include <net/util/checksum.h>
#include <string.h>

void icmp_build(struct icmphdr *icmph, uint8_t type,
		uint8_t code, const void *body, size_t body_sz) {
	assert(icmph != NULL);
	icmph->type = type;
	icmph->code = code;
	icmph->check = 0; /* use icmp_set_check_field */
	memcpy(icmph->body, body, body_sz);
}

void icmp_set_check_field(struct icmphdr *icmph,
		const struct iphdr *iph) {
	assert(icmph != NULL);
	icmph->check = 0;
	icmph->check = ptclbsum(icmph, ip_data_length(iph)) & 0xFFFF;
}
