#include <string.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("string/strspn_test");


TEST_CASE("strspn() returns the length for matching prefix") {
    /* Source string */
    char src[] = "abcdef";
    char accept[] = "abc";

    /* First 3 characters 'a', 'b' and 'c ' are in string accept */
    test_assert_equal(strspn(src, accept), 3);
}


TEST_CASE("strspn() returns the 0 when first character is not in accept set") {
    /* Source string */
    char src[] = "uvwxyz";
    char accept[] = "abc";

    /* If no characters match, in string accept */
    test_assert_equal(strspn(src, accept), 0);
}


TEST_CASE("strspn() returns full string length when all characters match") {
    /* Source string */
    char src[] = "aabbcc";
    char accept[] = "abc";

    /* All characters are in accept*/
    test_assert_equal(strspn(src, accept), 6);
}


TEST_CASE("strspn() works with empty accept string") {
    /* Source string */
    char src[] = "aabbcc";
    char accept[] = "";

    /* No characters can match, so span is always 0*/
    test_assert_equal(strspn(src, accept), 0);
}



