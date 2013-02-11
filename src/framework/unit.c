/**
 * @file
 * @brief TODO
 *
 * @date 11.03.10
 * @author Eldar Abusalimov
 */

#include <string.h>
#include <kernel/printk.h>

#include <framework/mod/ops.h>
#include <framework/mod/api.h>

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

	if (NULL == unit->init) {
		return 0;
	}

	printk("\tunit: initializing %s.%s: ",
		mod_info->mod->package->name, mod_info->mod->name);
	if (0 == (ret = unit->init())) {
		printk("done\n");
	} else {
		printk("error: %s\n", strerror(-ret));
	}

	return ret;
}

static int unit_mod_disable(struct mod_info *mod_info) {
	int ret = 0;
	struct unit *unit = (struct unit *) mod_info->data;

	if (NULL == unit->fini) {
		return 0;
	}

	printk("unit: finalizing %s.%s: ",
		mod_info->mod->package->name, mod_info->mod->name);
	if (0 == (ret = unit->fini())) {
		printk("done\n");
	} else {
		printk("error: %s\n", strerror(-ret));
	}

	return ret;
}
