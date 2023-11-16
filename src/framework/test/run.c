/**
 * @file
 * @brief Test suite invocation and runtime support.
 *
 * @date 04.12.08
 * @author Anton Bondarev
 *         - Initial implementation
 * @author Eldar Abusalimov
 *         - Separating from tests registry code
 *         - Test fixtures and assertions support
 */

#include <assert.h>
#include <errno.h>
#include <setjmp.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <framework/test/api.h>
#include <framework/test/assert.h>
#include <framework/test/emit.h>
#include <kernel/panic.h>
#include <kernel/printk.h>
#include <util/location.h>

/**
 * Runtime context for a test case.
 */
struct test_run_context {
	jmp_buf before_run;
	struct test_emit_buffer emitting;
};

static struct test_run_context *current;

#define EMIT_BUFFER_SZ 64

static char emit_buffer[EMIT_BUFFER_SZ];

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

	if (!test) {
		return -EINVAL;
	}

	fixture_ops = &test->suite_fixture_ops;

	printk("\ttest: running %s.%s ", test_package(test), test_name(test));

	if ((fx_op = *fixture_ops->p_setup) && (ret = fx_op()) != 0) {
		handle_suite_fixture_failure(test, ret, 1);
		return -EINTR;
	}

	array_spread_nullterm_foreach(test_case, test->test_cases) {
		if ((ret = test_case_run(test_case, &test->case_fixture_ops)) != 0) {
			++failures;
		}
		++total;
		// TODO this looks ugly.
		if (ret == -EINTR) {
			break;
		}
	}

	if ((fx_op = *fixture_ops->p_teardown) && (ret = fx_op()) != 0) {
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

	if ((fx_op = *fixtures->p_setup) && (ret = fx_op()) != 0) {
		handle_case_fixture_failure(test_case, ret, 1);
		return -EINTR;
	}

	failure = test_run(test_case->run);

	if ((fx_op = *fixtures->p_teardown) && (ret = fx_op()) != 0) {
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
	test_emit_buffer_init(&current->emitting, emit_buffer, EMIT_BUFFER_SZ);
	if (!(caught = setjmp(ctx.before_run))) {
		run();
	}
	current = NULL;

	return (const struct __test_assertion_point *)(uintptr_t)caught;
}

struct test_emit_buffer *__test_emit_buffer_current(void) {
	assert(current != NULL);
	return &current->emitting;
}

void __test_assertion_handle(int pass,
    const struct __test_assertion_point *point) {
	if (pass) {
		return;
	}

	assert(point);

	if (current) {
		longjmp(current->before_run, (intptr_t)point);
	}
	else {
		handle_case_result(NULL, point);
		panic("\n\ttest_assert failed inside fixture\n");
	}
}

static void handle_suite_fixture_failure(const struct test_suite *test_suite,
    int code, int setup) {
	printk("\n\tsuite fixture %s failed with code %d: %s\n",
	    setup ? "setup" : "tear down", code, strerror(-code));
}

static void handle_suite_result(const struct test_suite *test_suite,
    int failures, int total) {
	if (failures > 0) {
		printk("\n\ttesting %s (%s) failed\n"
		       "\t\t%d/%d failures\n",
		    test_name(test_suite), test_suite->description, failures, total);
	}
	else if (!failures) {
		printk(" done\n");
	}
}

static void handle_case_fixture_failure(const struct test_case *test_case,
    int code, int setup) {
	printk("\n\tcase fixture %s failed with code %d: %s\n",
	    setup ? "setup" : "tear down", code, strerror(-code));
}

static void handle_case_result(const struct test_case *test_case,
    const struct __test_assertion_point *failure) {
	const struct location __attribute__((unused)) * test_loc;
	const struct location_func __attribute__((unused)) * fail_loc;

	if (!failure) {
		printk(".");
		return;
	}

	fail_loc = &failure->location;
	printk("\n\tfailure at %s : %d, in function %s\n"
	       "\t\t%s\n",
	    fail_loc->at.file, fail_loc->at.line, fail_loc->func, failure->reason);

	if (test_case) {
		test_loc = &test_case->location;
		printk("\t   case at %s : %d\n"
		       "\t\t\"%s\"\n\t",
		    test_loc->file, test_loc->line, test_case->description);
	}
}
