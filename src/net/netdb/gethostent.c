/**
 * @file
 *
 * @date 20.08.12
 * @author Ilia Vaprol
 */

#include <net/netdb.h>
#include <net/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <ctype.h>
#include <errno.h>

#include <framework/mod/options.h>
#define MODOPS_PATH_TO_HOSTS OPTION_STRING_GET(path_to_hosts)

#define HOSTENT_BUFF_SZ 512

static FILE *hosts = NULL;
static int is_disposable;

static int open_hosts(const char *filename, int stayopen) {
	hosts = fopen(filename, "r");
	is_disposable = !stayopen;
	return !!hosts;
}

static void close_hosts(void) {
	if (hosts == NULL) {
		return;
	}

	if (fclose(hosts) < 0) {
		LOG_ERROR("can't close file");
	}
	hosts = NULL;
}

void sethostent(int stayopen) {
	open_hosts(MODOPS_PATH_TO_HOSTS, stayopen);
}

void endhostent(void) {
	close_hosts();
}

#define skip_spaces(ptr) while (*ptr && isspace(*ptr)) ptr++
#define skip_word(ptr) while (*ptr && !isspace(*ptr)) ptr++
static void split_word(char *ptr) { skip_word(ptr); *ptr = '\0'; }

struct hostent * gethostent(void) {
	char hostent_buff[HOSTENT_BUFF_SZ];
	struct hostent *he, *result;
	char *tmp, *ip_str, *name;
	struct in_addr ip_addr;

	/* prepare file (if needed) */
	if (hosts == NULL) {
		open_hosts(MODOPS_PATH_TO_HOSTS, 0);
	}

	result = NULL;

	/* read hostent from file */
	while ((tmp = fgets(&hostent_buff[0], sizeof hostent_buff, hosts)) != NULL) {
		/* Format: <addr> <name> [aliase] */

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
		skip_spaces(tmp); ip_str = tmp; skip_word(tmp);
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
		skip_spaces(tmp);
		name = tmp;
		split_word(tmp);
		if (*name != '\0') {
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
		close_hosts();
	}

	return result;
}

