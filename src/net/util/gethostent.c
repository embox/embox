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
	static struct hostent he;
	static struct in_addr *he_addr[2];
	static struct in_addr he_ip;
	static char *he_aliases[2];
	static char he_buff[HOSTENT_BUFF_SZ];
	char *tmp, *ip_str;
	struct hostent *result;

	/* prepare file (if needed) */
	if (hosts == NULL) {
		open_hosts(MODOPS_PATH_TO_HOSTS, 0);
	}

	result = NULL;

	/* read hostent from file */
	while ((tmp = fgets(&he_buff[0], sizeof he_buff, hosts)) != NULL) {
		/* Format: <addr> <name> [aliase] */

		/* skip comments */
		if (he_buff[0] == '#') {
			continue;
		}

		/* get ipv4 address */
		skip_spaces(tmp); ip_str = tmp; skip_word(tmp);
		if (tmp == '\0') {
			continue; /* invalid format */
		}
		*tmp++ = '\0';
		if (!inet_aton(ip_str, &he_ip)) {
			continue; /* bad ip */
		}
		he.h_addrtype = AF_INET;
		he.h_length = sizeof he_ip;
		he_addr[0] = &he_ip;
		he_addr[1] = NULL;
		he.h_addr_list = (char **)&he_addr;

		/* get main name */
		skip_spaces(tmp);
		he.h_name = tmp;
		skip_word(tmp);
		if (*tmp == '\0') {
			continue; /* unspecified official name */
		}
		*tmp++ = '\0';

		/* get alternate name */
		skip_spaces(tmp);
		if (*tmp != '\0') {
			he_aliases[0] = tmp;
			he_aliases[1] = NULL;
			split_word(tmp);
		}
		else {
			he_aliases[0] = NULL;
		}
		he.h_aliases = (char **)&he_aliases[0];

		/* save result */
		result = &he;

		/* exit */
		break;
	}

	/* close file (if needed) */
	if (is_disposable) {
		close_hosts();
	}

	return result;
}

