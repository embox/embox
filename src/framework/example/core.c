/**
 * @file
 *
 * @brief TODO describe it
 *
 * @date 06.07.2011
 * @author Anton Bondarev
 */

#include <types.h>
#include <util/array.h>
#include <errno.h>
#include <framework/mod/ops.h>

#include "types.h"

ARRAY_SPREAD_DEF(const struct example, __example_registry);

static int example_enable(struct mod_info *mod);

const struct mod_ops __example_mod_ops = {
		.enable = example_enable,
};

static int example_enable(struct mod_info *mod) {
	struct example *example = mod->data;

	if (example == NULL || example->exec == NULL) {
		return 0;
	}
	return example->exec(0, NULL);
}

int example_exec(const struct example *example, int argc, char **argv) {
	if (NULL == example) {
		return -EINVAL;
	}

	return example->exec(argc, argv);
}
