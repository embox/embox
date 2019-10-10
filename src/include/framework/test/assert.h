/**
 * @file
 * @brief Assertions for use inside user test cases.
 *
 * @date 11.06.10
 * @author Eldar Abusalimov
 */

#ifndef FRAMEWORK_TEST_ASSERT_H_
#define FRAMEWORK_TEST_ASSERT_H_

#include <string.h>
#include <sys/cdefs.h>

#include <stdint.h>

#include <util/location.h>

#include <framework/test/types.h>
#include <framework/test/emit.h>

EXTERN_C void __test_assertion_handle(int pass,
		const struct __test_assertion_point *point);

#define __test_assertion_point_ref(_reason) \
	({                                                                        \
		/* Statically allocate and define. Location and reason message are    \
		 * known at compile time and nobody cares about .rodata section. */   \
		static const struct __test_assertion_point __test_assertion_point = { \
			.location = LOCATION_FUNC_INIT, \
			.reason = _reason,              \
		};                                  \
		&__test_assertion_point;            \
	})

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
	__test_assertion_handle(0, \
			__test_assertion_point_ref("test_fail(\"" reason "\")"))
/**
@verbatim
failed: test_assert(<#condition>)
	at <file>:<line>
@endverbatim
 */
#define test_assert(condition) \
	__test_assertion_handle((intptr_t) (condition), \
			__test_assertion_point_ref("test_assert(" #condition ")"))

/**
@verbatim
failed: test_assert_true(<#value>)
	at <file>:<line>
@endverbatim
 */
#define test_assert_true(value) \
	__test_assertion_handle((value), \
			__test_assertion_point_ref("test_assert_true(" #value ")"))

/**
@verbatim
failed: test_assert_false(<#value>)
	at <file>:<line>
@endverbatim
 */
#define test_assert_false(value) \
	__test_assertion_handle(!(value), \
			__test_assertion_point_ref("test_assert_false(" #value ")"))

/**
@verbatim
failed: test_assert_zero(<#value>): <value>
	at <file>:<line>
@endverbatim
 */
#define test_assert_zero(value) \
	__test_assertion_handle(!(value), \
			__test_assertion_point_ref("test_assert_zero(" #value ")"))

/**
@verbatim
failed: test_assert_not_zero(<#value>)
	at <file>:<line>
@endverbatim
 */
#define test_assert_not_zero(value) \
	__test_assertion_handle((value), \
			__test_assertion_point_ref("test_assert_not_zero(" #value ")"))

/**
@verbatim
failed: test_assert_null(<#value>): <value>
	at <file>:<line>
@endverbatim
 */
#define test_assert_null(value) \
	__test_assertion_handle(!(value), \
			__test_assertion_point_ref("test_assert_null(" #value ")"))

/**
@verbatim
failed: test_assert_not_null(<#value>)
	at <file>:<line>
@endverbatim
 */
#define test_assert_not_null(value) \
	__test_assertion_handle((uintptr_t) (value), \
			__test_assertion_point_ref("test_assert_not_null(" #value ")"))

/**
@verbatim
failed: test_assert_equal(<#actual>, <#expected>): <actual>, expected <expected>
	at <file>:<line>
@endverbatim
 */
#define test_assert_equal(actual, expected) \
	__test_assertion_handle((actual) == (expected), \
			__test_assertion_point_ref( "test_assert_equal(" #actual ", " \
					#expected ")"))

/**
@verbatim
failed: test_assert_not_equal(<#actual>, <#expected>): <actual>
	at <file>:<line>
@endverbatim
 */
#define test_assert_not_equal(actual, expected) \
	__test_assertion_handle((actual) != (expected), \
			__test_assertion_point_ref( "test_assert_not_equal(" #actual ", " \
					#expected ")"))

#define test_assert_str_equal(actual, expected) \
	__test_assertion_handle(((actual) == (expected)) \
			|| (0 == strcmp((actual), (expected))), \
			__test_assertion_point_ref( "test_assert_str_equal(" \
					 #actual ", " #expected ")"))

#define test_assert_str_not_equal(actual, expected) \
	__test_assertion_handle(((actual) != (expected)) \
			&& (0 != strcmp((actual), (expected))), \
			__test_assertion_point_ref( "test_assert_str_not_equal(" \
					#actual ", " #expected ")"))

#define test_assert_strn_equal(actual, expected, n) \
	__test_assertion_handle(((actual) == (expected)) \
			|| (0 == strncmp((actual), (expected), (n))), \
			__test_assertion_point_ref( "test_assert_strn_equal(" \
					#actual ", " #expected ", " #n ")"))

#define test_assert_strn_not_equal(actual, expected, n) \
	__test_assertion_handle(((actual) != (expected)) \
			&& (0 != strncmp((actual), (expected), (n))), \
			__test_assertion_point_ref( "test_assert_strn_not_equal(" \
					#actual ", " #expected ", " #n ")"))

#define test_assert_mem_equal(actual, expected, n) \
	__test_assertion_handle(0 == memcmp((actual), (expected), (n)), \
			__test_assertion_point_ref( "test_assert_mem_equal(" \
					#actual ", " #expected ", " #n ")"))

#define test_assert_mem_not_equal(actual, expected, n) \
	__test_assertion_handle(0 != memcmp((actual), (expected), (n)), \
			__test_assertion_point_ref( "test_assert_mem_not_equal(" \
					#actual ", " #expected ", " #n ")"))

#define test_assert_emitted(expected) \
	__test_assertion_handle(0 == strcmp(test_get_emitted(), (expected)), \
			__test_assertion_point_ref( "test_assert_emitted(" #expected ")"))


/* Hide internals from CDT macro expansion. */
#ifdef __CDT_PARSER__

#undef __test_fail
#define __test_fail(reason) \
	test_fail(reason)

#undef __test_assert
#define __test_assert(condition, condition_str) \
	test_assert(condition)

#undef __test_assert_true
#define __test_assert_true(value, value_str) \
	test_assert_true(value)

#undef __test_assert_false
#define __test_assert_false(value, value_str) \
	test_assert_false(value)

#undef __test_assert_zero
#define __test_assert_zero(value, value_str) \
	test_assert_zero(value)

#undef __test_assert_not_zero
#define __test_assert_not_zero(value, value_str) \
	test_assert_not_zero(value)

#undef __test_assert_null
#define __test_assert_null(value, value_str) \
	test_assert_null(value)

#undef __test_assert_not_null
#define __test_assert_not_null(value, value_str) \
	test_assert_not_null(value)

#undef __test_assert_equal
#define __test_assert_equal(actual, expected, act_str, exp_str) \
	test_assert_equal(actual, expected)

#undef __test_assert_not_equal
#define __test_assert_not_equal(actual, expected, act_str, exp_str) \
	test_assert_not_equal(actual, expected)

#undef __test_assert_str_equal
#define __test_assert_str_equal(actual, expected, act_str, exp_str) \
	test_assert_str_equal(actual, expected)

#undef __test_assert_str_not_equal
#define __test_assert_str_not_equal(actual, expected, act_str, exp_str) \
	test_assert_str_not_equal(actual, expected)

#undef __test_assert_strn_equal
#define __test_assert_strn_equal(actual, expected, n, \
		act_str, exp_str, n_str) \
	test_assert_strn_equal(actual, expected, n)

#undef __test_assert_strn_not_equal
#define __test_assert_strn_not_equal(actual, expected, n, \
		act_str, exp_str, n_str) \
	test_assert_strn_not_equal(actual, expected, n)

#undef __test_assert_mem_equal
#define __test_assert_mem_equal(actual, expected, n, \
		act_str, exp_str, n_str) \
	test_assert_mem_equal(actual, expected, n)

#undef __test_assert_mem_not_equal
#define __test_assert_mem_not_equal(actual, expected, n, \
		act_str, exp_str, n_str) \
	test_assert_mem_not_equal(actual, expected, n)

#undef __test_assert_emitted
#define __test_assert_emitted(expected, exp_str) \
	test_assert_emitted(expected)

#endif /* __CDT_PARSER__ */

#endif /* FRAMEWORK_TEST_ASSERT_H_ */
