/**
 * @file
 * @brief Test assertion macros implementation.
 *
 * @date 12.06.2010
 * @author Eldar Abusalimov
 */

#ifndef FRAMEWORK_TEST_ASSERT_IMPL_H_
#define FRAMEWORK_TEST_ASSERT_IMPL_H_

#include <string.h>

#include <util/location.h>

#include "types.h"

extern void __test_assertion_handle(int pass,
		const struct __test_assertion_point *point);

#define __test_assertion_point_ref(_reason) \
	__extension__ ({                        \
		/* Statically allocate and define. Location and reason message are    \
		 * known at compile time and nobody cares about .rodata section. */   \
		static const struct __test_assertion_point __test_assertion_point = { \
			.location = LOCATION_FUNC_INIT, \
			.reason = _reason,              \
		};                                  \
		&__test_assertion_point;            \
	})

#define __test_fail(reason) \
	__test_assertion_handle(0, \
			__test_assertion_point_ref("test_fail(\"" reason "\")"))

#define __test_assert(condition, condition_str) \
	__test_assertion_handle((int) (condition), \
			__test_assertion_point_ref("test_assert(" condition_str ")"))

#define __test_assert_true(value, value_str) \
	__test_assertion_handle((value), \
			__test_assertion_point_ref("test_assert_true(" value_str ")"))

#define __test_assert_false(value, value_str) \
	__test_assertion_handle(!(value), \
			__test_assertion_point_ref("test_assert_false(" value_str ")"))

#define __test_assert_zero(value, value_str) \
	__test_assertion_handle(!(value), \
			__test_assertion_point_ref("test_assert_zero(" value_str ")"))

#define __test_assert_not_zero(value, value_str) \
	__test_assertion_handle((value), \
			__test_assertion_point_ref("test_assert_not_zero(" value_str ")"))

#define __test_assert_null(value, value_str) \
	__test_assertion_handle(!(value), \
			__test_assertion_point_ref("test_assert_null(" value_str ")"))

#define __test_assert_not_null(value, value_str) \
	__test_assertion_handle((int) (value), \
			__test_assertion_point_ref("test_assert_not_null(" value_str ")"))

#define __test_assert_equal(actual, expected, act_str, exp_str) \
	__test_assertion_handle((actual) == (expected), \
			__test_assertion_point_ref( "test_assert_equal(" act_str ", " \
					exp_str ")"))

#define __test_assert_not_equal(actual, expected, act_str, exp_str) \
	__test_assertion_handle((actual) != (expected), \
			__test_assertion_point_ref( "test_assert_not_equal(" act_str ", " \
					exp_str ")"))

#define __test_assert_str_equal(actual, expected, act_str, exp_str) \
	__test_assertion_handle(0 == strcmp((actual), (expected)), \
			__test_assertion_point_ref( "test_assert_str_equal(" \
					act_str ", " exp_str ")"))

#define __test_assert_str_not_equal(actual, expected, act_str, exp_str) \
	__test_assertion_handle(0 != strcmp((actual), (expected)), \
			__test_assertion_point_ref( "test_assert_str_not_equal(" \
					act_str ", " exp_str ")"))

#define __test_assert_strn_equal(actual, expected, n, act_str, exp_str) \
	__test_assertion_handle(0 == strncmp((actual), (expected), (n)), \
			__test_assertion_point_ref( "test_assert_strn_equal(" \
					act_str ", " exp_str ")"))

#define __test_assert_strn_not_equal(actual, expected, n, act_str, exp_str) \
	__test_assertion_handle(0 != strcmp((actual), (expected), (n)), \
			__test_assertion_point_ref( "test_assert_strn_not_equal(" \
					act_str ", " exp_str ")"))

#define __test_assert_mem_equal(actual, expected, n, act_str, exp_str) \
	__test_assertion_handle(0 == memcmp((actual), (expected), (n)), \
			__test_assertion_point_ref( "test_assert_mem_equal(" \
					act_str ", " exp_str ")"))

#define __test_assert_mem_not_equal(actual, expected, n, act_str, exp_str) \
	__test_assertion_handle(0 != memcmp((actual), (expected), (n)), \
			__test_assertion_point_ref( "test_assert_mem_not_equal(" \
					act_str ", " exp_str ")"))

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
#define __test_assert_strn_equal(actual, expected, n, act_str, exp_str) \
	test_assert_strn_equal(actual, expected, n)

#undef __test_assert_strn_not_equal
#define __test_assert_strn_not_equal(actual, expected, n, act_str, exp_str) \
	test_assert_strn_not_equal(actual, expected, n)

#undef __test_assert_mem_equal
#define __test_assert_mem_equal(actual, expected, n, act_str, exp_str) \
	test_assert_mem_equal(actual, expected, n)

#undef __test_assert_mem_not_equal
#define __test_assert_mem_not_equal(actual, expected, n, act_str, exp_str) \
	test_assert_mem_not_equal(actual, expected, n)

#endif /* __CDT_PARSER__ */

#endif /* FRAMEWORK_TEST_ASSERT_IMPL_H_ */
