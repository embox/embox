/**
 * @file
 * @brief EMBOX testing framework.
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
 *         - Hiding of complicated macro implementations
 */

#ifndef EMBOX_TEST_H_
#define EMBOX_TEST_H_

#include <impl/embox/test.h>

#define EMBOX_TEST(run) \
		__EMBOX_TEST(run)

#endif /* EMBOX_TEST_H_ */
