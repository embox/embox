/**
 * @file
 * @brief
 *
 * @date 01.07.11
 * @author Dmitry Zubarevich
 */

#include <string.h>
#include <framework/mod/ops.h>
#include <framework/net/pack/api.h>
#include <net/netdevice.h>

ARRAY_SPREAD_DEF(const struct net_pack, __net_pack_registry);

static int net_pack_mod_enable(struct mod_info *mod);

const struct mod_ops __net_pack_mod_ops = {
	.enable  = &net_pack_mod_enable,
};

static int net_pack_mod_enable(struct mod_info *mod) {
	int ret = 0;
	packet_type_t *net_pack = ((struct net_pack *) mod->data)->netpack;

	if (NULL == net_pack || NULL == net_pack->init) {
		TRACE ("\nWrong packet descriptor\n");
		return 0;
	}
	TRACE("NET: initializing packet %s.%s: ", mod->mod->package->name, mod->mod->name);

	if (0 == (ret = net_pack->init())) {
		dev_add_pack(net_pack);
		TRACE("done\n");
	} else {
		TRACE("error: %s\n", strerror(-ret));
	}

	return ret;
}
