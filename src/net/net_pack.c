/**
 * @file
 * @brief
 *
 * @date 01.07.11
 * @author Dmitry Zubarevich
 * @author Ilia Vaprol
 */

#include <stddef.h>
#include <lib/libds/array.h>
#include "embox/net/pack.h"

ARRAY_SPREAD_DEF(const struct net_pack, __net_pack_registry);

const struct net_pack * net_pack_lookup(unsigned short type) {
	const struct net_pack *npack;

	net_pack_foreach(npack) {
		if (npack->type == type) {
			return npack;
		}
	}

	return NULL;
}
