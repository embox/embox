/**
 * @file
 * @brief Embox testing framework.
 * TODO Test framework API docs: structs. -- Eldar
 *
 * @date 11.06.10
 * @author Eldar Abusalimov
 */

#ifndef FRAMEWORK_TEST_API_H_
#define FRAMEWORK_TEST_API_H_

#include <framework/mod/api.h>
#include <framework/test/types.h>
#include <lib/libds/array.h>
#include <util/member.h>

#define test_foreach(test_ptr) array_spread_foreach(test_ptr, __test_registry)

ARRAY_SPREAD_DECLARE(const struct test_suite *const, __test_registry);

#define TEST_REGISTER(_suite_ptr)                                          \
	ARRAY_SPREAD_DECLARE(const struct test_suite *const, __test_registry); \
	ARRAY_SPREAD_ADD(__test_registry, _suite_ptr)

static inline const struct test_mod *__test_mod(const struct test_suite *test) {
	return member_cast_out(test, const struct test_mod, suite);
}

static inline const char *test_name(const struct test_suite *test) {
	return mod_name(&__test_mod(test)->mod);
}

static inline const char *test_package(const struct test_suite *test) {
	return mod_pkg_name(&__test_mod(test)->mod);
}

/**
 * TODO docs. -- Eldar
 */
struct test_suite;

extern int test_suite_run(const struct test_suite *test);

extern const struct test_suite *test_lookup(const char *name);

#endif /* FRAMEWORK_TEST_API_H_ */
