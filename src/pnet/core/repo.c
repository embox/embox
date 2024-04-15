/**
 * @file
 *
 * @brief
 *
 * @date 01.11.2011
 * @author Anton Bondarev
 */

#include <lib/libds/array.h>
#include <string.h>

#include <pnet/core/core.h>
#include <pnet/core/repo.h>
#include <pnet/core/node.h>

ARRAY_SPREAD_DEF(const struct pnet_module, __pnet_mod_repo);

struct net_node *pnet_get_module(const char *name) {
	const struct pnet_module *mod;
	array_spread_foreach_ptr(mod, __pnet_mod_repo) {
		if (0 == strcmp(mod->name, name)) {
			if (mod->node != NULL) {
				return mod->node;
			}
			return pnet_node_alloc(0, mod->proto);
		}
	}
	return NULL;
}
