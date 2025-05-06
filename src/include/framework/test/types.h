/**
 * @file
 * @brief Type declarations shared between test framework and each test suite.
 *
 * @date 11.06.10
 * @author Eldar Abusalimov
 */

#ifndef FRAMEWORK_TEST_TYPES_H_
#define FRAMEWORK_TEST_TYPES_H_

#include <stdbool.h>

#include <framework/mod/types.h>
#include <util/location.h>

/**
 * Each test case implements this interface.
 */
typedef void (*test_case_run_t)(void);
typedef int (*__test_fixture_op_t)(void);

struct test_suite;
struct test_case;
struct __test_fixture_ops;
struct __test_private;
struct __test_assertion_point;

struct __test_fixture_ops {
	const __test_fixture_op_t *p_setup;    /**< Initialize fixture. */
	const __test_fixture_op_t *p_teardown; /**< Cleanup fixture. */
};

struct test_suite {
	/** @c NULL terminated array of pointers to test cases. */
	const struct test_case *volatile const *test_cases;
	/** One-line human readable description of the whole test suite. */
	const char *description;
	/** If test should be run when module loaded */
	bool autorun;
	/** Internal data managed by framework. */
	struct __test_private *test_priv;
	struct __test_fixture_ops suite_fixture_ops;
	struct __test_fixture_ops case_fixture_ops;
};

struct test_case {
	/** The test itself. */
	test_case_run_t run;
	/** One-line human readable description of the test case. */
	const char *description;
	/** Location of the test case function definition within the file. */
	struct location location;
};

struct __test_assertion_point {
	struct location_func location;
	const char *reason;
};

struct __test_private {
	int result;
};

struct test_mod {
	struct mod mod;
	struct test_suite suite;
};

#endif /* FRAMEWORK_TEST_TYPES_H_ */
