/**
 * @file
 * @brief TODO
 *
 * @date 11.03.10
 * @author Eldar Abusalimov
 */

#include <string.h>

#include <embox/unit.h>
#include <framework/mod/api.h>
#include <framework/mod/ops.h>
#include <kernel/printk.h>

static int unit_mod_enable(const struct mod *mod);
static int unit_mod_disable(const struct mod *mod);

const struct mod_ops __unit_mod_ops = {
    .enable = &unit_mod_enable,
    .disable = &unit_mod_disable,
};

static int unit_mod_enable(const struct mod *mod) {
	int ret = 0;
	struct unit *unit = (struct unit *)mod;

	if (NULL == unit->init) {
		return 0;
	}

	printk("\tunit: initializing %s.%s: ", mod_pkg_name(mod), mod_name(mod));
	if (0 == (ret = unit->init())) {
		printk("done\n");
	}
	else {
		printk("error: %s\n", strerror(-ret));
	}

	return ret;
}

static int unit_mod_disable(const struct mod *mod) {
	int ret = 0;
	struct unit *unit = (struct unit *)mod;

	if (NULL == unit->fini) {
		return 0;
	}

	printk("unit: finalizing %s.%s: ", mod_pkg_name(mod), mod_name(mod));
	if (0 == (ret = unit->fini())) {
		printk("done\n");
	}
	else {
		printk("error: %s\n", strerror(-ret));
	}

	return ret;
}
