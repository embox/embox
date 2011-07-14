/**
 * @file
 * @brief
 *
 * @date 05.07.11
 * @author Dmitry Zubarevich
 */

#include <string.h>
#include <framework/mod/ops.h>
#include <framework/net/sock/api.h>
#include <net/protocol.h>

ARRAY_SPREAD_DEF(const struct net_sock, __net_sock_registry);

static int net_sock_mod_enable(struct mod_info *mod);
static int net_sock_mod_disable(struct mod_info *mod);

const struct mod_ops __net_sock_mod_ops = {
	.enable  = &net_sock_mod_enable,
	.disable = &net_sock_mod_disable
};

static int net_sock_mod_enable(struct mod_info *mod) {
	int ret = 0;

	net_proto_family_t *net_proto_family = ((net_sock_t *) mod->data)->net_proto_family;
	TRACE("NET: initializing socket %s.%s: ", mod->mod->package->name, mod->mod->name);
	TRACE("done\n");

	if (net_proto_family != NULL) {
		sock_register(net_proto_family);
	}

	return ret;
}

static int net_sock_mod_disable(struct mod_info *mod) {
	int ret = 0;

	net_proto_family_t *net_proto_family = ((net_sock_t *) mod->data)->net_proto_family;

	if (net_proto_family != NULL) {
		sock_unregister(net_proto_family->family);
	}

	return ret;
}
