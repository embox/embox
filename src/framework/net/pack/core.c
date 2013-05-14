/**
 * @file
 * @brief
 *
 * @date 01.07.11
 * @author Dmitry Zubarevich
 * @author Ilia Vaprol
 */

#include <framework/mod/self.h>
#include <framework/net/pack/api.h>
#include <kernel/printk.h>
#include <stddef.h>
#include <string.h>

ARRAY_SPREAD_DEF(const struct net_pack, __net_pack_registry);

static int net_pack_mod_enable(struct mod_info *info);
static int net_pack_mod_disable(struct mod_info *info);

const struct mod_ops __net_pack_mod_ops = {
	.enable   = &net_pack_mod_enable,
	.disable  = &net_pack_mod_disable
};

static int net_pack_mod_enable(struct mod_info *info) {
	int ret;
	const struct net_pack *npack;

	ret = 0;
	npack = (struct net_pack *)info->data;

	printk("\tNET: initializing packet %s.%s: ",
			info->mod->package->name, info->mod->name);

	if (npack->init != NULL) {
		ret = npack->init();
	}

	if (ret == 0) {
		printk("done\n");
	}
	else {
		printk("error: %s\n", strerror(-ret));
	}

	return ret;
}

static int net_pack_mod_disable(struct mod_info *info) {
	int ret;
	const struct net_pack *npack;

	ret = 0;
	npack = (struct net_pack *)info->data;

	printk("\tNET: finalizing packet %s.%s: ",
			info->mod->package->name, info->mod->name);

	if (npack->fini != NULL) {
		ret = npack->fini();
	}

	if (ret == 0) {
		printk("done\n");
	}
	else {
		printk("error: %s\n", strerror(-ret));
	}

	return ret;
}

const struct net_pack * net_pack_lookup(unsigned short type) {
	const struct net_pack *npack;

	net_pack_foreach(npack) {
		if (npack->type == type) {
			return npack;
		}
	}

	return NULL;
}
