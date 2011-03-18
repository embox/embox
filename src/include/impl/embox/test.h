/**
 * @file
 * @brief Hidden implementations of test registration macros.
 *
 * @date 12.06.2010
 * @author Eldar Abusalimov
 */

#ifndef EMBOX_TEST_H_
# error "Do not include this file directly, use <embox/test.h> instead!"
#endif /* EMBOX_TEST_H_ */

#include <util/array.h>
#include <util/macro.h>
#include <mod/self.h>

#include <impl/test/types.h>

#define __EMBOX_TEST(_run) \
	__EMBOX_TEST_SUITE("generic test suite"); \
	__TEST_CASE_NM("generic test case", MACRO_GUARD(__test_case), _run)

#define __EMBOX_TEST_SUITE(description) \
	__EMBOX_TEST_SUITE_NM("" description, MACRO_GUARD(__test_suite))

#define __EMBOX_TEST_SUITE_NM(_description, _test_suite) \
	ARRAY_SPREAD_DEF_TERMINATED(static const  struct test_case *, \
			__TEST_CASES_ARRAY, NULL);                            \
	static struct __test_private __test_private;                  \
	ARRAY_SPREAD_ADD_NAMED(__test_registry, _test_suite, {        \
			.private = &__test_private,                           \
			.test_cases = __TEST_CASES_ARRAY,                     \
			.mod = &mod_self,                                     \
			.description = _description,                          \
		});                                                       \
	MOD_SELF_BIND(_test_suite, &__test_mod_ops)

#define __TEST_CASE(description) \
	__TEST_CASE_NM("" description, MACRO_GUARD(__test_case), \
			MACRO_GUARD(__test_case_run))

#define __TEST_CASE_NM(_description, test_case_nm, run_nm) \
	static int run_nm(void);                             \
	static const struct test_case test_case_nm = {       \
		.run = run_nm,                                   \
		.location = __TEST_CASE_LOCATION_INIT,           \
		.description = _description,                     \
	};                                                   \
	extern const struct test_case *__TEST_CASES_ARRAY[]; \
	ARRAY_SPREAD_ADD(__TEST_CASES_ARRAY, &test_case_nm); \
	static int run_nm(void)

#define __TEST_CASES_ARRAY \
	MACRO_CONCAT(__test_cases__,__EMBUILD_MOD__)

extern const struct mod_ops __test_mod_ops;
extern const struct test __test_registry[];
