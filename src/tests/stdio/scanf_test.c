/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    04.04.2013
 */

#include <stdio.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("stdio/scanf test");

TEST_CASE("sscanf should accept fmt with some placeholders") {
	const char *data = "1234 5678";
	int a = 0, b = 0;

	test_assert_equal(sscanf(data, "%d %d", &a, &b), 2);

	test_assert_equal(a, 1234);
	test_assert_equal(b, 5678);
}

TEST_CASE("sscanf should keep case") {
	int a = 0, b = 0;

	test_assert_equal(sscanf("1234u5678", "%du%d", &a, &b), 2);

	test_assert_equal(a, 1234);
	test_assert_equal(b, 5678);
}

TEST_CASE("sscanf should work with hex") {
	int a = 0, b = 0;

	test_assert_equal(sscanf("123aub678", "%xu%x", &a, &b), 2);

	test_assert_equal(a, 0x123a);
	test_assert_equal(b, 0xb678);
}

TEST_CASE("sscanf should work with %n") {
	int a = 0, b = 0, na = 0, nb = 0;

	test_assert_equal(sscanf(" 123u   456", "%d%nu%d%n", &a, &na, &b, &nb), 2);

	test_assert_equal(a, 123);
	test_assert_equal(b, 456);
	test_assert_equal(na, 4);
	test_assert_equal(nb, 11);
}

