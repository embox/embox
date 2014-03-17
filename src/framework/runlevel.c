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

#include <util/array.h>
#include <framework/mod/self.h>
#include <kernel/panic.h>

static runlevel_nr_t init_level = -1;

static const struct mod **runlevel_sentinel[RUNLEVEL_NRS_TOTAL]; ;

static void runlevels_init(void) {
	static char inited;

	/* sentinel init */
	if (!inited) {
		int i;
		const struct mod **modp;

		extern const struct mod *__RUNLEVEL_LAST_MODULE(0),
		       *__RUNLEVEL_LAST_MODULE(1),
		       *__RUNLEVEL_LAST_MODULE(2),
		       *__RUNLEVEL_LAST_MODULE(3);
		const struct mod **last_mod_raw[RUNLEVEL_NRS_TOTAL] = {
			&__RUNLEVEL_LAST_MODULE(0),
			&__RUNLEVEL_LAST_MODULE(1),
			&__RUNLEVEL_LAST_MODULE(2),
			&__RUNLEVEL_LAST_MODULE(3),
		};

		modp = (const struct mod **) __mod_registry;

		for (i = 0; i < RUNLEVEL_NRS_TOTAL; i++) {

			if (*last_mod_raw[i]) {
				while (*modp != *last_mod_raw[i]) {
					modp ++;
				}

				modp++;
			}

			runlevel_sentinel[i] = modp;

		}

		inited = 1;
	}
}

static int runlevel_increase(runlevel_nr_t rl) {
	const struct mod *const volatile*start_mod, *const volatile*end_mod;
	int ret;

	start_mod = rl < 0 ? __mod_registry : runlevel_sentinel[rl];
	end_mod = runlevel_sentinel[rl + 1];

	for (; start_mod != end_mod; start_mod++) {
		if ((ret = mod_enable(*start_mod))) {
			return ret;
		}
	}

	return 0;
}

static int runlevel_decrease(runlevel_nr_t rl) {
	const struct mod *const volatile*start_mod, *const volatile*end_mod;
	int ret;

	start_mod = runlevel_sentinel[rl];
	end_mod = rl > 0 ? runlevel_sentinel[rl - 1] : __mod_registry;

	for (; start_mod != end_mod; start_mod--) {
		if ((ret = mod_disable(*start_mod))) {
			return ret;
		}
	}

	return 0;
}

static int runlevel_change_hook(runlevel_nr_t new_rl, int res) {

	if (!res) {
		init_level = new_rl;
		printk("runlevel: init level is %d\n", init_level);
	} else {
		printk("Failed to get into level %d, current level %d\n",
					new_rl, init_level);
	}

	return res;
}

int runlevel_set(runlevel_nr_t level) {
	int (*runlevel_change)(runlevel_nr_t);
	int d;
	int ret;

	if (!runlevel_nr_valid(level)) {
		return -EINVAL;
	}

	runlevels_init();

	if (init_level < level) {
		d = 1;
		runlevel_change = runlevel_increase;
	} else {
		d = -1;
		runlevel_change = runlevel_decrease;
	}

	while (init_level != level) {

		ret = runlevel_change(init_level);
		if (runlevel_change_hook(init_level + d, ret)) {
			return ret;
		}
	}

	return 0;
}
