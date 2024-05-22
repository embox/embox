/**
 * @file
 *
 * @date 24 july 2015
 * @author: Anton Bondarev
 */

#ifndef SRC_TESTS_C___TEST_CXX_H_
#define SRC_TESTS_C___TEST_CXX_H_

#include <framework/test/self.h>

# define __TEST_FIXTURE_OP_DEF(fixture_nm, function_nm)  \
	static const __test_fixture_op_t                     \
			__TEST_FIXTURE_OP(fixture_nm) = function_nm

#define EMBOX_TEST_SUITE_EXT(_description, _suite_setup, _suite_teardown,    \
		_case_setup, _case_teardown)                                           \
	__TEST_FIXTURE_OP_DEF(suite_setup, _suite_setup);                          \
	__TEST_FIXTURE_OP_DEF(suite_teardown, _suite_teardown);                    \
	__TEST_FIXTURE_OP_DEF(case_setup, _case_setup);                            \
	__TEST_FIXTURE_OP_DEF(case_teardown, _case_teardown);                      \
	__EMBOX_TEST_SUITE_NM_EXT("" _description, &__TEST_FIXTURE_OP(suite_setup), \
			&__TEST_FIXTURE_OP(suite_teardown), &__TEST_FIXTURE_OP(case_setup), \
			&__TEST_FIXTURE_OP(case_teardown), __test_suite,                   \
			MACRO_GUARD(__test_private), true)

#endif /* SRC_TESTS_C___TEST_CXX_H_ */
