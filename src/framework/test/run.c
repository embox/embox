/**
 * @file
 * @brief Test suite invocation and runtime support.
 *
 * @date Dec 4, 2008
 * @author Anton Bondarev
 *         - Initial implementation
 * @author Eldar Abusalimov
 *         - Separating from tests registry code
 *         - Assertions support
 */

#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <setjmp.h>

#include <util/location.h>

#include <framework/test/api.h>
#include "assert_impl.h"

/**
 * Runtime context for a test case.
 */
struct test_run_context {
	jmp_buf before_run;
};

struct test_run_context *current;

static int test_case_run(const struct test_case *test_case);
static const struct __test_assertion_point *test_run(test_run_t run);

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
		TRACE("\ntest: %d/%d in %s (%s) failed\n", failures, total, name,
				test->description);
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

	TRACE("\ntest case: %s,\n"
			"\t(at %s : %d)\n",
			test_case->description, test_loc->file, test_loc->line);
	TRACE("failed on: %s,\n"
			"\t(at %s : %d, in function %s)\n",
			failure->reason, fail_loc->file, fail_loc->line,
			failure->location.func);

	return -1;
}

static const struct __test_assertion_point *test_run(test_run_t run) {
	struct test_run_context ctx;
	int caught;

	current = &ctx;
	if (!(caught = setjmp(ctx.before_run))) {
		run();
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

