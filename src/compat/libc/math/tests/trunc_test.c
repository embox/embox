#include <math.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("trunc() tests");

TEST_CASE("Test for trunc(6.1)") {
	test_assert(trunc(6.1) == 6.0);
}

TEST_CASE("Test for trunc(-6.1)") {
	test_assert(trunc(-6.1) == -6.0);
}

TEST_CASE("Test for trunc(0.75)") {
	test_assert(trunc(0.75) == 0.0);
}

TEST_CASE("Test for trunc(-0.75)") {
	test_assert(trunc(-0.75) == -0.0);
}

TEST_CASE("Test for trunc(0.0)") {
	test_assert(trunc(0.0) == 0.0);
}

TEST_CASE("Test for trunc(-0.0)") {
	test_assert(trunc(-0.0) == -0.0);
}

TEST_CASE("Test for trunc(INFINITY)") {
	test_assert(isinf(trunc(INFINITY)));
}

TEST_CASE("Test for trunc(NaN)") {
	test_assert(isnan(trunc(NAN)));
}
