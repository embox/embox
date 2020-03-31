/**
 * @file
 *
 * @date Mar 31, 2020
 * @author: Prajawal Kumar Pandey
 */

#include <string.h>
#include <stdlib.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("test suite for strcat()");

TEST_CASE("concatenate full string with strcat()") {
    char src[] = "string";
    char dst[] = "char ";

    strcat(dst, src);
    test_assert_not_null(dst);
    test_assert_zero(strcmp(dst, "char string"));
}

TEST_CASE("concatenate empty string with strcat()") {
    char src[] = "";
    char dst[] = "char ";
    char chk[sizeof(dst)+1];

    strcpy(chk,dst);

    strcat(dst, src);
    test_assert_not_null(dst);
    test_assert_zero(strcmp(dst, chk));
}

TEST_CASE("concatenate to empty string with strcat()") {
    char src[] = "string";
    char dst[] = "";
    
    strcat(dst, src);
    test_assert_zero(strcmp(dst, src));
}

TEST_CASE("check for error if dst isn't allocated enough memory for strcat()") {
    char src[] = "string";
    char dst[3];

    strcat(dst, src);
    test_assert_not_zero(strcmp(dst, src));
}
