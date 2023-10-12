#include <embox/test.h>
#include <math.h>

EMBOX_TEST_SUITE("exp() tests");

TEST_CASE("Test for exp(0.0)") {
	test_assert(exp(0.0) == 1.0);
}

TEST_CASE("Test for exp(-0.0)") {
	test_assert(exp(-0.0) == 1.0);
}

TEST_CASE("Test for exp(+INFINITY)") {
	test_assert(isinf(exp(INFINITY)));
}

TEST_CASE("Test for exp(NaN)") {
	test_assert(isnan(exp(NAN)));
}

