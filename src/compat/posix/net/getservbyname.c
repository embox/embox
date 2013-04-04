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

struct servent * getservbyname(const char *name,
		const char *proto) {
	struct servent *se;
	char **aliases;

	if ((name == NULL) || (proto == NULL)) {
		return NULL;
	}

	setservent(1);

	while ((se = getservent()) != NULL) {
		/* same protocol? */
		if (strcmp(proto, se->s_proto) != 0) {
			continue; /* not same. try again */
		}
		/* mb it's the official name? */
		if (strcmp(name, se->s_name) == 0) {
			break;
		}
		/* mb it's one of the alternative name? */
		for (aliases = se->s_aliases; *aliases != NULL; ++aliases) {
			if (strcmp(name, *aliases) == 0) {
				break;
			}
		}
		if (*aliases != NULL) {
			break;
		}
		/* nope. try again */
	}

	endservent();

	return se;
}
