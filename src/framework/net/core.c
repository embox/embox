/**
 * @file
 * @brief
 *
 * @date 01.07.11
 * @author Dmitry Zubarevich
 */

#include <framework/net/api.h>

#include <string.h>

#include <framework/mod/ops.h>
#include <framework/mod/api.h>
#include <net/netdevice.h>
#include <util/array.h>

ARRAY_SPREAD_DEF(const struct net, __net_registry);

static int net_mod_enable(struct mod_info *mod_info);

const struct mod_ops __net_mod_ops = {
	.enable  = &net_mod_enable,
};

static int net_mod_enable(struct mod_info *mod_info) {
	int ret;
	const struct net *net = (const struct net *) mod_info->data;
	//const struct mod *mod = mod_info->mod; /* warning fix */
	packet_type_t *netpack = net->netpack;

	if (!netpack || !netpack->init) {
		return 0;
	}

	//TRACE("NET: initializing %s.%s: ", mod->package->name, mod->name); /* warning fix */

	if ((ret = netpack->init()) != 0) {
		TRACE("error: %s\n", strerror(-ret));
	}

	dev_add_pack(netpack);
	TRACE("done\n");

	return 0;
}
