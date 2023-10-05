/**
 * @file
 *
 * @date June 30, 2020
 * @author: KOUAM Ledoux
 */

#include <string.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("memmove test suit");

TEST_CASE("move full data") {

    const char src[] = "string";
    char dest[strlen(src)+1];

    test_assert_equal(strlen(src), \
    strlen(memmove(dest, src, strlen(src))));

    test_assert_zero(strcmp(dest, src));
}

TEST_CASE("Check for null argument") {

    char src[] = "\0";
    char dest[strlen(src)+1];

    test_assert_zero(strlen(memmove(dest, src, strlen(src))));
}

TEST_CASE("move array") {

    char src[] = {'s','t','r','i','n','g'};
    char dest[strlen(src)+1];

    test_assert_equal(strlen(src), \
    strlen(memmove(dest, src, strlen(src))));

    test_assert_zero(strcmp(dest, src));
}

TEST_CASE("Move array with null value in the middle") {

    char src[] = {'s','t','r','\0','i','n','g','\0'};
    char dest[strlen(src)+1];

    test_assert_equal(strlen(src), \
    strlen(memmove(dest, src, strlen(src))));

    test_assert_zero(strcmp(dest, src));

    test_assert_equal(strlen(src+4), \
    strlen(memmove(dest+4, src+4, strlen(src+4))));

    test_assert_zero(strcmp(dest+4, src+4));
}

TEST_CASE("Memory is not allocated enough") {

    char src[] = {'s','t','r','i','n','g'};

    char dest[2] = "OK";

    test_assert_equal(strlen(dest), strlen(memmove(dest, src, strlen(dest))));
}
