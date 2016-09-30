/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    24.08.2015
 */

#include <embox/test.h>
#include <framework/mod/api.h>

EMBOX_TEST_SUITE("integrity check tests");

TEST_CASE("this test should pass integrity test") {
	test_assert_equal(0, mod_integrity_check(test_mod_self()));
}

static void do_nothing(void) {

}

TEST_CASE("this test should fail integrity check after code modification") {
	char *ptr = (char *) do_nothing;
	char byte;

	byte = *ptr;
	*ptr = (char) ((unsigned char) byte + 1);
	test_assert_equal(1, mod_integrity_check(test_mod_self()));

	*ptr = byte;
}

TEST_CASE("this test should fail integrity check after rodata modification") {
	char *ptr = (char *) "INTEGRITY";
	char byte;

	byte = *ptr;
	*ptr = (char) ((unsigned char) byte + 1);
	test_assert_equal(1, mod_integrity_check(test_mod_self()));

	*ptr = byte;
}
