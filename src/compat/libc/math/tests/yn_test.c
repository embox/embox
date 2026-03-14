#include <math.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("yn() tests");

TEST_CASE("Test for yn() with negative x argument") {
    test_assert(isnan(yn(0,-1.0)));
}

TEST_CASE("Test for yn() with negative x argument") {
    test_assert(isnan(yn(1,-1.0)));
}

TEST_CASE("Test for yn(0,0.0)") {
    test_assert(yn(0, 0.0) == -INFINITY);
}

TEST_CASE("Test for yn(1,0.0)") {
    test_assert(yn(1, 0.0) == -INFINITY);
}

TEST_CASE("Test for yn(0,INFINITY)") {
    test_assert(yn(0,INFINITY)==0.0);
}

TEST_CASE("Test for yn(1,INFINITY)") {
    test_assert(yn(1,INFINITY)==0.0);
}

TEST_CASE("Test for yn(0,NaN)") {
    test_assert(isnan(yn(0,NAN)));
}

TEST_CASE("Test for yn(1,NaN)") {
    test_assert(isnan(yn(1,NAN)));
}