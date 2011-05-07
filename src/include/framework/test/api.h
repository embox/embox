/**
 * @file
 * @brief Embox testing framework.
 * TODO Test framework API docs: structs. -- Eldar
 *
 * @date 11.06.10
 * @author Eldar Abusalimov
 */

#ifndef FRAMEWORK_TEST_API_H_
#define FRAMEWORK_TEST_API_H_

#include __impl_x(framework/test/api_impl.h)

#define test_foreach(test_ptr) \
		__test_foreach(test_ptr)

/**
 * TODO docs. -- Eldar
 */
struct test_suite;

extern int test_suite_run(const struct test_suite *test);

extern const struct test_suite *test_lookup(const char *name);

extern const char *test_name(const struct test_suite *test);

#endif /* FRAMEWORK_TEST_API_H_ */
