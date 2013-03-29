/**
 * @file
 * @brief
 *
 * @date 29.03.13
 * @author Ilia Vaprol
 */

#include <stddef.h>
#include <string.h>
#include <netdb.h>

struct protoent * getprotobyname(const char *name) {
	struct protoent *pe;
	char **aliases;

	if (name == NULL) {
		return NULL;
	}

	setprotoent(1);

	while ((pe = getprotoent()) != NULL) {
		/* mb it's the official name? */
		if (strcmp(name, pe->p_name) == 0) {
			break;
		}
		/* mb it's one of the alternative name? */
		for (aliases = pe->p_aliases; *aliases != NULL; ++aliases) {
			if (strcmp(name, *aliases) == 0) {
				break;
			}
		}
		if (*aliases != NULL) {
			break;
		}
		/* nope. try again */
	}

	endprotoent();

	return pe;
}
