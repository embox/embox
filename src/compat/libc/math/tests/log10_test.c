#include <embox/test.h>
#include <math.h>

EMBOX_TEST_SUITE("log10() tests");


TEST_CASE("Test for log10() with negative argument") {
	test_assert(isnan(log10(-1.0)));
}

TEST_CASE("Test for log10(0.0)") {
	test_assert(log10(0.0) == -INFINITY);
}

TEST_CASE("Test for log10(-0.0)") {
	test_assert(log10(-0.0) == -INFINITY);
}

TEST_CASE("Test for log10(0.001)") {
	test_assert(log10(0.001) == -3.0);
}

TEST_CASE("Test for log10(1.0)") {
	test_assert(log10(1.0) == 0.0);
}

TEST_CASE("Test for log10(100.0)") {
	test_assert(log10(100.0) == 2.0);
}

TEST_CASE("Test for log10(+INFINITY)") {
	test_assert(isinf(log10(INFINITY)));
}

TEST_CASE("Test for log10(-INFINITY)") {
	test_assert(isnan(log10(-INFINITY)));
}

TEST_CASE("Test for log10(NaN)") {
	test_assert(isnan(log10(NAN)));
}


