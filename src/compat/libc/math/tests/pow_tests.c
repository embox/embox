#include <embox/test.h>
#include <math.h>

EMBOX_TEST_SUITE("pow() tests");

TEST_CASE("Test for common cases") {
	test_assert(pow(2, 2) == 4);
	test_assert(pow(3, 2) == 9);
	test_assert(pow(10, 6) == 1000000);
}

TEST_CASE("Test for number x degree 0") {
	test_assert(pow(7, 0) == 1);
	test_assert(pow(-128721, 0) == 1);
	test_assert(pow(0, 0) == 1);
	test_assert(pow(0.273, 0) == 1);
	test_assert(pow(NAN, 0) == 1);
	test_assert(pow(INFINITY, 0) == 1);
	test_assert(pow(-INFINITY, 0) == 1);
	test_assert(pow(+0, 0) == 1);
	test_assert(pow(-0, 0) == 1);
}

TEST_CASE("Test for number x degree 1") {
	test_assert(pow(7, 1) == 7);
	test_assert(pow(-128721, 1) == -128721);
	test_assert(pow(0, 1) == 0);
	test_assert(pow(0.273, 1) == 0.273);
}

TEST_CASE("Test for number x degree NAN") {
	test_assert(isnan(pow(7, NAN)));
	test_assert(isnan(pow(-128721, NAN)));
	test_assert(isnan(pow(0, NAN)));
	test_assert(isnan(pow(0.273, NAN)));
	test_assert(isnan(pow(NAN, NAN)));
	test_assert(isnan(pow(INFINITY, NAN)));
	test_assert(isnan(pow(-INFINITY, NAN)));
	test_assert(isnan(pow(+0, NAN)));
	test_assert(isnan(pow(-0, NAN)));
}

TEST_CASE("Test for number NAN and degree x, x != 0") {
	test_assert(isnan(pow(NAN, 7)));
	test_assert(isnan(pow(NAN, 1)));
	test_assert(isnan(pow(NAN, -223)));
	test_assert(isnan(pow(NAN, 0.352)));
	test_assert(isnan(pow(NAN, INFINITY)));
	test_assert(isnan(pow(NAN, -INFINITY)));
}

TEST_CASE("Test for number +-(|x| > 1) and degree +INF") {
	test_assert(isinf(pow(32, INFINITY)));
	test_assert(isinf(pow(-713, INFINITY)));
	test_assert(isinf(pow(1.12, INFINITY)));
	test_assert(isinf(pow(-6.18, INFINITY)));
}

TEST_CASE("Test for number +-(|x| > 1) and degree -INF") {
	test_assert(pow(32, -INFINITY) == +0);
	test_assert(pow(-713, -INFINITY) == +0);
	test_assert(pow(1.12, -INFINITY) == +0);
	test_assert(pow(-6.18, -INFINITY) == +0);
}

TEST_CASE("Test for number +-(|x| < 1) and degree -INF") {
	test_assert(isinf(pow(0.1, -INFINITY)));
	test_assert(isinf(pow(-0.67, -INFINITY)));
	test_assert(isinf(pow(0.9, -INFINITY)));
	test_assert(isinf(pow(-0.89, -INFINITY)));
}

TEST_CASE("Test for number +-(|x| < 1) and degree +INF") {
	test_assert(pow(0.1, INFINITY) == +0);
	test_assert(pow(-0.67, INFINITY) == +0);
	test_assert(pow(0.9, INFINITY) == +0);
	test_assert(pow(-0.89, INFINITY) == +0);
}

TEST_CASE("Test for number +-1 and degree +-INF") {
	test_assert(isnan(pow(1, INFINITY)));
	test_assert(isnan(pow(1, -INFINITY)));
	test_assert(isnan(pow(-1, INFINITY)));
	test_assert(isnan(pow(-1, -INFINITY)));
}

TEST_CASE("Test for number +0 and degree +x, x != 0, x != NAN") {
	test_assert(pow(+0, 11) == +0);
	test_assert(pow(+0, 76) == +0);
	test_assert(pow(+0, INFINITY) == +0);
}

TEST_CASE("Test for number +0 and degree -x, x != 0, x != NAN") {
	test_assert(isinf(pow(+0, -11)));
	test_assert(isinf(pow(+0, -76)));
	test_assert(isinf(pow(+0, -INFINITY)));
}

TEST_CASE("Test for number -0 and degree +x, x != 0, x != NAN, x mod 2 == 0") {
	test_assert(pow(-0, 12) == +0);
	test_assert(pow(-0, 76) == +0);
}

TEST_CASE("Test for number -0 and degree -x, x != 0, x != NAN, x mod 2 == 0") {
	test_assert(isinf(pow(-0, -12)));
	test_assert(isinf(pow(-0, -76)));
}

TEST_CASE("Test for -0 and degree x, x mod 2 != 0") {
	test_assert(pow(-0, 11) == -(pow(+0, 11)));
}

TEST_CASE("Test for number +INF and degree +x, x != 0, x != NAN") {
	test_assert(isinf(pow(INFINITY, 11)));
	test_assert(isinf(pow(INFINITY, 76)));
	test_assert(isinf(pow(INFINITY, INFINITY)));
}

TEST_CASE("Test for number +INF and degree -x, x != 0, x != NAN") {
	test_assert(pow(INFINITY, -11) == +0);
	test_assert(pow(INFINITY, -76) == +0);
	test_assert(pow(INFINITY, -INFINITY) == +0);
}

TEST_CASE("Test for number -INF and degree x") {
	test_assert(pow(-INFINITY, 11) == (pow(-0, -11)));
	test_assert(pow(-INFINITY, -7) == (pow(-0, 7)));
}

TEST_CASE("Test for number -x and degree int") {
	test_assert(pow(-7, 10) == pow(-1, 10) * pow(7, 10));
	test_assert(pow(-9, -10) == pow(-1, -10) * pow(9, -10));
	test_assert(pow(-10, 17) == pow(-1, 17) * pow(10, 17));
	test_assert(pow(-16, -17) == pow(-1, -17) * pow(16, -17));
}

TEST_CASE("Test for number -x, x != 0, x != +-INF and degree x != int") {
	test_assert(isnan(pow(-7, 3.32)));
}