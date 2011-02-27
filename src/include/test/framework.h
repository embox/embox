/**
 * @file
 * @brief The external API for EMBOX testing framework.
 * TODO Test framework API docs: structs. -- Eldar
 *
 * @date 11.06.2010
 * @author Eldar Abusalimov
 */

#ifndef TEST_FRAMEWORK_H_
#define TEST_FRAMEWORK_H_

#include <impl/test/framework.h>

#define test_foreach(test_ptr) \
		__test_foreach(test_ptr)

/**
 * TODO docs. -- Eldar
 */
struct test;

extern int test_invoke(struct test *test);

extern const char *test_name(struct test *test);

#endif /* TEST_FRAMEWORK_H_ */
