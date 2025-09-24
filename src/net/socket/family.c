/**
 * @file
 * @brief
 *
 * @date 12.05.13
 * @author Ilia Vaprol
 */

#include <net/family.h>
#include <stddef.h>
#include <lib/libds/array.h>

ARRAY_SPREAD_DEF(const struct net_family, __net_family_registry);

const struct net_family * net_family_lookup(int family) {
	const struct net_family *nfamily;

	net_family_foreach(nfamily) {
		if (nfamily->family == family) {
			return nfamily;
		}
	}

	return NULL;
}

const struct net_family_type * net_family_type_lookup(
		const struct net_family *nfamily, int type) {
	const struct net_family_type *nftype;

	if (nfamily == NULL) {
		return NULL;
	}

	net_family_type_foreach(nftype, nfamily) {
		if (nftype->type == type) {
			return nftype;
		}
	}

	return NULL;
}
