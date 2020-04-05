/**
 * @file
 *
 * @date Mar 31, 2020
 * @author: Prajawal Kumar Pandey
 */

#include <string.h>
#include <stdlib.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("test suite for strncat()");

TEST_CASE("concatenate full string with strncat()") {
    char src[] = "string";
    char dst[] = "char ";

    strncat(dst, src, sizeof(src));

    test_assert_zero(strcmp(dst, "char string"));
}

TEST_CASE("concatenate empty string with strncat()") {
    char src[] = "";
    char dst[] = "char ";

    strncat(dst, src, sizeof(src));
    test_assert_zero(strcmp(dst, "char "));
}

TEST_CASE("concatenate to an empty string with strncat()") {
    char src[] = "string";
    char dst[] = "";

    strncat(dst, src, sizeof(src));

    test_assert_zero(strcmp(dst, src));
}

TEST_CASE("concatenate part of a string with strncat()") {
    char src[] = "string";

    char dst[] = "char ";

    strncat(dst, src, 3);
    test_assert_zero(strcmp(dst, "char str"));
    test_assert_equal(dst[8], '\0');
}

TEST_CASE("check for zero argument length with strncat()") {
    char src[] = "string";

    char dst[] = "char ";

    strncat(dst, src, 0);
    test_assert_zero(strcmp(dst, "char "));
    test_assert_equal(dst[5], '\0');
}

TEST_CASE("concatenate for length greater than size of src string with strncat()") {
    char src[] = "string";
    char dst[] = "char ";

    strncat(dst, src, sizeof(src)+5);
    test_assert_zero(strcmp(dst, "char string"));
}

