/**
 * @file
 * @brief Test assertion macros implementation.
 *
 * @date 12.06.2010
 * @author Eldar Abusalimov
 */

#ifndef FRAMEWORK_TEST_ASSERT_IMPL_H_
#define FRAMEWORK_TEST_ASSERT_IMPL_H_

#include <util/location.h>

#include "types.h"

extern void __test_assertion_handle0(int pass,
		const struct __test_assertion_point *point);

extern void __test_assertion_handle1(int pass,
		const struct __test_assertion_point *point, void *arg1);

extern void __test_assertion_handle2(int pass,
		const struct __test_assertion_point *point, void *arg1, void *arg2);

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
	__test_assertion_handle0(0, \
			__test_assertion_point_ref("test_fail(\"" reason "\")"))

#define __test_assert(condition, condition_str) \
	__test_assertion_handle0((condition), \
			__test_assertion_point_ref("test_assert(" condition_str ")"))

#define __test_assert_true(value, value_str) \
	__test_assertion_handle0((value), \
			__test_assertion_point_ref("test_assert_true(" value_str ")"))

#define __test_assert_false(value, value_str) \
	__test_assertion_handle0(!(value), \
			__test_assertion_point_ref("test_assert_false(" value_str ")"))

#define __test_assert_zero(value, value_str) \
	__test_assertion_handle0(!(value), \
			__test_assertion_point_ref("test_assert_zero(" value_str ")"))

#define __test_assert_not_zero(value, value_str) \
	__test_assertion_handle0((value), \
			__test_assertion_point_ref("test_assert_not_zero(" value_str ")"))

#define __test_assert_null(value, value_str) \
	__test_assertion_handle0(!(value), \
			__test_assertion_point_ref("test_assert_null(" value_str ")"))

#define __test_assert_not_null(value, value_str) \
	__test_assertion_handle0((int) (value), \
			__test_assertion_point_ref("test_assert_not_null(" value_str ")"))

#define __test_assert_equal(actual, expected, actual_str, expected_str) \
	__test_assertion_handle0((actual) == (expected), \
			__test_assertion_point_ref( "test_assert_equal(" actual_str ", " \
					expected_str ")"))

#define __test_assert_not_equal(actual, expected, actual_str, expected_str) \
	__test_assertion_handle0((actual) != (expected), \
			__test_assertion_point_ref( "test_assert_not_equal(" actual_str \
					", " expected_str ")"))

#endif /* FRAMEWORK_TEST_ASSERT_IMPL_H_ */
