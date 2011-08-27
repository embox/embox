/**
 * @file
 * @brief Tests proper stack usage.
 *
 * @date 14.08.09
 * @author Eldar Abusalimov
 */

#include <embox/test.h>

EMBOX_TEST_SUITE("Stack usage tests");

static int fib(int k) {
	if (k < 2) {
		return k;
	} else {
		return fib(k - 1) + fib(k - 2);
	}
}

TEST_CASE("Recursively calculating Fibonacci number") {
	test_assert_equal(fib(0), 0);
	test_assert_equal(fib(1), 1);
	test_assert_equal(fib(2), 1);
	test_assert_equal(fib(3), 2);
	test_assert_equal(fib(4), 3);
	test_assert_equal(fib(5), 5);

	/* This is madness. This is SPARC!!! */
	test_assert_equal(fib(17), 1597);
}
