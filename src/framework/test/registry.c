/**
 * @file
 * @brief Registry of available tests.
 *
 * @date 04.12.08
 * @author Anton Bondarev
 *         - Initial implementation
 * @author Alexey Fomin
 *         - Adding level implementation code
 * @author Eldar Abusalimov
 *         - Reworking handler search algorithm
 *         - Distinguishing test suites and test cases
 */

#include <stddef.h>
#include <string.h>

#include <framework/mod/ops.h>
#include <framework/test/api.h>
#include <lib/libds/array.h>

static int test_mod_enable(const struct mod *mod);

const struct mod_ops __test_mod_ops = {
    .enable = &test_mod_enable,
};

ARRAY_SPREAD_DEF(const struct test_suite *const, __test_registry);

static int test_mod_enable(const struct mod *mod) {
	struct test_mod *test_mod;

	test_mod = (struct test_mod *)mod;

	if (!test_mod->suite.autorun) {
		return 0;
	}

	return test_suite_run(&test_mod->suite);
}

const struct test_suite *test_lookup(const char *name) {
	const struct test_suite *test = NULL;

	test_foreach(test) {
		if (strcmp(test_name(test), name) == 0) {
			return test;
		}
	}

	return NULL;
}
