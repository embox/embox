/**
 * @file
 * @brief API for registering tests in Embox testing framework.
 *
 * @sa EMBOX_TEST() simple macro used to declare a new test
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

#include __impl_x(framework/test/self_impl.h)

#define EMBOX_TEST_SUITE(description) \
	  __EMBOX_TEST_SUITE(description)

#define TEST_CASE(description) \
	  __TEST_CASE(description)

#define TEST_SETUP_SUITE(setup) \
	  __TEST_SETUP_SUITE(setup)

#define TEST_TEARDOWN_SUITE(teardown) \
	  __TEST_TEARDOWN_SUITE(teardown)

#define TEST_SETUP(setup_each) \
	  __TEST_SETUP(setup_each)

#define TEST_TEARDOWN(teardown_each) \
	  __TEST_TEARDOWN(teardown_each)

/** @deprecated Use #EMBOX_TEST_SUITE() and #TEST_CASE() instead. */
#define EMBOX_TEST(run) \
	  __EMBOX_TEST(run)

#endif /* FRAMEWORK_TEST_SELF_H_ */
