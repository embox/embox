/**
 * @file
 * @brief Main Embox unit test include file for user tests.
 * @details
 *   Embox has built-in support for unit testing.
 *
 *   A system is organized in the same way as any other xUnit-family framework.
 * Individual test cases are packaged into suites, which in turn are bound to
 * the corresponding modules. Each test case in the suite and the test suite
 * itself can have setup and teardown functions which are automatically called
 * before and after running the case or the suite accordingly.
 *
 * @see EMBOX_TEST_SUITE()
 * @see TEST_CASE()
 * @see test_assert()
 *
 * @note
 *   This file is just a shortcut for
 *   <framework/test/self.h> and <framework/test/assert.h> headers
 *   introduced to easy a new test creation.
 *
 * @date 20.03.2011
 * @author Eldar Abusalimov
 */

#ifndef EMBOX_TEST_H_
#define EMBOX_TEST_H_

#include <framework/test/self.h>
#include <framework/test/assert.h>

#endif /* EMBOX_TEST_H_ */
