/**
 * @file
 * @brief Test foreach iteration macro and inline versions of some methods from
 *        test registry.
 *
 * @date 11.06.10
 * @author Eldar Abusalimov
 */

#ifndef FRAMEWORK_TEST_API_IMPL_H_
#define FRAMEWORK_TEST_API_IMPL_H_

#include <stddef.h>

#include <util/array.h>
#include <util/member.h>
#include <framework/mod/api.h>

#include "types.h"

#define __test_foreach(test_ptr) \
	array_spread_foreach(test_ptr, __test_registry)

ARRAY_SPREAD_DECLARE(const struct test_suite *, __test_registry);

#define TEST_ADD(_suite_ptr) \
	ARRAY_SPREAD_DECLARE(const struct test_suite *, __test_registry);\
	ARRAY_SPREAD_ADD(__test_registry, _suite_ptr)

#include <util/member.h>
static inline const struct test_mod *__test_mod(const struct test_suite *test) {
	return member_cast_out(test, const struct test_mod, suite);
}

static inline const char *test_name(const struct test_suite *test) {
	return mod_name(&__test_mod(test)->mod);
}

static inline const char *test_package(const struct test_suite *test) {
	return mod_pkg_name(&__test_mod(test)->mod);
}

#endif /* FRAMEWORK_TEST_API_IMPL_H_ */
