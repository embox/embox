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
#include <sys/cdefs.h>

#include <util/macro.h>
#include <util/array.h>
#include <util/location.h>
#include <framework/mod/self.h>

#include <framework/test/api.h>
#include "types.h"

#define __EMBOX_TEST_SUITE(description) \
	__EMBOX_TEST_SUITE_AUTORUN(description, true)

#define __EMBOX_TEST_SUITE_AUTORUN(description, autorun) \
	__EMBOX_TEST_SUITE_NM("" description, __test_suite, \
			MACRO_GUARD(__test_private), autorun)

#define __EMBOX_TEST_SUITE_NM(_description, test_suite_nm, test_private_nm,    \
		_autorun)                                                              \
	__TEST_FIXTURE_OP_DECL(suite_setup);                                       \
	__TEST_FIXTURE_OP_DECL(suite_teardown);                                    \
	__TEST_FIXTURE_OP_DECL(case_setup);                                        \
	__TEST_FIXTURE_OP_DECL(case_teardown);                                     \
	__EMBOX_TEST_SUITE_NM_EXT(_description, &__TEST_FIXTURE_OP(suite_setup),   \
			&__TEST_FIXTURE_OP(suite_teardown), &__TEST_FIXTURE_OP(case_setup), \
			&__TEST_FIXTURE_OP(case_teardown), test_suite_nm, test_private_nm, \
			_autorun)

#define __EMBOX_TEST_SUITE_EXT(_description, _suite_setup, _suite_teardown,    \
		_case_setup, _case_teardown)                                           \
	__TEST_FIXTURE_OP_DEF(suite_setup, _suite_setup);                          \
	__TEST_FIXTURE_OP_DEF(suite_teardown, _suite_teardown);                    \
	__TEST_FIXTURE_OP_DEF(case_setup, _case_setup);                            \
	__TEST_FIXTURE_OP_DEF(case_teardown, _case_teardown);                      \
	__EMBOX_TEST_SUITE_NM_EXT("" _description, &__TEST_FIXTURE_OP(suite_setup), \
			&__TEST_FIXTURE_OP(suite_teardown), &__TEST_FIXTURE_OP(case_setup), \
			&__TEST_FIXTURE_OP(case_teardown), __test_suite,                   \
			MACRO_GUARD(__test_private), true)

#define __EMBOX_TEST_SUITE_NM_EXT(_description, _suite_setup, _suite_teardown, \
		_case_setup, _case_teardown, test_suite_nm, test_private_nm, _autorun) \
	EXTERN_C const struct mod_ops __test_mod_ops;                    \
	ARRAY_SPREAD_DEF_TERMINATED(static const struct test_case *,     \
			__TEST_CASES_ARRAY, NULL);                               \
	static struct __test_private test_private_nm;                    \
	MOD_SELF_INIT_DECLS(__EMBUILD_MOD__);                            \
	const struct test_mod mod_self = {                               \
		/* .mod   = */ MOD_SELF_INIT(__EMBUILD_MOD__, &__test_mod_ops), \
		/* .suite = */ {                                             \
			/* .test_cases        = */ __TEST_CASES_ARRAY,           \
			/* .description       = */ _description,                 \
			/* .autorun           = */ _autorun,                     \
			/* .test_priv         = */ &test_private_nm,             \
			/* .suite_fixture_ops = */ {                             \
				/* .p_setup    = */ _suite_setup,                    \
				/* .p_teardown = */ _suite_teardown,                 \
			},                                                       \
			/* .case_fixture_ops = */ {                              \
				/* .p_setup    = */ _case_setup,                     \
				/* .p_teardown = */ _case_teardown,                  \
			},                                                       \
		}                                                            \
	};                                                               \
	TEST_ADD(&mod_self.suite)

#define __TEST_FIXTURE_OP(fixture_nm) \
	MACRO_CONCAT(__test_fixture_, fixture_nm)

#define __TEST_FIXTURE_OP_DECL(fixture_nm) \
	static const __test_fixture_op_t __TEST_FIXTURE_OP(fixture_nm)

#ifndef __cplusplus
# define __TEST_FIXTURE_OP_DEF(fixture_nm, function_nm)  \
	static int function_nm(void);                        \
	static const __test_fixture_op_t                     \
			__TEST_FIXTURE_OP(fixture_nm) = function_nm; \
	static int function_nm(void)
#else /* __cplusplus */
# define __TEST_FIXTURE_OP_DEF(fixture_nm, function_nm)  \
	static const __test_fixture_op_t                     \
			__TEST_FIXTURE_OP(fixture_nm) = function_nm
#endif /* !__cplusplus */

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
		/* .run         = */ run_nm,                     \
		/* .description = */ _description,               \
		/* .location    = */ LOCATION_INIT,              \
	};                                                   \
	ARRAY_SPREAD_ADD(__TEST_CASES_ARRAY, &test_case_nm); \
	static void run_nm(void)

#define __TEST_CASES_ARRAY \
	MACRO_CONCAT(__test_cases_in_suite__, __EMBUILD_MOD__)

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
	static int function_nm(); \
	typedef typeof(function_nm) __test_fixture_placeholder

# undef  __TEST_CASE
# define __TEST_CASE(ignored) \
	static void MACRO_GUARD(__test_case)(void); \
	typedef typeof(MACRO_GUARD(__test_case)) __test_case_placeholder; \
	static void MACRO_GUARD(__test_case)(void)

# undef  __EMBOX_TEST
# define __EMBOX_TEST(_run) \
	int _run(void)

#endif /* __CDT_PARSER__ */

#endif /* FRAMEWORK_TEST_SELF_IMPL_H_ */
