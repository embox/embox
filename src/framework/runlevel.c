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

extern const struct mod *__RUNLEVEL_LAST_MODULE(0), *__RUNLEVEL_LAST_MODULE(1),
       *__RUNLEVEL_LAST_MODULE(2), *__RUNLEVEL_LAST_MODULE(3);

static const struct mod **last_module_at_runlevel[RUNLEVEL_NRS_TOTAL] __attribute__((used)) = {
	&__RUNLEVEL_LAST_MODULE(0),
	&__RUNLEVEL_LAST_MODULE(1),
	&__RUNLEVEL_LAST_MODULE(2),
	&__RUNLEVEL_LAST_MODULE(3),
};

runlevel_nr_t runlevel_get_entered(void) {
	return init_level;
}

static const struct mod **mod_order_place(const struct mod *mod) {
	const struct mod **modp;

	array_spread_foreach_ptr(modp, __mod_registry) {
		if (*modp == mod) {
			return modp;
		}
	}
	return NULL;
}

static const struct mod *mod_last_by_level(runlevel_nr_t nr) {
	int level = nr;

	while (level >= 0 && !*last_module_at_runlevel[level]) {
		level --;
	}

	return level >= 0 ? *last_module_at_runlevel[level] : NULL;
}

static void runlevel_mod_checkin(const struct mod *mod) {

	/*
	printk("runlevel: init level is %d\n", init_level);
	*/
}

int runlevel_set(runlevel_nr_t level) {
	const struct mod *const volatile*start_mod, *const volatile*end_mod;
	const struct mod *base_mod, *target_mod;
	int ret;

	if (!runlevel_nr_valid(level)) {
		return -EINVAL;
	}

	base_mod = mod_last_by_level(init_level);
	target_mod = mod_last_by_level(level);


	if (level > init_level) {

		start_mod = !base_mod ? __mod_registry : mod_order_place(base_mod);
		end_mod = !target_mod ? NULL : mod_order_place(target_mod);

		for (; start_mod <= end_mod; start_mod++) {

			if ((ret = mod_enable(*start_mod))) {
				return ret;
			}

			runlevel_mod_checkin(*start_mod);
		}
	} else {

		start_mod = !base_mod ? NULL : mod_order_place(base_mod);
		end_mod = !target_mod ? __mod_registry : mod_order_place(target_mod);

		for (; end_mod < start_mod; start_mod--) {

			if ((ret = mod_disable(*start_mod--))) {
				return ret;
			}

			runlevel_mod_checkin(*start_mod);
		}
	}

	return 0;
}

