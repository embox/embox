/**
 * @file
 * @brief
 *
 * @date 01.07.11
 * @author Dmitry Zubarevich
 */

#include <string.h>

#include <framework/net/api.h>
#include <net/netdevice.h>

ARRAY_SPREAD_DEF(const struct net, __net_registry);

static int net_mod_enable(struct mod *mod);

const struct mod_ops __net_mod_ops = {
	.enable  = &net_mod_enable,
};

static int net_mod_enable(struct mod *mod) {
	int ret = 0;
	struct net *net_mod = (struct net*) mod;

	if (NULL == net_mod->netpack->init) {
		return 0;
	}

	TRACE("NET: initializing %s.%s: ", mod->package->name, mod->name);
//	if (0 == (ret = net_mod->netpack->init())) {
//		dev_add_pack(net_mod->netpack);
//		TRACE("done\n");
//	} else {
//		TRACE("error: %s\n", strerror(-ret));
//	}

	return ret;
}

