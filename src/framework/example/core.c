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
#include <framework/example/api.h>
#include "types.h"


ARRAY_SPREAD_DEF(const struct example, __example_registry);

int examples_exec(const struct example *example, int argc, char **argv) {
	if (NULL == example) {
		return -EINVAL;
	}

	return example->exec(argc, argv);
}
