/**
 * @file
 * @brief
 *
 * @date 29.03.13
 * @author Ilia Vaprol
 */

#include <arpa/inet.h>
#include <net/lib/dns.h>
#include <net/util/hostent.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <swab.h>
#include <sys/socket.h>
#include <lib/libds/array.h>

struct hostent * gethostbyaddr(const void *addr, socklen_t len,
		int type) {
	int ret;
	struct hostent *he;
	struct dns_result result;
	struct dns_rr *rr;
	struct in_addr in;
	char hostaddr[] = "xxx.xxx.xxx.xxx.in-addr.arpa";
	size_t i;

	if ((type != AF_INET) || (len != sizeof in)) {
		return NULL; /* error: IPv4 supported only */
	}

	memcpy(&in, addr, sizeof in);
	in.s_addr = swab32(in.s_addr);

	if (0 > snprintf(&hostaddr[0], ARRAY_SIZE(hostaddr),
			"%s.in-addr.arpa", inet_ntoa(in))) {
		return NULL;
	}

	ret = dns_query(&hostaddr[0], DNS_RR_TYPE_PTR, DNS_RR_CLASS_IN, &result);
	if (ret != 0) {
		h_errno = HOST_NOT_FOUND;
		return NULL;
	}

	if (((he = hostent_create()) == NULL)
			|| (hostent_set_addr_info(he, AF_INET, len) != 0)
			|| (hostent_add_addr(he, addr) != 0)) {
		dns_result_free(&result);
		return NULL;
	}

	for (i = 0, rr = result.an; i < result.ancount; ++i, ++rr) {
		switch (rr->rtype) {
		default:
			ret = 0;
			break;
		case DNS_RR_TYPE_PTR:
			ret = hostent_set_name(he, &rr->rdata.ptr.ptrdname[0]);
			break;
		}
		if (ret != 0) {
			dns_result_free(&result);
			return NULL;
		}
	}

	dns_result_free(&result);

	return he;
}
