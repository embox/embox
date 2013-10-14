/**
 * @file
 * @brief
 *
 * @date 04.07.11
 * @author Dmitry Zubarevich
 * @author Ilia Vaprol
 */

#include <framework/mod/self.h>
#include <framework/net/proto/api.h>
#include <kernel/printk.h>
#include <stddef.h>
#include <string.h>

ARRAY_SPREAD_DEF(volatile const struct net_proto,
		__net_proto_registry);

static int net_proto_mod_enable(struct mod_info *info);
static int net_proto_mod_disable(struct mod_info *info);

const struct mod_ops __net_proto_mod_ops = {
	.enable  = &net_proto_mod_enable,
	.disable = &net_proto_mod_disable
};

static int net_proto_mod_enable(struct mod_info *info) {
	int ret;
	const struct net_proto *nproto;

	ret = 0;
	nproto = (const struct net_proto *)info->data;

	printk("\tNET: initializing protocol %s.%s: ",
			info->mod->package->name, info->mod->name);

	if (nproto->init != NULL) {
		ret = nproto->init();
	}

	if (ret == 0) {
		printk("done\n");
	}
	else {
		printk("error: %s\n", strerror(-ret));
	}

	return ret;
}

static int net_proto_mod_disable(struct mod_info *info) {
	int ret;
	const struct net_proto *nproto;

	ret = 0;
	nproto = (const struct net_proto *)info->data;

	printk("\tNET: finalizing protocol %s.%s: ",
			info->mod->package->name, info->mod->name);

	if (nproto->fini != NULL) {
		ret = nproto->fini();
	}

	if (ret == 0) {
		printk("done\n");
	}
	else {
		printk("error: %s\n", strerror(-ret));
	}

	return ret;
}

const struct net_proto * net_proto_lookup(unsigned short pack,
		unsigned char type) {
	volatile const struct net_proto *nproto;

	net_proto_foreach(nproto) {
		if ((nproto->pack == pack)
				&& (nproto->type == type)) {
			return (const struct net_proto *)nproto;
		}
	}

	return NULL;
}
