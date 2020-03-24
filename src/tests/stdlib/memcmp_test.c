/**
 * @file
 * @brief Test unit for string/memcmp
 *
 * @date March 18,2020
 * @author Sha Ahammed Roze
 */

 #include <embox/test.h>
 #include <strings.h>

 EMBOX_TEST_SUITE("Test suite for memcmp()");

TEST_CASE("Test that 'SWgaOtP43df2' > 'SWGaOTP12df0' ") {
    int n;
    char buf1[] = "SWgaOtP43df2";
    char buf2[] = "SWGaOTP12df0";
 
    n = memcmp(buf1, buf2, sizeof(buf1));
    test_assert(n > 0);
}

TEST_CASE("Test that 'Hello' < 'Welcome'") {
    int n;
    char buf1[] = "Hello";
    char buf2[] = "Welcome";
 
    n = memcmp(buf1, buf2, sizeof(buf1));
    test_assert(n < 0);
}

TEST_CASE("Test's that 'Welcome' and 'Welcome' are equal") {
    int n;
    char buf1[] = "Welcome";
    char buf2[] = "Welcome";
 
    n = memcmp(buf1, buf2, sizeof(buf1));
    test_assert(n == 0);
}
