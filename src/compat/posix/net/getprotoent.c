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
#include <net/util/protoent.h>
#include <netdb.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define MODOPS_PATH_TO_PROTOCOLS OPTION_STRING_GET(path_to_protocols)

#define PROTOENT_BUFF_SZ 512

static FILE *protocols = NULL;
static int is_disposable;

void setprotoent(int stayopen) {
	protocols = fopen(MODOPS_PATH_TO_PROTOCOLS, "r");
	is_disposable = !stayopen;
}

void endprotoent(void) {
	if (protocols != NULL) {
		fclose(protocols);
		protocols = NULL;
	}
}

#define skip_spaces(ptr) while (*ptr && isspace(*ptr)) ptr++
#define skip_word(ptr) while (*ptr && !isspace(*ptr)) ptr++

struct protoent * getprotoent(void) {
	char protoent_buff[PROTOENT_BUFF_SZ];
	struct protoent *pe, *result;
	char *tmp, *name, *proto_str;
	int proto;

	/* prepare file (if needed) */
	if (protocols == NULL) {
		setprotoent(0);
	}

	result = NULL;

	/* read hostent from file */
	while (1) {
		/* Format: <name> <proto> [alias, ...] */

		/* get line */
		tmp = fgets(&protoent_buff[0], sizeof protoent_buff, protocols);
		if (tmp == NULL) {
			break;
		}

		/* skip comments */
		if (*tmp == '#') {
			continue;
		}

		/* create netent structure */
		pe = protoent_create();
		if (pe == NULL) {
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
		if (protoent_set_name(pe, name) != 0) {
			break; /* error occurred */
		}

		/* get proto */
		skip_spaces(tmp);
		proto_str = tmp;
		skip_word(tmp);
		if (*tmp == '\0') {
			continue; /* invalid format */
		}
		*tmp++ = '\0';
		if (sscanf(proto_str, "%d", &proto) != 1) {
			continue; /* bad number */
		}
		if (protoent_set_proto(pe, proto) != 0) {
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
			if (protoent_add_alias(pe, name) != 0) {
				break;
			}
		}

		/* save result */
		result = pe;

		/* exit */
		break;
	}

	/* close file (if needed) */
	if (is_disposable) {
		endprotoent();
	}

	return result;
}
