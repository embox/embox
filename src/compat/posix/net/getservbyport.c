/**
 * @file
 * @brief
 *
 * @date 29.03.13
 * @author Ilia Vaprol
 */

#include <arpa/inet.h>
#include <stddef.h>
#include <string.h>
#include <netdb.h>

struct servent * getservbyport(int port,
		const char *proto) {
	struct servent *se;

	if (proto == NULL) {
		return NULL;
	}

	setservent(1);
	port = htons((unsigned short)port);

	while ((se = getservent()) != NULL) {
		/* same protocol? */
		if (strcmp(proto, se->s_proto) != 0) {
			continue; /* not same. try again */
		}
		/* same port? */
		if (se->s_port == port) {
			break;
		}
		/* nope. try again */
	}

	endservent();

	return se;
}
