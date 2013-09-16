/**
 * @file
 * @brief Command registry and invocation code.
 *
 * @date 01.03.11
 * @author Eldar Abusalimov
 */

#include <framework/cmd/api.h>

#include <ctype.h>
#include <stddef.h>
#include <errno.h>
#include <string.h>

#include <kernel/printk.h>
#include <framework/mod/ops.h>
#include <util/array.h>

#define APP_DATA_RESERVE_OFFSET ({ \
		extern char __app_data_start; \
		extern char __app_reserve_start; \
		&__app_reserve_start - &__app_data_start; \
	})

ARRAY_SPREAD_DEF(const struct cmd, __cmd_registry);

static int cmd_mod_enable(struct mod_info *mod);

const struct mod_ops __cmd_mod_ops = {
	.enable = &cmd_mod_enable,
};

static int cmd_mod_enable(struct mod_info *mod_info) {
	const struct mod_extra *extra = mod_info->mod->extra;

	printk("\tcmd: loading %s.%s: data=0x%08lx, data_sz=%zu, reserve=0x%08lx\n",
		mod_info->mod->package->name, mod_info->mod->name,
		(unsigned long) extra->data, extra->data_sz,
		(unsigned long) extra->data + APP_DATA_RESERVE_OFFSET);

	memcpy(extra->data + APP_DATA_RESERVE_OFFSET, extra->data, extra->data_sz);

	return 0;
}

int cmd_exec(const struct cmd *cmd, int argc, char **argv) {
	const struct mod *mod;
	const struct mod_extra *extra;

	if (!cmd)
		return -EINVAL;

	mod = cmd->mod;
	if (!mod_is_running(mod))
		return -ENOENT;

	extra = mod->extra;
	memcpy(extra->data, extra->data + APP_DATA_RESERVE_OFFSET, extra->data_sz);
	memset(extra->bss, 0, extra->bss_sz);

	return cmd->exec(argc, argv);
}

const struct cmd *cmd_lookup(const char *name) {
	const struct cmd *cmd = NULL;

	cmd_foreach(cmd) {
		if (strcmp(cmd_name(cmd), name) == 0) {
			return cmd;
		}
	}

	return NULL;
}

