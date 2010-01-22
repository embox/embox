/**
 * \file test_template.c
 *
 * \date 22.01.10
 * \author Alexey Fomin
 */

#include <express_tests.h>


/*
 * If you want to add info function
 * write:
DECLARE_EXPRESS_TEST(TEST_NAME_IN_UPPER_CASE, exec_func, your_info_func);
...
static void your_info_func(void) {...}
 *
 * But usually you don't need that. Then leave NULL: */
DECLARE_EXPRESS_TEST(TEST_NAME_IN_UPPER_CASE, your_exec_func, NULL);

static int your_exec_func(int argc, char** argv) {
	return 0;
}
