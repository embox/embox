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

#define __test_failure_ref(_reason) __extension__ ({ \
		/* Statically allocate and define. Location and reason message are    \
		 * known at compile time and nobody cares about .rodata section. */   \
		static const struct __test_assertion_point __test_assertion_point = { \
			.location = LOCATION_FUNC_INIT,    \
			.reason = _reason,                 \
		};                                     \
		&__test_assertion_point;               \
	})

#define __test_fail(reason) \
		__test_assertion_handle0(0, \
				__test_failure_ref("test_fail(\"" reason "\")"))

#define __test_assert(condition, condition_str) \
		__test_assertion_handle0((condition), \
				__test_failure_ref("test_assert(" condition_str ")"))

#endif /* FRAMEWORK_TEST_ASSERT_IMPL_H_ */
