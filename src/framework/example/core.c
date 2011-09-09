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
#include <string.h>
#include <framework/mod/ops.h>


#include <framework/example/api.h>


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
	*path = NULL;

	return 0;
}

const struct example *example_lookup(const char *full_name) {
	const struct example *example = NULL;
	char *path;
	char *name;

	parse_name(full_name, &path, &name);
	example_foreach(example) {
		if ((strcmp(example_name(example), name) == 0) &&
				(strcmp(example_path(example), path) == 0)) {
			return example;
		}
	}

	return NULL;
}

int example_exec(const struct example *example, int argc, char **argv) {
	if (NULL == example) {
		return -EINVAL;
	}

	return example->exec(argc, argv);
}
