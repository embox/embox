/**
 * @file
 *
 * @date 20.08.12
 * @author Ilia Vaprol
 */

#include <net/netdb.h>
#include <net/in.h>
#include <stdlib.h>
#include <string.h>
#include <net/dns.h>
#include <assert.h>
#include "hostent_api.h"

static struct hostent * get_hostent_from_ip(const char *ip_str) {
	struct hostent *he;
	struct in_addr ip_addr;

	if (!inet_aton(ip_str, &ip_addr)) {
		return NULL; /* bad ip */
	}

	if (((he = hostent_create()) == NULL)
			|| (hostent_set_name(he, inet_ntoa(ip_addr)) != 0)
			|| (hostent_set_addr_info(he, AF_INET, sizeof ip_addr) != 0)
			|| (hostent_add_addr(he, (char *)&ip_addr) != 0)) {
		return NULL; /* error occurred */
	}

	return he;
}

static struct hostent * get_hostent_from_file(const char *hostname) {
	struct hostent *he;
	char **aliases;

	sethostent(1);

	while ((he = gethostent()) != NULL) {
		/* mb it's the official name? */
		if (strcmp(hostname, he->h_name) == 0) {
			break;
		}
		/* mb it's one of the alternative name? */
		for (aliases = he->h_aliases; *aliases != NULL; ++aliases) {
			if (strcmp(hostname, *aliases) == 0) {
				break;
			}
		}
		if (*aliases != NULL) {
			break;
		}
		/* nope. try again */
	}

	endhostent();

	return he;
}

static struct hostent * get_hostent_from_net(const char *hostname) {
	struct hostent *he;
	struct dns_rr *records, *record;
	size_t i, records_sz, addr_len;

	if (dns_query(hostname, DNS_RR_TYPE_A, DNS_RR_CLASS_IN, &records, &records_sz) != 0) {
		return NULL;
	}

	if (records_sz == 0) {
		return NULL;
	}

	assert(records != NULL);
	addr_len = (*(records + 0)).rdlength;

	if (((he = hostent_create()) == NULL)
			|| (hostent_set_name(he, hostname) != 0)
			|| (hostent_set_addr_info(he, AF_INET, addr_len) != 0)) {
		return NULL;
	}

	for (i = 0, record = records; i < records_sz; ++i, ++record) {
		if ((record->rtype == DNS_RR_TYPE_A)
				&& (record->rclass = DNS_RR_CLASS_IN)) {
			assert(record->rdlength == addr_len);
			if (hostent_add_addr(he, &record->rdata.a.address) != 0) {
				return NULL;
			}
		}
	}

	free(records);

	return he;
}

struct hostent * gethostbyname(const char *hostname) {
	struct hostent *he;

	/* 1. If it's IP address (not symbolic name) */
	he = get_hostent_from_ip(hostname);
	if (he != NULL) {
		return he;
	}

	/* 2. Lookup in local machine */
	he = get_hostent_from_file(hostname);
	if (he != NULL) {
		return he;
	}

	/* 3. Finally, try to get answer from nameserver */
	return get_hostent_from_net(hostname);
}

