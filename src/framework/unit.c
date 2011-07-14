/**
 * @file
 * @brief TODO
 *
 * @date 11.03.10
 * @author Eldar Abusalimov
 */

#include <string.h>
#include <stdio.h>

#include <framework/mod/ops.h>
#include <framework/mod/api.h>

// XXX rewrite this shit. -- Eldar
#define __EMBUILD_MOD__
#include <embox/unit.h>

static int unit_mod_enable(struct mod_info *mod_info);
static int unit_mod_disable(struct mod_info *mod_info);

const struct mod_ops __unit_mod_ops = {
	.enable  = &unit_mod_enable,
	.disable = &unit_mod_disable,
};

static int unit_mod_enable(struct mod_info *mod_info) {
	int ret = 0;
	struct unit *unit = (struct unit *) mod_info->data;
	const struct mod *mod = mod_info->mod;

	if (NULL == unit->init) {
		return 0;
	}

	TRACE("unit: initializing %s.%s: ", mod->package->name, mod->name);
	if (0 == (ret = unit->init())) {
		TRACE("done\n");
	} else {
		TRACE("error: %s\n", strerror(-ret));
	}

	return ret;
}

static int unit_mod_disable(struct mod_info *mod_info) {
	int ret = 0;
	struct unit *unit = (struct unit *) mod_info->data;
	const struct mod *mod = mod_info->mod;

	if (NULL == unit->fini) {
		return 0;
	}

	TRACE("unit: finalizing %s.%s: ", mod->package->name, mod->name);
	if (0 == (ret = unit->fini())) {
		TRACE("done\n");
	} else {
		TRACE("error: %s\n", strerror(-ret));
	}

	return ret;
}
