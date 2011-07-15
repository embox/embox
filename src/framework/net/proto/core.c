/**
 * @file
 * @brief
 *
 * @date 04.07.11
 * @author Dmitry Zubarevich
 */

#include <string.h>
#include <framework/mod/ops.h>
#include <framework/net/proto/api.h>
#include <net/protocol.h>

ARRAY_SPREAD_DEF(const struct net_proto, __net_proto_registry);

static int net_proto_mod_enable(struct mod_info *mod);
static int net_proto_mod_disable(struct mod_info *mod);

const struct mod_ops __net_proto_mod_ops = {
	.enable  = &net_proto_mod_enable,
	.disable = &net_proto_mod_disable
};

static int net_proto_mod_enable(struct mod_info *mod) {
	int ret = 0;
	net_proto_t *net_proto_ptr = ((struct net_proto *) mod->data);
	net_protocol_t *net_proto = net_proto_ptr->netproto;

	TRACE("NET: initializing protocol %s.%s: ", mod->mod->package->name, mod->mod->name);

	if (inet_add_protocol(net_proto, net_proto->type) < 0) {
		TRACE("error: %s\n", strerror(-ret));
	} else {
		if (net_proto_ptr->init != NULL) {
			net_proto_ptr->init();
		}
		TRACE("done\n");
	}

	return ret;
}

static int net_proto_mod_disable(struct mod_info *mod) {
	int ret = 0;
	net_protocol_t *net_proto = ((struct net_proto *) mod->data)->netproto;

	TRACE("NET: finalizing protocol %s: ", mod->mod->name);
	if (inet_del_protocol(net_proto, net_proto->type) < 0) {
		TRACE("done\n");
	} else {
		TRACE("error: %s\n", strerror(-ret));
	}

	return ret;
}
