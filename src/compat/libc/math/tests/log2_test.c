//
// Created by walterhackin on 13.03.25.
//

#include <math.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("log2() tests");


TEST_CASE("Testing log2(4.0)") {
	test_assert(log2(4.0) == 2.0);
}

TEST_CASE("Testing log2(8.0)") {
	test_assert(log2(8.0) == 3.0);
}

TEST_CASE("Testing log2(1024.0)") {
	test_assert(log2(1024.0) == 10.0);
}

// edge cases
TEST_CASE("Testing log2(1.0)") {
	test_assert(log2(1.0) == 0.0);
}

TEST_CASE("Testing log2(0.0)") {
	test_assert(isinf(log2(0.0)) && signbit(log2(0.0)));
}

TEST_CASE("Testing log2(-1.0)") {
	test_assert(isnan(log2(-1.0)));
}

TEST_CASE("Testing log2(Infinity)") {
	test_assert(isinf(log2(INFINITY)));
}

TEST_CASE("Testing log2(-Infinity)") {
	test_assert(isnan(log2(-INFINITY)));
}

// decimal cases
TEST_CASE("Testing log2(0.5)") {
	test_assert(log2(0.5) == -1.0);
}

TEST_CASE("Testing log2(0.25)") {
	test_assert(log2(0.25) == -2.0);
}

TEST_CASE("Testing log2(1.5)") {
	test_assert(fabs(log2(1.5) - 0.5849625) < 1e-6);
}

TEST_CASE("Testing log2(10.0)") {
	test_assert(fabs(log2(10.0) - 3.321928) < 1e-6);
}

// special values
TEST_CASE("Testing log2(NaN)") {
	test_assert(isnan(log2(NAN)));
}
