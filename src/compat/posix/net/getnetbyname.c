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

struct netent * getnetbyname(const char *name) {
	struct netent *ne;
	char **aliases;

	if (name == NULL) {
		return NULL;
	}

	setnetent(1);

	while ((ne = getnetent()) != NULL) {
		/* mb it's the official name? */
		if (strcmp(name, ne->n_name) == 0) {
			break;
		}
		/* mb it's one of the alternative name? */
		for (aliases = ne->n_aliases; *aliases != NULL; ++aliases) {
			if (strcmp(name, *aliases) == 0) {
				break;
			}
		}
		if (*aliases != NULL) {
			break;
		}
		/* nope. try again */
	}

	endnetent();

	return ne;
}
