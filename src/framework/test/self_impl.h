/**
 * @file
 * @brief Hidden implementations of test registration macros.
 *
 * @date 12.06.2010
 * @author Eldar Abusalimov
 */

#ifndef FRAMEWORK_TEST_SELF_IMPL_H_
#define FRAMEWORK_TEST_SELF_IMPL_H_

#include <stddef.h>

#include <util/array.h>
#include <util/macro.h>
#include <util/location.h>
#include <framework/mod/self.h>

#include "types.h"

#define __EMBOX_TEST_SUITE(description) \
	__EMBOX_TEST_SUITE_NM("" description, __test_suite, \
			MACRO_GUARD(__test_private))

#define __EMBOX_TEST_SUITE_NM(_description, test_suite_nm, test_private_nm) \
	extern const struct test_suite __test_registry[];                \
	extern const struct mod_ops __test_mod_ops;                      \
	ARRAY_SPREAD_DEF_TERMINATED(static const struct test_case *,     \
			__TEST_CASES_ARRAY, NULL);                               \
	static struct __test_private test_private_nm;                    \
	__TEST_FIXTURE_DECL(setup);                                      \
	__TEST_FIXTURE_DECL(teardown);                                   \
	__TEST_FIXTURE_DECL(setup_each);                                 \
	__TEST_FIXTURE_DECL(teardown_each);                              \
	ARRAY_SPREAD_ADD_NAMED(__test_registry, test_suite_nm, {         \
			.private = &test_private_nm,                             \
			.test_cases = __TEST_CASES_ARRAY,                        \
			.mod = &mod_self,                                        \
			.description = _description,                             \
			.fixtures = {                                            \
					.setup         = &__TEST_FIXTURE(setup),         \
					.teardown      = &__TEST_FIXTURE(teardown),      \
					.setup_each    = &__TEST_FIXTURE(setup_each),    \
					.teardown_each = &__TEST_FIXTURE(teardown_each), \
				},                                                   \
		});                                                          \
	MOD_SELF_BIND(test_suite_nm, &__test_mod_ops)

#define __TEST_FIXTURE(fixture_nm) \
	MACRO_CONCAT(__test_fixture_, fixture_nm)

#define __TEST_FIXTURE_DECL(fixture_nm) \
	static const __test_fixture_t __TEST_FIXTURE(fixture_nm)

#define __TEST_FIXTURE_DEF(fixture_nm, function_nm) \
	static int function_nm(void);                   \
	__TEST_FIXTURE_DECL(fixture_nm) = (function_nm)

#define __TEST_SETUP(function_nm) \
	__TEST_FIXTURE_DEF(setup, function_nm)
#define __TEST_TEARDOWN(function_nm) \
	__TEST_FIXTURE_DEF(teardown, function_nm)

#define __TEST_SETUP_EACH(function_nm) \
	__TEST_FIXTURE_DEF(setup_each, function_nm)
#define __TEST_TEARDOWN_EACH(function_nm) \
	__TEST_FIXTURE_DEF(teardown_each, function_nm)

#define __TEST_CASE(description) \
	__TEST_CASE_NM("" description, MACRO_GUARD(__test_case), \
			MACRO_GUARD(__test_case_run))

#define __TEST_CASE_NM(_description, test_case_nm, run_nm) \
	static void run_nm(void);                            \
	static const struct test_case test_case_nm = {       \
		.run = run_nm,                                   \
		.location = LOCATION_INIT,                       \
		.description = _description,                     \
	};                                                   \
	extern const struct test_case *__TEST_CASES_ARRAY[]; \
	ARRAY_SPREAD_ADD(__TEST_CASES_ARRAY, &test_case_nm); \
	static void run_nm(void)

#define __TEST_CASES_ARRAY \
	MACRO_CONCAT(__test_cases__,__EMBUILD_MOD__)

/* This is implemented on top of test suite and test case. */
#define __EMBOX_TEST(_run) \
	static int _run(void);                     \
	__EMBOX_TEST_SUITE("generic test suite");  \
	__TEST_CASE("generic test case") {         \
		if (_run()) {                          \
			test_fail("non-zero return code"); \
		}                                      \
	} /* suppress "extra `;' outside of a function" warning. */ \
	static int _run(void)

/* Simplify the life of Eclipse CDT. */
#ifdef __CDT_PARSER__

# undef __EMBOX_TEST_SUITE
# define __EMBOX_TEST_SUITE(ignored) \
	typedef int __test_suite_placeholder

# undef __TEST_SETUP __TEST_TEARDOWN __TEST_SETUP_EACH __TEST_TEARDOWN_EACH
#define __TEST_SETUP(function_nm) \
	static int function_nm(void)
#define __TEST_TEARDOWN(function_nm) \
	static int function_nm(void)
#define __TEST_SETUP_EACH(function_nm) \
	static int function_nm(void)
#define __TEST_TEARDOWN_EACH(function_nm) \
	static int function_nm(void)

# undef __TEST_CASE
# define __TEST_CASE(ignored) \
	static void MACRO_GUARD(__test_case_run)(void)

#endif /* __CDT_PARSER__ */

#endif /* FRAMEWORK_TEST_SELF_IMPL_H_ */
