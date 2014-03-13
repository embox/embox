/**
 * @file
 * @brief TODO
 *
 * @date 10.03.10
 * @author Eldar Abusalimov
 */

#include <errno.h>
#include <kernel/printk.h>

#include <embox/runlevel.h>

#include <framework/mod/api.h>

#include <framework/mod/self.h>
#include <kernel/panic.h>

#define __RUNLEVEL_MOD_NAME(nr) \
	generic__runlevel##nr

#define RUNLEVEL_MOD_DEF(nr) \
	extern const struct mod __MOD(__RUNLEVEL_MOD_NAME(nr)) __attribute__((weak)); \
	__MOD_INFO_DEF(__RUNLEVEL_MOD_NAME(nr), &__runlevel_mod_ops, nr)

#define __RUNLEVEL_MOD_REF(nr) \
	__MOD(__RUNLEVEL_MOD_NAME(nr))

static runlevel_nr_t init_level = -1;

static int rl_mod_enable(const struct mod_info *mod_info) {
	int rl_level = (runlevel_nr_t) mod_info->data;

	init_level = rl_level;
	printk("runlevel: init level is %d\n", init_level);

	return 0;
}

static int rl_mod_disable(const struct mod_info *mod_info) {
	int rl_level = (runlevel_nr_t) mod_info->data;

	init_level = rl_level - 1;
	printk("runlevel: init level is %d\n", init_level);

	return 0;
}

static const struct mod_ops __runlevel_mod_ops = {
	.enable = rl_mod_enable,
	.disable = rl_mod_disable
};

RUNLEVEL_MOD_DEF(0);
RUNLEVEL_MOD_DEF(1);
RUNLEVEL_MOD_DEF(2);
RUNLEVEL_MOD_DEF(3);

static const struct mod *runlevel_mod[RUNLEVEL_NRS_TOTAL] = {
	&__RUNLEVEL_MOD_REF(0),
	&__RUNLEVEL_MOD_REF(1),
	&__RUNLEVEL_MOD_REF(2),
	&__RUNLEVEL_MOD_REF(3),
};

runlevel_nr_t runlevel_get_entered(void) {
	return init_level;
}

int runlevel_set(runlevel_nr_t level) {
	const struct mod *const volatile*start_mod;
	int ret;

	if (!runlevel_nr_valid(level)) {
		return -EINVAL;
	}

	if (init_level < 0) {
		start_mod = __mod_registry;
	} else {
		panic("Can't start from not-negaitve runlevel");
	}

	do {
		ret = mod_enable(*start_mod);
		if (ret) {
			return ret;
		}
		start_mod ++;
	} while (*start_mod != runlevel_mod[level]);

	return 0;
}

