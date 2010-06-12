/**
 * @file
 * @brief The external API for EMBOX testing framework.
 *
 * @date 11.06.2010
 * @author Eldar Abusalimov
 */

#ifndef TEST_FRAMEWORK_H_
#define TEST_FRAMEWORK_H_

#include <stddef.h>
#include <stdbool.h>

#include <test/types.h>

#include <impl/test/framework.h>

#define test_foreach(t) __test_foreach(t)

struct test_iterator;

extern int test_invoke(struct test *test);

extern struct test_iterator *test_get_all(struct test_iterator *iterator);

extern struct test *test_iterator_next(struct test_iterator *iterator);

extern bool test_iterator_has_next(struct test_iterator *iterator);

#endif /* TEST_FRAMEWORK_H_ */
