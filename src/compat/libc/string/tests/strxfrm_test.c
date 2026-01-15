/**
 * @file
 * @brief
 *
 * @author  Nastya Nizharadze
 * @date    08.07.2019
 */

#include <stdlib.h>
#include <errno.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("strxfrm test");

TEST_CASE("n > strlen(src); strxfrm should return 4 and put in dest \"qwer\"") {
	char dest[6];

	test_assert_equal(strxfrm(dest, "qwer", 6), 4);
	test_assert_str_equal(dest, "qwer");
}

TEST_CASE("n < strlen(src); strxfrm should return 5 and put in dest \"qwe\"") {
	char dest[6];

	test_assert_equal(strxfrm(dest, "qwert", 3), 5);
}

TEST_CASE("n == 0; strxfrm should return 4") {
	char dest[6];

	test_assert_equal(strxfrm(dest, "qwer", 0), 4);
}

TEST_CASE("src == \"\\0\"; strxfrm should return 0") {
	char dest[6];

	test_assert_equal(strxfrm(dest, "\0", 7), 0);
}
