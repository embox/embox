/**
 * @file
 * @brief
 *
 * @date 01.07.11
 * @author Dmitry Zubarevich
 * @author Ilia Vaprol
 */

#include <framework/net/pack/api.h>
#include <stddef.h>
#include <util/array.h>

ARRAY_SPREAD_DEF(const struct net_pack, __net_pack_registry);
ARRAY_SPREAD_DEF(const struct net_pack_out,
		__net_pack_out_registry);

const struct net_pack * net_pack_lookup(unsigned short type) {
	const struct net_pack *npack;

	net_pack_foreach(npack) {
		if (npack->type == type) {
			return npack;
		}
	}

	return NULL;
}

const struct net_pack_out * net_pack_out_lookup(
		int family) {
	const struct net_pack_out *npout;

	net_pack_out_foreach(npout) {
		if (npout->family == family) {
			return npout;
		}
	}

	return NULL;
}
