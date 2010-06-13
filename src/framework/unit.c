/**
 * @file
 * @brief TODO
 *
 * @date 11.03.2010
 * @author Eldar Abusalimov
 */

#include <string.h>

#include <mod/ops.h>
#include <mod/framework.h>

// XXX rewrite this shit. -- Eldar
#define __EMBUILD_MOD__
#include <embox/unit.h>

static int unit_mod_enable(struct mod *mod);
static int unit_mod_disable(struct mod *mod);

const struct mod_ops __unit_mod_ops = {
	.enable  = &unit_mod_enable,
	.disable = &unit_mod_disable,
};

static int unit_mod_enable(struct mod *mod) {
	int ret = 0;
	struct unit *unit = (struct unit *) mod_data(mod);

	if (NULL == unit->init) {
		return 0;
	}

	TRACE("unit: initializing %s: ", unit->name);
	if (0 == (ret = unit->init())) {
		TRACE("done\n");
	} else {
		TRACE("error: %s\n", strerror(-ret));
	}

	return ret;
}

static int unit_mod_disable(struct mod *mod) {
	int ret = 0;
	struct unit *unit = (struct unit *) mod_data(mod);

	if (NULL == unit->fini) {
		return 0;
	}

	TRACE("unit: finalizing %s: ", unit->name);
	if (0 == (ret = unit->fini())) {
		TRACE("done\n");
	} else {
		TRACE("error: %s\n", strerror(-ret));
	}

	return ret;
}
