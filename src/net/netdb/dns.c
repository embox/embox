/**
 * @file
 * @brief
 *
 * @date 22.08.12
 * @author Ilia Vaprol
 */

#include <net/netdb.h>
#include <net/dns.h>
#include <net/in.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int dns_query(const char *query, uint16_t qtype, uint16_t qclass,
		struct dns_rr **out_result, size_t *out_amount) {
	return -ENOSYS;
}

