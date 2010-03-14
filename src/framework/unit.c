/**
 * @file
 * @brief TODO
 *
 * @date 11.03.2010
 * @author Eldar Abusalimov
 */

#include <common.h>

#include <embox/unit.h>
#include <embox/mod.h>

static int unit_mod_enable(void *data);
static int unit_mod_disable(void *data);

struct mod_ops __unit_mod_ops = { .enable = &unit_mod_enable,
		.disable = &unit_mod_disable };

static int unit_mod_enable(void *data) {
	int ret = 0;
	struct unit *unit = (struct unit *) data;

	if (NULL == unit->init) {
		return 0;
	}

	TRACE("unit: initializing %s: ", unit->name);
	if (0 == (ret = unit->init())) {
		TRACE("done\n");
	} else {
		TRACE("error\n");
	}

	return ret;
}

static int unit_mod_disable(void *data) {
	int ret = 0;
	struct unit *unit = (struct unit *) data;

	if (NULL == unit->fini) {
		return 0;
	}

	TRACE("unit: finalizing %s: ", unit->name);
	if (0 == (ret = unit->fini())) {
		TRACE("done\n");
	} else {
		TRACE("error\n");
	}

	return ret;
}
