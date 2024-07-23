/**
 * @file
 * @brief TODO
 *
 * @date 10.03.10
 * @author Eldar Abusalimov
 */

#include <errno.h>

#include <framework/mod/api.h>
#include <framework/mod/runlevel.h>
#include <framework/mod/self.h>
#include <kernel/panic.h>
#include <lib/libds/array.h>
#include <util/log.h>

ARRAY_SPREAD_DEF(const struct mod *const, __mod_runlevel0);
ARRAY_SPREAD_DEF(const struct mod *const, __mod_runlevel1);
ARRAY_SPREAD_DEF(const struct mod *const, __mod_runlevel2);
ARRAY_SPREAD_DEF(const struct mod *const, __mod_runlevel3);
ARRAY_SPREAD_DEF(const struct mod *const, __mod_runlevel4);

static runlevel_nr_t current_runlevel = -1;

const struct mod *const volatile *mod_runlevels_start[RUNLEVEL_NRS_TOTAL] = {
    __mod_runlevel0,
    __mod_runlevel1,
    __mod_runlevel2,
    __mod_runlevel3,
    __mod_runlevel4,
};

const struct mod *const volatile *mod_runlevels_end[RUNLEVEL_NRS_TOTAL] = {
    __ARRAY_SPREAD_PRIVATE(__mod_runlevel0, tail),
    __ARRAY_SPREAD_PRIVATE(__mod_runlevel1, tail),
    __ARRAY_SPREAD_PRIVATE(__mod_runlevel2, tail),
    __ARRAY_SPREAD_PRIVATE(__mod_runlevel3, tail),
    __ARRAY_SPREAD_PRIVATE(__mod_runlevel4, tail),
};

static int runlevel_change_hook(runlevel_nr_t new_rl, int res) {
	if (!res) {
		current_runlevel = new_rl;
		log_info("runlevel is %d", current_runlevel);
	}
	else {
		log_error("Failed to get into level %d, current level %d", new_rl,
		    current_runlevel);
	}

	return res;
}

int runlevel_set(runlevel_nr_t level) {
	const struct mod *mod;
	int err;
	int i;

	if (!runlevel_nr_valid(level)) {
		return -EINVAL;
	}

	err = 0;

	for (i = current_runlevel + 1; i <= level; i++) {
		runlevel_mod_foreach(mod, i) {
			if ((err = mod_enable(mod))) {
				break;
			}
		}
		if (runlevel_change_hook(i, err)) {
			break;
		}
	}

	for (i = current_runlevel; i > level;) {
		runlevel_mod_foreach_reverse(mod, i) {
			if ((err = mod_disable(mod))) {
				break;
			}
		}
		if (runlevel_change_hook(--i, err)) {
			break;
		}
	}

	return err;
}
