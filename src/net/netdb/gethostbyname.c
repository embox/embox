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

	return NULL;
}

