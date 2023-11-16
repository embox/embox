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
#include <util/log.h>

static int unit_mod_enable(const struct mod *mod);
static int unit_mod_disable(const struct mod *mod);

const struct mod_ops __unit_mod_ops = {
    .enable = &unit_mod_enable,
    .disable = &unit_mod_disable,
};

static int unit_mod_enable(const struct mod *mod) {
	int ret = 0;
	struct unit *unit = (struct unit *)mod;

	if (unit->init == NULL) {
		return 0;
	}

	log_info("unit: initializing %s.%s", mod_pkg_name(mod), mod_name(mod));
	if ((ret = unit->init())) {
		log_error("%s", strerror(-ret));
	}

	return ret;
}

static int unit_mod_disable(const struct mod *mod) {
	int ret = 0;
	struct unit *unit = (struct unit *)mod;

	if (unit->fini == NULL) {
		return 0;
	}

	log_info("unit: finalizing %s.%s", mod_pkg_name(mod), mod_name(mod));
	if ((ret = unit->fini())) {
		log_error("%s", strerror(-ret));
	}

	return ret;
}
