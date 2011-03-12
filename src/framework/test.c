/**
 * @file
 *
 * @brief Embox test framework implementation.
 *
 * @date Dec 4, 2008
 * @author Anton Bondarev
 *         - Initial implementation
 * @author Eldar Abusalimov
 *         - Reworking handler search algorithm
 * @author Alexey Fomin
 *         - Adding level implementation code
 */

#include <test/framework.h>
#include <test/tools.h>

#include <stddef.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <util/array.h>
#include <mod/core.h>

static int test_mod_enable(struct mod *mod);

const struct mod_ops __test_mod_ops = {
	.enable = &test_mod_enable,
};

ARRAY_SPREAD_DEF(const struct test, __test_registry);
ARRAY_SPREAD_DEF(const struct test_failure, __test_failures);

static int test_mod_enable(struct mod *mod) {
	return test_invoke((struct test *) mod_data(mod));
}

int test_invoke(const struct test *test) {
	int result;

	if (NULL == test) {
		return -EINVAL;
	}
	if (NULL == test->run) {
		return -EBADF;
	}

	TRACE("test: running %s: ", test->mod->name);
	if (0 == (result = test->run())) {
		TRACE("passed\n");
	} else {
		const struct test_failure *failure =
				(const struct test_failure *) result;
		if (__test_failures <= failure && failure < __test_failures
				+ ARRAY_SPREAD_SIZE(__test_failures)) {
			/* Valid test_failure object (well, we hope so). */
			TRACE("failed:\n%s (0x%08x), at %s : %d, in function %s\n",
					failure->info->reason, (unsigned int) failure->info->data,
					failure->file, failure->line, failure->func);
		} else {
			/* Plain error code. */
			TRACE("failed\n");
		}
	}

	return (test->private->result = result);
}

const struct test *test_lookup(const char *name) {
	const struct test *test;

	test_foreach(test) {
		if (strcmp(test_name(test), name) == 0) {
			return test;
		}
	}

	return NULL;
}
