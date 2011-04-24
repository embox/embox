/**
 * @file
 * @brief Test suite invocation and runtime support.
 *
 * @date Dec 4, 2008
 * @author Anton Bondarev
 *         - Initial implementation
 * @author Eldar Abusalimov
 *         - Separating from tests registry code
 *         - Test fixtures and assertions support
 */

#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
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

static struct test_run_context *current;

static int test_case_run(const struct test_case *test_case,
		const struct __test_fixture_ops *fixtures);
static const struct __test_assertion_point *test_run(test_case_run_t run);

static void handle_suite_fixture_failure(const struct test_suite *, int code,
		int setup);
static void handle_suite_result(const struct test_suite *, int failures,
		int total);

static void handle_case_fixture_failure(const struct test_case *, int code,
		int setup);
static void handle_case_result(const struct test_case *,
		const struct __test_assertion_point *failure);

int test_suite_run(const struct test_suite *test) {
	const struct test_case *test_case;
	const struct __test_fixture_ops *fixture_ops;
	__test_fixture_op_t fx_op;
	int failures = 0, total = 0;
	int ret;

	if (NULL == test) {
		return -EINVAL;
	}

	fixture_ops = &test->suite_fixture_ops;

	TRACE("test: running %s ", test_name(test));

	if ((fx_op = *fixture_ops->p_setup) != NULL && (ret = fx_op()) != 0) {
		handle_suite_fixture_failure(test, ret, 1);
		return -EINTR;
	}

	array_nullterm_foreach(test_case, test->test_cases) {
		if ((ret = test_case_run(test_case, &test->case_fixture_ops)) != 0) {
			++failures;
		}
		++total;
		// TODO this looks ugly.
		if (ret == -EINTR) {
			failures = ret;
			break;
		}
	}

	if ((fx_op = *fixture_ops->p_teardown) != NULL && (ret = fx_op()) != 0) {
		handle_suite_fixture_failure(test, ret, 0);
		return -EINTR;
	}

	handle_suite_result(test, failures, total);

	return failures;
}

static int test_case_run(const struct test_case *test_case,
		const struct __test_fixture_ops *fixtures) {
	const struct __test_assertion_point *failure;
	__test_fixture_op_t fx_op;
	int ret;

	if ((fx_op = *fixtures->p_setup) != NULL && (ret = fx_op()) != 0) {
		handle_case_fixture_failure(test_case, ret, 1);
		return -EINTR;
	}

	failure = test_run(test_case->run);

	if ((fx_op = *fixtures->p_teardown) != NULL && (ret = fx_op()) != 0) {
		handle_case_fixture_failure(test_case, ret, 0);
		return -EINTR;
	}

	handle_case_result(test_case, failure);

	return !!failure;
}

static const struct __test_assertion_point *test_run(test_case_run_t run) {
	struct test_run_context ctx;
	int caught;

	current = &ctx;
	if (!(caught = setjmp(ctx.before_run))) {
		run();
	}
	current = NULL;

	return (const struct __test_assertion_point *) caught;
}

void __test_assertion_handle(int pass,
		const struct __test_assertion_point *point) {
	if (pass) {
		return;
	}

	assert(point != NULL);
	assert(current != NULL);
	longjmp(current->before_run, (int) point);
}

static void handle_suite_fixture_failure(const struct test_suite *test_suite,
		int code, int setup) {
	TRACE("\n\tsuite fixture %s failed with code %d: %s\n",
			setup ? "setup" : "tear down", code, strerror(-code));
}

static void handle_suite_result(const struct test_suite *test_suite,
		int failures, int total) {
	if (failures > 0) {
		TRACE("\n\ttesting %s (%s) failed\n"
				"\t\t%d/%d failures\n",
				test_name(test_suite), test_suite->description,
				failures, total);
	} else if (!failures) {
		TRACE(" done\n");
	}

}

static void handle_case_fixture_failure(const struct test_case *test_case,
		int code, int setup) {
	TRACE("\n\tcase fixture %s failed with code %d: %s\n",
			setup ? "setup" : "tear down", code, strerror(-code));
}

static void handle_case_result(const struct test_case *test_case,
		const struct __test_assertion_point *failure) {
	const struct location *test_loc;
	const struct location_func *fail_loc;

	if (!failure) {
		TRACE(".");
		return;
	}

	test_loc = &test_case->location;
	fail_loc = &failure->location;

	TRACE("\n\tfailure at %s : %d, in function %s\n"
			"\t\t%s\n",
			fail_loc->at.file, fail_loc->at.line, fail_loc->func,
			failure->reason);
	TRACE("\t   case at %s : %d\n"
			"\t\t\"%s\"\n\t",
			test_loc->file, test_loc->line, test_case->description);
}
