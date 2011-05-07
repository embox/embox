/**
 * @file
 * @brief Hidden implementations of test registration macros.
 *
 * @date 12.06.10
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
	static const __test_fixture_op_t                                 \
			__TEST_FIXTURE_OP(suite_setup),                          \
			__TEST_FIXTURE_OP(suite_teardown);                       \
	static const __test_fixture_op_t                                 \
			__TEST_FIXTURE_OP(case_setup),                           \
			__TEST_FIXTURE_OP(case_teardown);                        \
	ARRAY_SPREAD_ADD_NAMED(__test_registry, test_suite_nm, {         \
			.private = &test_private_nm,                             \
			.test_cases = __TEST_CASES_ARRAY,                        \
			.mod = &mod_self,                                        \
			.description = _description,                             \
			.suite_fixture_ops = {                                   \
					.p_setup    = &__TEST_FIXTURE_OP(suite_setup),   \
					.p_teardown = &__TEST_FIXTURE_OP(suite_teardown),\
				},                                                   \
			.case_fixture_ops = {                                    \
					.p_setup    = &__TEST_FIXTURE_OP(case_setup),    \
					.p_teardown = &__TEST_FIXTURE_OP(case_teardown), \
				},                                                   \
		});                                                          \
	MOD_SELF_BIND(test_suite_nm, &__test_mod_ops)

#define __TEST_FIXTURE_OP(fixture_nm) \
	MACRO_CONCAT(__test_fixture_, fixture_nm)

#define __TEST_FIXTURE_OP_DECL(fixture_nm) \
	static const __test_fixture_op_t __TEST_FIXTURE_OP(fixture_nm)

#define __TEST_FIXTURE_OP_DEF(fixture_nm, function_nm) \
	static int function_nm(void);                        \
	static const __test_fixture_op_t                     \
			__TEST_FIXTURE_OP(fixture_nm) = function_nm; \
	static int function_nm(void)

#define __TEST_SETUP_SUITE(function_nm) \
	__TEST_FIXTURE_OP_DEF(suite_setup, function_nm)
#define __TEST_TEARDOWN_SUITE(function_nm) \
	__TEST_FIXTURE_OP_DEF(suite_teardown, function_nm)

#define __TEST_SETUP(function_nm) \
	__TEST_FIXTURE_OP_DEF(case_setup, function_nm)
#define __TEST_TEARDOWN(function_nm) \
	__TEST_FIXTURE_OP_DEF(case_teardown, function_nm)

#define __TEST_CASE(description) \
	__TEST_CASE_NM("" description, MACRO_GUARD(__test_case_struct), \
			MACRO_GUARD(__test_case))

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
	}                                          \
	static int _run(void)

/* Simplify the life of Eclipse CDT. */
#ifdef __CDT_PARSER__

# undef  __EMBOX_TEST_SUITE
# define __EMBOX_TEST_SUITE(ignored) \
	typedef int __test_suite_placeholder

# undef  __TEST_FIXTURE_OP_DEF
# define __TEST_FIXTURE_OP_DEF(ignored, function_nm) \
	static int function_nm(void)

# undef  __TEST_CASE
# define __TEST_CASE(ignored) \
	static void MACRO_GUARD(__test_case)(void)

# undef  __EMBOX_TEST
# define __EMBOX_TEST(_run) \
	static int _run(void)

#endif /* __CDT_PARSER__ */

#endif /* FRAMEWORK_TEST_SELF_IMPL_H_ */
