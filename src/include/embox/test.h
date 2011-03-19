/**
 * @file
 * @brief Embox testing framework.
 * @details Test is a simple function which examines some parts of hardware or
 * software. Test can be either passed or failed (framework uses return value
 * of test function to determine the test result).
 * Each test resides in a single mod which is usually defined in
 * @code embox.test @endcode or @code <platform>.test @endcode packages.
 *
 * @sa EMBOX_TEST() simple macro used to declare a new test
 *
 * @date Jul 4, 2009
 * @author Anton Bondarev, Alexey Fomin
 *         - Initial implementation
 * @author Eldar Abusalimov
 *         - Rewriting from scratch, adapting for usage with mods framework
 *         - Split external and internal APIs
 *         - Hiding complicated macros implementation
 *         - Introducing test suites
 */

#ifndef EMBOX_TEST_H_
#define EMBOX_TEST_H_

#include <impl/embox/test.h>

#define EMBOX_TEST(run) \
	  __EMBOX_TEST(run)

#define EMBOX_TEST_SUITE(description) \
	  __EMBOX_TEST_SUITE(description)

#define TEST_CASE(description) \
	  __TEST_CASE(description)

#define test_pass() \
	  __test_pass()

#define test_fail(reason) \
	  __test_fail(reason)

#define test_assert(condition) \
	  __test_assert(#condition) /* # prevents condition from expansion. */

#define test_assert_true(value) \
	  __test_assert_true(#value)

#define test_assert_false(value) \
	  __test_assert_false(#value)

#define test_assert_zero(value) \
	  __test_assert_zero(#value)

#define test_assert_not_zero(value) \
	  __test_assert_not_zero(#value)

#define test_assert_null(value) \
	  __test_assert_null(#value)

#define test_assert_not_null(value) \
	  __test_assert_not_null(#value)

#define test_assert_equal(actual, expected) \
	  __test_assert_equal(#actual, #expected)

#define test_assert_not_equal(actual, expected) \
	  __test_assert_not_equal(#actual, #expected)

#endif /* EMBOX_TEST_H_ */
