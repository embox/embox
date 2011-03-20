/**
 * @file
 * @brief Embox testing framework.
 * XXX obsolete docs. -- Eldar
 * @details Test is a simple function which examines some parts of hardware or
 * software. Test can be either passed or failed (framework uses return value
 * of test function to determine the test result).
 * Each test resides in a single mod which is usually defined in
 * @code embox.test @endcode or @code <platform>.test @endcode packages.
 *
 * @sa EMBOX_TEST() simple macro used to declare a new test
 *
 * @date Jul 4, 2009
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

/** @deprecated Use #EMBOX_TEST_SUITE() and #TEST_CASE() instead. */
#define EMBOX_TEST(run) \
	  __EMBOX_TEST(run)

#endif /* FRAMEWORK_TEST_SELF_H_ */
