/**
 * @file
 *
 * @brief Embox test framework implementation.
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

#include <test/framework.h>
#include <test/tools.h>

#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>

#include <util/array.h>
#include <util/location.h>
#include <mod/core.h>

/**
 * Runtime context for a test case.
 */
struct test_run_context {
	jmp_buf before_run;
};

struct test_run_context *current;

static int test_mod_enable(struct mod *mod);

const struct mod_ops __test_mod_ops = {
	.enable = &test_mod_enable,
};

ARRAY_SPREAD_DEF(const struct test_suite, __test_registry);
ARRAY_SPREAD_DEF(const struct test_failure, __test_failures);

static int test_case_run(const struct test_case *test_case);
static const struct __test_assertion_point *test_run(test_run_t run);

static int test_mod_enable(struct mod *mod) {
	return test_suite_run((struct test_suite *) mod_data(mod));
}

int test_suite_run(const struct test_suite *test) {
	int total = 0, failures = 0;
	const struct test_case *test_case;
	const char *name;

	if (NULL == test) {
		return -EINVAL;
	}

	name = test->mod->name;

	TRACE("test: running %s: ", name);
	array_nullterm_foreach(test_case, test->test_cases) {
		if(test_case_run(test_case) != 0) {
			++failures;
		}
		++total;
	}

	if (failures) {
		TRACE("\ntest: %d/%d in %s failed\n", failures, total, name);
	} else {
		TRACE(" done\n");
	}

	return (test->private->result = failures);
}

static int test_case_run(const struct test_case *test_case) {
	const struct __test_assertion_point *failure;
	const struct location *test_loc, *fail_loc;

	if (!(failure = test_run(test_case->run))) {
		TRACE(".");
		return 0;
	}

	test_loc = &test_case->location;
	fail_loc = &failure->location.input;

	TRACE("\ntest case failed: %s,\n\t(defined at %s : %d)\n",
			test_case->description, test_loc->file, test_loc->line);
	TRACE("reason:\n%s,\n\t(at %s : %d, in function %s)\n",
			failure->reason, fail_loc->file, fail_loc->line,
			failure->location.func);

	return -1;
}

static const struct __test_assertion_point *test_run(test_run_t run) {
	struct test_run_context ctx;
	int caught;

	current = &ctx;
	if (!(caught = setjmp(ctx.before_run))) {
		if (run()) {
			TRACE("XXX deprecated Embox test failure");
		}
	}
	current = NULL;

	return (const struct __test_assertion_point *) caught;
}

void __test_assertion_handle0(int pass,
		const struct __test_assertion_point *point) {
	if (pass) {
		return;
	}

	assert(point != NULL);
	assert(current != NULL);
	longjmp(current->before_run, (int) point);
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

