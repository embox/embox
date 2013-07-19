/**
 * @file
 * @brief
 *
 * @date 29.03.13
 * @author Ilia Vaprol
 */

#include <arpa/inet.h>
#include <ctype.h>
#include <framework/mod/options.h>
#include <net/util/netent.h>
#include <netdb.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define MODOPS_PATH_TO_NETWORKS OPTION_STRING_GET(path_to_networks)

#define NETENT_BUFF_SZ 512

static FILE *networks = NULL;
static int is_disposable;

void setnetent(int stayopen) {
	networks = fopen(MODOPS_PATH_TO_NETWORKS, "r");
	is_disposable = !stayopen;
}

void endnetent(void) {
	if (networks != NULL) {
		fclose(networks);
		networks = NULL;
	}
}

#define skip_spaces(ptr) while (*ptr && isspace(*ptr)) ptr++
#define skip_word(ptr) while (*ptr && !isspace(*ptr)) ptr++

struct netent * getnetent(void) {
	char netent_buff[NETENT_BUFF_SZ];
	struct netent *ne, *result;
	char *tmp, *name, *net_str;
	struct in_addr net;

	/* prepare file (if needed) */
	if (networks == NULL) {
		setnetent(0);
	}

	result = NULL;

	/* read hostent from file */
	while (1) {
		/* Format: <name> <net> [alias, ...] */

		/* get line */
		tmp = fgets(&netent_buff[0], sizeof netent_buff, networks);
		if (tmp == NULL) {
			break;
		}

		/* skip comments */
		if (*tmp == '#') {
			continue;
		}

		/* create netent structure */
		ne = netent_create();
		if (ne == NULL) {
			break;
		}

		/* get main name */
		skip_spaces(tmp);
		name = tmp;
		skip_word(tmp);
		*tmp++ = '\0';
		if (*name == '\0') {
			continue; /* unspecified official name */
		}
		if (netent_set_name(ne, name) != 0) {
			break; /* error occurred */
		}

		/* get net */
		skip_spaces(tmp);
		net_str = tmp;
		skip_word(tmp);
		if (*tmp == '\0') {
			continue; /* invalid format */
		}
		*tmp++ = '\0';
		if (!inet_aton(net_str, &net)) {
			continue; /* bad ip */
		}
		if (netent_set_addr_info(ne, AF_INET) != 0) {
			break; /* error occurred */
		}
		if (netent_set_net(ne, ntohl(net.s_addr)) != 0) {
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
			if (netent_add_alias(ne, name) != 0) {
				break;
			}
		}

		/* save result */
		result = ne;

		/* exit */
		break;
	}

	/* close file (if needed) */
	if (is_disposable) {
		endnetent();
	}

	return result;
}
