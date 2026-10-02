/**
 * @file
 * @brief API for registering tests in Embox testing framework.
 *
 *
 * @date 04.06.09
 * @author Anton Bondarev, Alexey Fomin
 *         - Initial implementation
 * @author Eldar Abusalimov
 *         - Rewriting from scratch, adapting for usage with mods framework
 *         - Split external and internal APIs
 *         - Hiding complicated macros implementation
 *         - Introducing test suites
 */

#ifndef FRAMEWORK_TEST_SELF_H_
#define FRAMEWORK_TEST_SELF_H_

#include <stddef.h>
#include <sys/cdefs.h>

#include <framework/mod/self.h>
#include <framework/test/api.h>
#include <framework/test/types.h>
#include <lib/libds/array.h>
#include <util/location.h>
#include <util/macro.h>

#define EMBOX_TEST_SUITE(description)                   \
	__EMBOX_TEST_SUITE_NM("" description, __test_suite, \
	    MACRO_GUARD(__test_private), true)

#define TEST_CASE(description)                                      \
	__TEST_CASE_NM("" description, MACRO_GUARD(__test_case_struct), \
	    MACRO_GUARD(__test_case))

#define TEST_SETUP_SUITE(setup) __TEST_FIXTURE_OP_DEF(suite_setup, setup)

#define TEST_TEARDOWN_SUITE(teardown) \
	__TEST_FIXTURE_OP_DEF(suite_teardown, teardown)

#define TEST_SETUP(setup_each) __TEST_FIXTURE_OP_DEF(case_setup, setup_each)

#define TEST_TEARDOWN(teardown_each) \
	__TEST_FIXTURE_OP_DEF(case_teardown, teardown_each)

#define __EMBOX_TEST_SUITE_NM(_description, test_suite_nm, test_private_nm,  \
    _autorun)                                                                \
	__TEST_FIXTURE_OP_DECL(suite_setup);                                     \
	__TEST_FIXTURE_OP_DECL(suite_teardown);                                  \
	__TEST_FIXTURE_OP_DECL(case_setup);                                      \
	__TEST_FIXTURE_OP_DECL(case_teardown);                                   \
	__EMBOX_TEST_SUITE_NM_EXT(_description, &__TEST_FIXTURE_OP(suite_setup), \
	    &__TEST_FIXTURE_OP(suite_teardown), &__TEST_FIXTURE_OP(case_setup),  \
	    &__TEST_FIXTURE_OP(case_teardown), test_suite_nm, test_private_nm,   \
	    _autorun)

/* clang-format off */
#define __EMBOX_TEST_SUITE_NM_EXT(_description, _suite_setup, _suite_teardown, \
    _case_setup, _case_teardown, test_suite_nm, test_private_nm, _autorun)     \
	ARRAY_SPREAD_DEF_TERMINATED(static const struct test_case *,               \
	    __TEST_CASES_ARRAY, NULL);                                             \
	static struct __test_private test_private_nm;                              \
	const struct test_mod __MOD(__EMBUILD_MOD__) = {                           \
	    /* .mod   = */ MOD_SELF_INIT(&__test_mod_ops),                         \
		/* .suite = */ {                                                       \
	        /* .test_cases        = */ __TEST_CASES_ARRAY,                     \
	        /* .description       = */ _description,                           \
	        /* .autorun           = */ _autorun,                               \
	        /* .test_priv         = */ &test_private_nm,                       \
			/* .suite_fixture_ops = */ {                                       \
	            /* .p_setup    = */ _suite_setup,                              \
	            /* .p_teardown = */ _suite_teardown,                           \
	        },                                                                 \
			/* .case_fixture_ops = */ {                                        \
	            /* .p_setup    = */ _case_setup,                               \
	            /* .p_teardown = */ _case_teardown,                            \
	        },                                                                 \
	    }};                                                                    \
	TEST_REGISTER(&__MOD(__EMBUILD_MOD__).suite);                              \
	MOD_SELF_RUNTIME()
/* clang-format on */

#define __TEST_CASE_NM(_description, test_case_nm, run_nm) \
	static void run_nm(void);                              \
	static const struct test_case test_case_nm = {         \
	    /* .run         = */ run_nm,                       \
	    /* .description = */ _description,                 \
	    /* .location    = */ LOCATION_INIT,                \
	};                                                     \
	ARRAY_SPREAD_ADD(__TEST_CASES_ARRAY, &test_case_nm);   \
	static void run_nm(void)

#define __TEST_FIXTURE_OP(fixture_nm) MACRO_CONCAT(__test_fixture_, fixture_nm)

#define __TEST_FIXTURE_OP_DECL(fixture_nm) \
	static const __test_fixture_op_t __TEST_FIXTURE_OP(fixture_nm)

#ifndef __cplusplus
#define __TEST_FIXTURE_OP_DEF(fixture_nm, function_nm)  \
	static int function_nm(void);                       \
	static const __test_fixture_op_t __TEST_FIXTURE_OP( \
	    fixture_nm) = function_nm;                      \
	static int function_nm(void)
#endif /* __cplusplus */

#define __TEST_CASES_ARRAY \
	MACRO_CONCAT(__test_cases_in_suite__, __EMBUILD_MOD__)

/* Simplify the life of Eclipse CDT. */
#ifdef __CDT_PARSER__

#undef __EMBOX_TEST_SUITE
#define __EMBOX_TEST_SUITE(ignored) typedef int __test_suite_placeholder

#undef __TEST_FIXTURE_OP_DEF
#define __TEST_FIXTURE_OP_DEF(ignored, function_nm) \
	static int function_nm();                       \
	typedef typeof(function_nm) __test_fixture_placeholder

#undef __TEST_CASE
#define __TEST_CASE(ignored)                                          \
	static void MACRO_GUARD(__test_case)(void);                       \
	typedef typeof(MACRO_GUARD(__test_case)) __test_case_placeholder; \
	static void MACRO_GUARD(__test_case)(void)

#endif /* __CDT_PARSER__ */

__BEGIN_DECLS

extern const struct test_mod __MOD(__EMBUILD_MOD__);

extern const struct mod_ops __test_mod_ops;

static inline const struct mod *test_mod_self() {
	return &__MOD(__EMBUILD_MOD__).mod;
}

__END_DECLS

#endif /* FRAMEWORK_TEST_SELF_H_ */
