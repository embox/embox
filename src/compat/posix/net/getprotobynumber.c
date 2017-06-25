/**
 * @file
 * @brief
 *
 * @date 29.03.13
 * @author Ilia Vaprol
 */

#include <stddef.h>
#include <netdb.h>

struct protoent * getprotobynumber(int proto) {
	struct protoent *pe;

	setprotoent(1);

	while ((pe = getprotoent()) != NULL) {
		/* same proto? */
		if (pe->p_proto == proto) {
			break;
		}
		/* nope. try again */
	}

	endprotoent();

	return pe;
}
