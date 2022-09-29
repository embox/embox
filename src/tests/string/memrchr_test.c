#include <string.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("string/memrchr_test");

TEST_CASE("find a existed character with memrchr()") {
    /* Source string */
    char src[] = "ABCCDEF";

    /* Checking that a character is found */
    test_assert_equal(memrchr(src, 'C', sizeof(src)), src + 3);
}

TEST_CASE("find a not existed character with memrchr()") {
    /* Source string */
    char src[] = "ABCCDEF";

    /* Checking that a character is not found */
    test_assert_null(memrchr(src, 1, sizeof(src)));
}
