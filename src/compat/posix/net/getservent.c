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
#include <net/util/servent.h>
#include <netdb.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MODOPS_PATH_TO_SERVICES OPTION_STRING_GET(path_to_services)

#define SERVENT_BUFF_SZ 512

static FILE *services = NULL;
static int is_disposable;

void setservent(int stayopen) {
	services = fopen(MODOPS_PATH_TO_SERVICES, "r");
	is_disposable = !stayopen;
}

void endservent(void) {
	if (services != NULL) {
		fclose(services);
		services = NULL;
	}
}

#define skip_spaces(ptr) while (*ptr && isspace(*ptr)) ptr++
#define skip_word(ptr) while (*ptr && !isspace(*ptr)) ptr++

struct servent * getservent(void) {
	char servent_buff[SERVENT_BUFF_SZ];
	struct servent *se, *result;
	char *tmp, *name, *port_str, *proto;
	int port;

	/* prepare file (if needed) */
	if (services == NULL) {
		setservent(0);
	}

	result = NULL;

	/* read hostent from file */
	while (1) {
		/* Format: <name> <port>/<proto> [alias, ...] */

		/* get line */
		tmp = fgets(&servent_buff[0], sizeof servent_buff, services);
		if (tmp == NULL) {
			break;
		}

		/* skip comments */
		if (*tmp == '#') {
			continue;
		}

		/* create netent structure */
		se = servent_create();
		if (se == NULL) {
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
		if (servent_set_name(se, name) != 0) {
			break; /* error occurred */
		}

		/* get port and proto */
		skip_spaces(tmp);
		port_str = tmp;
		skip_word(tmp);
		if (*tmp == '\0') {
			continue; /* invalid format */
		}
		*tmp++ = '\0';
		proto = strchr(port_str, '/');
		if (proto == NULL) {
			continue; /* invalid format */
		}
		*proto++ = '\0';
		if (sscanf(port_str, "%d", &port) != 1) {
			continue; /* bad number */
		}
		if (servent_set_port(se, port) != 0) {
			break; /* error occurred */
		}
		if (servent_set_proto(se, proto) != 0) {
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
			if (servent_add_alias(se, name) != 0) {
				break;
			}
		}

		/* save result */
		result = se;

		/* exit */
		break;
	}

	/* close file (if needed) */
	if (is_disposable) {
		endservent();
	}

	return result;
}
