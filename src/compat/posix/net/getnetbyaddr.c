/**
 * @file
 * @brief
 *
 * @date 29.03.13
 * @author Ilia Vaprol
 */

#include <stddef.h>
#include <netdb.h>

struct netent * getnetbyaddr(uint32_t net, int type) {
	struct netent *ne;

	setnetent(1);

	while ((ne = getnetent()) != NULL) {
		/* mb it's this entity? */
		if ((ne->n_net == net) && (ne->n_addrtype == type)) {
			break;
		}
		/* nope. try again */
	}

	endnetent();

	return ne;
}
