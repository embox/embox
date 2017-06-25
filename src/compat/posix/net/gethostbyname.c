/**
 * @file
 * @brief
 *
 * @date 20.08.12
 * @author Ilia Vaprol
 */

#include <ctype.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stddef.h>
#include <string.h>
#include <net/lib/dns.h>
#include <net/util/hostent.h>

static int check_ip_format(const char *ip_str) {
	while (*ip_str && (isdigit(*ip_str) || *ip_str == '.'))
		++ip_str;
	return *ip_str;
}

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
	int ret, addr_len;
	struct hostent *he;
	struct dns_result result;
	struct dns_rr *rr;
	size_t i;

	ret = dns_query(hostname, DNS_RR_TYPE_A, DNS_RR_CLASS_IN, &result);
	if (ret != 0) {
		h_errno = HOST_NOT_FOUND;
		return NULL;
	}

	addr_len = result.an->rdlength;

	if (((he = hostent_create()) == NULL)
			|| (hostent_set_name(he, hostname) != 0)
			|| (hostent_set_addr_info(he, AF_INET, addr_len) != 0)) {
		dns_result_free(&result);
		return NULL;
	}

	for (i = 0, rr = result.an; i < result.ancount; ++i, ++rr) {

		if (rr->rdlength != addr_len) {
			continue;
		}

		switch (rr->rtype) {
		default:
			ret = 0;
			break;
		case DNS_RR_TYPE_A:
			ret = hostent_add_addr(he, &rr->rdata.a.address[0]);
			break;
		case DNS_RR_TYPE_AAAA:
			ret = hostent_add_addr(he, &rr->rdata.aaaa.address[0]);
			break;
		}

		switch(ret) {
		case 0:
			/* continue processing */
			break;
		case -ENOMEM:
		case -ERANGE:
			/* some addresses can't be inserted, return he as is */
			goto out_rr_loop;
		default:
			/* don't know what to do, through he out */
			he = NULL;
			goto out_rr_loop;
		}
	}
out_rr_loop:

	dns_result_free(&result);

	return he;
}

struct hostent * gethostbyname(const char *name) {
	struct hostent *he;

	if (name == NULL) {
		return NULL;
	}

	/* 1. If it's IP address (not symbolic name) */
	if (!check_ip_format(name)) {
		he = get_hostent_from_ip(name);
		if (he == NULL) {
			h_errno = HOST_NOT_FOUND;
			return NULL;
		}
		return he;
	}

	/* 2. Lookup in local machine */
	he = get_hostent_from_file(name);
	if (he != NULL) {
		return he;
	}

	/* 3. Finally, try to get answer from nameserver */
	return get_hostent_from_net(name);
}
