/**
 * @file
 * @brief Assertions for use inside user test cases.
 *
 * @date 11.06.10
 * @author Eldar Abusalimov
 */

#ifndef FRAMEWORK_TEST_ASSERT_H_
#define FRAMEWORK_TEST_ASSERT_H_

#include __impl_x(framework/test/assert_impl.h)

/**
 * Fails a test with the given @a reason.
 *
 * This causes the following message to be logged:
@verbatim
failed: test_fail(<reason>)
	at <file>:<line>
@endverbatim
 *
 * @param reason (optional)
 *   String literal containing the description of the test failure.
 * @note
 *   This macro function never returns.
 */
#define test_fail(reason) \
	  __test_fail(reason)

/**
@verbatim
failed: test_assert(<#condition>)
	at <file>:<line>
@endverbatim
 */
#define test_assert(condition) \
	  __test_assert(condition, #condition)

/**
@verbatim
failed: test_assert_true(<#value>)
	at <file>:<line>
@endverbatim
 */
#define test_assert_true(value) \
	  __test_assert_true(value, #value)

/**
@verbatim
failed: test_assert_false(<#value>)
	at <file>:<line>
@endverbatim
 */
#define test_assert_false(value) \
	  __test_assert_false(value, #value)

/**
@verbatim
failed: test_assert_zero(<#value>): <value>
	at <file>:<line>
@endverbatim
 */
#define test_assert_zero(value) \
	  __test_assert_zero(value, #value)

/**
@verbatim
failed: test_assert_not_zero(<#value>)
	at <file>:<line>
@endverbatim
 */
#define test_assert_not_zero(value) \
	  __test_assert_not_zero(value, #value)

/**
@verbatim
failed: test_assert_null(<#value>): <value>
	at <file>:<line>
@endverbatim
 */
#define test_assert_null(value) \
	  __test_assert_null(value, #value)

/**
@verbatim
failed: test_assert_not_null(<#value>)
	at <file>:<line>
@endverbatim
 */
#define test_assert_not_null(value) \
	  __test_assert_not_null(value, #value)

/**
@verbatim
failed: test_assert_equal(<#actual>, <#expected>): <actual>, expected <expected>
	at <file>:<line>
@endverbatim
 */
#define test_assert_equal(actual, expected) \
	  __test_assert_equal(actual, expected, #actual, #expected)

/**
@verbatim
failed: test_assert_not_equal(<#actual>, <#expected>): <actual>
	at <file>:<line>
@endverbatim
 */
#define test_assert_not_equal(actual, expected) \
	  __test_assert_not_equal(actual, expected, #actual, #expected)

#define test_assert_str_equal(actual, expected) \
	  __test_assert_str_equal(actual, expected, #actual, #expected)

#define test_assert_str_not_equal(actual, expected) \
	  __test_assert_str_not_equal(actual, expected, #actual, #expected)

#define test_assert_strn_equal(actual, expected, n) \
	  __test_assert_strn_equal(actual, expected, n, #actual, #expected)

#define test_assert_strn_not_equal(actual, expected, n) \
	  __test_assert_strn_not_equal(actual, expected, n, #actual, #expected)

#define test_assert_mem_equal(actual, expected, n) \
	  __test_assert_mem_equal(actual, expected, n, #actual, #expected)

#define test_assert_mem_not_equal(actual, expected, n) \
	  __test_assert_mem_not_equal(actual, expected, n, #actual, #expected)

#endif /* FRAMEWORK_TEST_ASSERT_H_ */
