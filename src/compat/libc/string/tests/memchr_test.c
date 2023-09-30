#include <string.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("string/memchr_test");

TEST_CASE("find a existed character with memchr()") {
    /* Source string */
    char src[] = "ABCCDEF";

    /* Checking that a character is found */
    test_assert_equal(memchr(src, 'C', sizeof(src)), src + 2);
}

TEST_CASE("find a not existed character with memchr()") {
    /* Source string */
    char src[] = "ABCCDEF";

    /* Checking that a character is not found */
    test_assert_null(memchr(src, 1, sizeof(src)));
}
