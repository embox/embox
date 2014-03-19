/**
 * @file
 *
 * @brief TODO describe it
 *
 * @date 06.07.2011
 * @author Anton Bondarev
 */

#include <util/array.h>
#include <errno.h>
#include <string.h>
#include <framework/mod/ops.h>

#include <framework/example/api.h>

ARRAY_SPREAD_DEF(const struct example *, __example_registry);

static int example_enable(const struct mod *mod);

const struct mod_ops __example_mod_ops = {
		.enable = example_enable,
};

static int example_enable(const struct mod *mod) {
	/* FIXME
	 * This executed only with optimization's flag (-O1 or more)
	 */
#if 0
	/* Example may wait some actions from outside */
	struct example *example = mod->data;

	if (example == NULL || example->exec == NULL) {
		return 0;
	}
	return example->exec(0, NULL);
#endif
	return 0;
}

static int parse_name(const char *full_name, char **path, char **name) {
	char *iter = (char *)full_name;
	char *splitter = NULL;

	while('\0' != *iter) {
		if('.' == *iter) {
			splitter = iter;
		}
		iter ++;
	}
	if(NULL != splitter) {
		*splitter = '\0';
		*name = splitter + 1;
		*path = (char *)full_name;
		return 0;
	}
	*name = (char *)full_name;
	*path = "";

	return 0;
}

static const char *strtostr(const char *str) {
	return (str != NULL) ? str : "";
}

const struct example *example_lookup(const char *full_name) {
	const struct example *example = NULL;
	char *path;
	char *name;

	parse_name(full_name, &path, &name);
	example_foreach(example) {
		if ((strcmp(example_name(example), name) == 0) &&
				(strcmp(strtostr(example_path(example)), path) == 0)) {
			if (*path != '\0') {
				*(name - 1) = '.';
			}
			return example;
		}
	}

	if (*path != '\0') {
		*(name - 1) = '.';
	}

	return NULL;
}

int example_exec(const struct example *example, int argc, char **argv) {
	if (NULL == example) {
		return -EINVAL;
	}

	return example->exec(argc, argv);
}
