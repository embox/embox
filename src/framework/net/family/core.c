/**
 * @file
 * @brief
 *
 * @date 12.05.13
 * @author Ilia Vaprol
 */

#include <framework/mod/self.h>
#include <framework/net/family/api.h>
#include <kernel/printk.h>
#include <stddef.h>
#include <string.h>

ARRAY_SPREAD_DEF(const struct net_family, __net_family_registry);

static int net_family_mod_enable(struct mod_info *info);
static int net_family_mod_disable(struct mod_info *info);

const struct mod_ops __net_family_mod_ops = {
	.enable   = &net_family_mod_enable,
	.disable  = &net_family_mod_disable
};

static int net_family_mod_enable(struct mod_info *info) {
	int ret;
	const struct net_family *nfamily;

	ret = 0;
	nfamily = (const struct net_family *)info->data;

	printk("\tNET: initializing family %s.%s: ",
			info->mod->package->name, info->mod->name);

	if (nfamily->init != NULL) {
		ret = nfamily->init();
	}

	if (ret == 0) {
		printk("done\n");
	}
	else {
		printk("error: %s\n", strerror(-ret));
	}

	return ret;
}

static int net_family_mod_disable(struct mod_info *info) {
	int ret;
	const struct net_family *nfamily;

	ret = 0;
	nfamily = (const struct net_family *)info->data;

	printk("\tNET: finalizing family %s.%s: ",
			info->mod->package->name, info->mod->name);

	if (nfamily->fini!= NULL) {
		ret = nfamily->fini();
	}

	if (ret == 0) {
		printk("done\n");
	}
	else {
		printk("error: %s\n", strerror(-ret));
	}

	return ret;
}

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
