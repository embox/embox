/**
 * @file
 * @brief TODO
 *
 * @date 10.03.10
 * @author Eldar Abusalimov
 */

#include <errno.h>

#include <embox/runlevel.h>
#include <framework/mod/api.h>
#include <framework/mod/self.h>
#include <kernel/panic.h>
#include <lib/libds/array.h>
#include <util/log.h>

static runlevel_nr_t init_level = -1;

ARRAY_SPREAD_DEF(const struct mod *const, __mod_runlevel0);
ARRAY_SPREAD_DEF(const struct mod *const, __mod_runlevel1);
ARRAY_SPREAD_DEF(const struct mod *const, __mod_runlevel2);
ARRAY_SPREAD_DEF(const struct mod *const, __mod_runlevel3);
ARRAY_SPREAD_DEF(const struct mod *const, __mod_runlevel4);

static const struct mod *const volatile
    *mod_runlevels_start[RUNLEVEL_NRS_TOTAL] = {
        __mod_runlevel0,
        __mod_runlevel1,
        __mod_runlevel2,
        __mod_runlevel3,
        __mod_runlevel4,
};

static const struct mod *const volatile *mod_runlevels_end[RUNLEVEL_NRS_TOTAL] = {
    __ARRAY_SPREAD_PRIVATE(__mod_runlevel0, tail),
    __ARRAY_SPREAD_PRIVATE(__mod_runlevel1, tail),
    __ARRAY_SPREAD_PRIVATE(__mod_runlevel2, tail),
    __ARRAY_SPREAD_PRIVATE(__mod_runlevel3, tail),
    __ARRAY_SPREAD_PRIVATE(__mod_runlevel4, tail),
};

static int runlevel_change_hook(runlevel_nr_t new_rl, int res) {
	if (!res) {
		init_level = new_rl;
		log_info("init level is %d", init_level);
	}
	else {
		log_error("Failed to get into level %d, current level %d", new_rl,
		    init_level);
	}

	return res;
}

int runlevel_set(runlevel_nr_t level) {
	const struct mod *const volatile **start_mods, *const volatile **end_mods;
	int (*mod_op)(const struct mod *);
	int d;

	if (!runlevel_nr_valid(level)) {
		return -EINVAL;
	}

	if (init_level < level) {
		start_mods = mod_runlevels_start;
		end_mods = mod_runlevels_end;
		d = 1;
		mod_op = mod_enable;
	}
	else {
		start_mods = mod_runlevels_end;
		end_mods = mod_runlevels_start;
		d = -1;
		mod_op = mod_disable;
	}

	while (init_level != level) {
		const struct mod *const volatile *mod;
		int ret;

		ret = 0;
		for (mod = start_mods[init_level + d]; mod != end_mods[init_level + d];
		     mod += d) {
			if (*mod && (ret = mod_op(*mod))) {
				goto mod_fail;
			}
		}
mod_fail:
		if (runlevel_change_hook(init_level + d, ret)) {
			return ret;
		}
	}

	return 0;
}
