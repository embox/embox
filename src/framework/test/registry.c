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

#include <framework/test/api.h>

#include <stddef.h>
#include <string.h>

#include <framework/mod/ops.h>
#include <util/array.h>

static int test_mod_enable(struct mod_info *mod);

const struct mod_ops __test_mod_ops = {
	.enable = &test_mod_enable,
};

ARRAY_SPREAD_DEF(const struct test_suite, __test_registry);

static int test_mod_enable(struct mod_info *mod) {
	return test_suite_run((struct test_suite *) mod->data);
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
