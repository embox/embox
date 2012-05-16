/**
 * @file
 * @brief TODO
 *
 * @date 10.03.10
 * @author Eldar Abusalimov
 */

#include <errno.h>
#include <kernel/prom_printf.h>

#include <embox/runlevel.h>

#include <framework/mod/api.h>

#include <framework/mod/self.h>

#define __RUNLEVEL_MOD(nr) \
	generic__runlevel##nr

#define RUNLEVEL_MOD_DEF(nr) \
	extern const struct mod __MOD(__RUNLEVEL_MOD(nr)); \
	__MOD_INFO_DEF(__RUNLEVEL_MOD(nr), &__runlevel_mod_ops, nr)

#define RUNLEVEL_DEF(nr) \
	RUNLEVEL_MOD_DEF(nr)

#define RUNLEVEL(nr) { \
		.mod = &__MOD(__RUNLEVEL_MOD(nr)), \
	}

static int rl_mod_enable(struct mod_info *mod_info);
static int rl_mod_disable(struct mod_info *mod_info);

struct mod_ops __runlevel_mod_ops = {
	.enable = rl_mod_enable,
	.disable = rl_mod_disable
};

RUNLEVEL_DEF(0);
RUNLEVEL_DEF(1);
RUNLEVEL_DEF(2);
RUNLEVEL_DEF(3);

struct runlevel {
	const struct mod *mod;
};

static const struct runlevel runlevels[RUNLEVEL_NRS_TOTAL] = {
	RUNLEVEL(0),
	RUNLEVEL(1),
	RUNLEVEL(2),
	RUNLEVEL(3),
};

static runlevel_nr_t init_level = -1, fini_level = -1;

static int rl_mod_enable(struct mod_info *mod_info) {
	int ret;
	int level = (runlevel_nr_t) mod_info->data;
	const struct mod *rl_mod = runlevels[level].mod;

	if (runlevel_nr_valid(level - 1)) {
		const struct mod *mod;
		array_nullterm_foreach(mod, rl_mod->contents) {
			if (0 != (ret = mod_enable(mod))) {
				return ret;
			}
		}

		if (0 != (ret = mod_enable(runlevels[level - 1].mod))) {
			return ret;
		}
	}

	init_level = level;
	prom_printf("runlevel: init level is %d\n", init_level);

	return 0;
}

static int rl_mod_disable(struct mod_info *mod_info) {
	int ret;
	int level = (runlevel_nr_t) mod_info->data;

	if (runlevel_nr_valid(level + 1) &&
		0 != (ret = mod_disable(runlevels[level + 1].mod))) {
		return ret;
	}
	init_level = level - 1;
	prom_printf("runlevel: init level is %d\n", init_level);

	return 0;
}

runlevel_nr_t runlevel_get_entered(void) {
	return init_level;
}

runlevel_nr_t runlevel_get_leaved(void) {
	return fini_level;
}

int runlevel_set(runlevel_nr_t level) {
	int ret;

	if (!runlevel_nr_valid(level)) {
		return -EINVAL;
	}

	if (0 != (ret = runlevel_enter(level)) || (runlevel_nr_valid(level + 1)
			&& 0 != (ret = runlevel_leave(level + 1)))) {
		return ret;
	}
	return 0;
}

int runlevel_enter(runlevel_nr_t level) {
	int ret = 0;

	if (!runlevel_nr_valid(level)) {
		return -EINVAL;
	}

	while (init_level < level) {
		if (0 != (ret = mod_enable(runlevels[init_level + 1].mod))) {
			break;
		}
	}

	return ret;
}

int runlevel_leave(runlevel_nr_t level) {
	int ret = 0;

	if (!runlevel_nr_valid(level)) {
		return -EINVAL;
	}

	while (fini_level > level) {
		if (0 != (ret = mod_disable(runlevels[fini_level - 1].mod))) {
			break;
		}
	}

	return ret;
}
