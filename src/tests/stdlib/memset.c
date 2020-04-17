/**
 * @file
 * @brief Unit tests for stdlib/memset.
 *
 * @date Mar 26, 2020
 * @author: Khaled Emara
 */

#include <string.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("memset test suit");

TEST_CASE("memset simple test") {
	char str[] = "string";
	char exp[] = "------";

	memset(str, '-', strlen(str));

	test_assert_str_equal(str, exp);
}

TEST_CASE("memset first n elements") {
	char str[] = "string";
	char exp[] = "------";

	memset(str, '-', 3);

	test_assert_strn_equal(str, exp, 3);
}

TEST_CASE("memset negative number") {
	char str[] = {1, 2, 3, 4, 5};
	unsigned char exp[] = {-1, -1, -1, -1, -1};

	memset(str, -1, sizeof str);

	test_assert_mem_equal(str, exp, sizeof str);
}

TEST_CASE("memset middle of the buffer") {
	char str[] = {1, 2, 3, 4, 5};
	char exp[] = {1, 0, 0, 0, 5};

	memset(str + 1, 0, 3);

	test_assert_mem_equal(str, exp, sizeof str);
}
