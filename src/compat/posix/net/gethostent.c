/**
 * @file
 * @brief
 *
 * @date 20.08.12
 * @author Ilia Vaprol
 */

#include <arpa/inet.h>
#include <ctype.h>
#include <framework/mod/options.h>
#include <net/util/hostent.h>
#include <netdb.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define MODOPS_PATH_TO_HOSTS OPTION_STRING_GET(path_to_hosts)

#define HOSTENT_BUFF_SZ 512

static FILE *hosts = NULL;
static int is_disposable;

void sethostent(int stayopen) {
	hosts = fopen(MODOPS_PATH_TO_HOSTS, "r");
	is_disposable = !stayopen;
}

void endhostent(void) {
	if (hosts != NULL) {
		fclose(hosts);
		hosts = NULL;
	}
}

#define skip_spaces(ptr) while (*ptr && isspace(*ptr)) ptr++
#define skip_word(ptr) while (*ptr && !isspace(*ptr)) ptr++

struct hostent * gethostent(void) {
	char hostent_buff[HOSTENT_BUFF_SZ];
	struct hostent *he, *result;
	char *tmp, *ip_str, *name;
	struct in_addr ip_addr;

	/* prepare file (if needed) */
	if (hosts == NULL) {
		sethostent(0);
	}

	result = NULL;

	/* read hostent from file */
	while (1) {
		/* Format: <addr> <name> [alias, ...] */

		/* get line */
		tmp = fgets(&hostent_buff[0], sizeof hostent_buff, hosts);
		if (tmp == NULL) {
			break;
		}

		/* skip comments */
		if (*tmp == '#') {
			continue;
		}

		/* create hostent structure */
		he = hostent_create();
		if (he == NULL) {
			break;
		}

		/* get ipv4 address */
		skip_spaces(tmp);
		ip_str = tmp;
		skip_word(tmp);
		if (*tmp == '\0') {
			continue; /* invalid format */
		}
		*tmp++ = '\0';
		if (!inet_aton(ip_str, &ip_addr)) {
			continue; /* bad ip */
		}
		if (hostent_set_addr_info(he, AF_INET, sizeof ip_addr) != 0) {
			break; /* error occurred */
		}
		if (hostent_add_addr(he, (char *)&ip_addr) != 0) {
			break; /* error occurred */
		}

		/* get main name */
		skip_spaces(tmp);
		name = tmp;
		skip_word(tmp);
		*tmp++ = '\0';
		if (*name == '\0') {
			continue; /* unspecified official name */
		}
		if (hostent_set_name(he, name) != 0) {
			break; /* error occurred */
		}

		/* get alternate name */
		while (1) {
			skip_spaces(tmp);
			name = tmp;
			skip_word(tmp);
			*tmp++ = '\0';
			if (*name == '\0') {
				break;
			}
			if (hostent_add_alias(he, name) != 0) {
				break;
			}
		}

		/* save result */
		result = he;

		/* exit */
		break;
	}

	/* close file (if needed) */
	if (is_disposable) {
		endhostent();
	}

	return result;
}
