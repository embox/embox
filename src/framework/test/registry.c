/**
 * @file
 * @brief Registry of available tests.
 *
 * @date Dec 4, 2008
 * @author Anton Bondarev
 *         - Initial implementation
 * @author Alexey Fomin
 *         - Adding level implementation code
 * @author Eldar Abusalimov
 *         - Reworking handler search algorithm
 *         - Distinguishing test suites and test cases
 */

#include <framework/test/api.h>

#include <stddef.h>
#include <string.h>

#include <util/array.h>
#include <framework/mod/api.h>

static int test_mod_enable(struct mod *mod);

const struct mod_ops __test_mod_ops = {
	.enable = &test_mod_enable,
};

ARRAY_SPREAD_DEF(const struct test_suite, __test_registry);

static int test_mod_enable(struct mod *mod) {
	return test_suite_run((struct test_suite *) mod_data(mod));
}

const struct test_suite *test_lookup(const char *name) {
	const struct test_suite *test;

	test_foreach(test) {
		if (strcmp(test_name(test), name) == 0) {
			return test;
		}
	}

	return NULL;
}

