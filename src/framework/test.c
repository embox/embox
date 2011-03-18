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

int test_case_run(const struct test_case *test_case) {
	int error;

	if (!(error = test_case->run())) {
		TRACE(".");

	} else {
		const struct test_failure *failure =
				(const struct test_failure *) error;
		const struct test_case_location *location = &test_case->location;

		TRACE("\ntest case failed: %s,\n\t(defined at %s : %d)\n",
				test_case->description, location->file, location->line);
		if (__test_failures <= failure && failure < __test_failures
				+ ARRAY_SPREAD_SIZE(__test_failures)) {
			/* Valid test_failure object (well, we hope so). */
			TRACE("reason:\n%s (0x%08x),\n\t(at %s : %d, in function %s)\n",
					failure->info->reason, (unsigned int) failure->info->data,
					failure->file, failure->line, failure->func);
		} else {
			/* Plain error code. */
			TRACE("error code: %d\n", error);
		}

	}

	return error;
}

int test_invoke(const struct test *test) {
	int error = 0;
	const struct test_case *test_case;
	const char *name;

	if (NULL == test) {
		return -EINVAL;
	}

	name = test->mod->name;

	TRACE("test: running %s: ", name);
	array_nullterm_foreach(test_case, test->test_cases) {
		error |= test_case_run(test_case);
	}

	if (!error) {
		TRACE(" done\n");
	} else {
		TRACE("\ntest: %s failed\n", name);
	}

	return (test->private->result = error);
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
