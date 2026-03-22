#include <math.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("fmax() tests");

TEST_CASE("Test for fmax(5, 3)") {
	test_assert(fmax(5, 3) == 5);
}

TEST_CASE("Test for fmax(3, 5)") {
	test_assert(fmax(3, 5) == 5);
}

TEST_CASE("Test for fmax(5, 5)") {
	test_assert(fmax(5, 5) == 5);
}

TEST_CASE("Test for fmax(-8, -3)") {
	test_assert(fmax(-8, -3) == -3);
}

TEST_CASE("Test for fmax(-3, -8)") {
	test_assert(fmax(-3, -8) == -3);
}

TEST_CASE("Test for fmax(-10, 5)") {
	test_assert(fmax(-10, 5) == 5);
}

TEST_CASE("Test for fmax(5, -10)") {
	test_assert(fmax(5, -10) == 5);
}

TEST_CASE("Test for fmax(0.0, -0.0)") {
	test_assert(fmax(0.0, -0.0) == 0.0 && !signbit(fmax(0.0,-0.0)));
}

TEST_CASE("Test for fmax(-0.0, 0.0)") {
	test_assert(fmax(-0.0, 0.0) == 0.0 && !signbit(fmax(-0.0,0.0)));
}

TEST_CASE("Test for fmax(0.0, 0.0)") {
	test_assert(fmax(0.0, 0.0) == 0.0);
}

TEST_CASE("Test for fmax(1.5, 2.7)") {
	test_assert(fmax(1.5, 2.7) == 2.7);
}

TEST_CASE("Test for fmax(2.7, 1.5)") {
	test_assert(fmax(2.7, 1.5) == 2.7);
}

TEST_CASE("Test for fmax(5.0, NAN)") {
	test_assert(fmax(5.0, NAN) == 5.0);
}

TEST_CASE("Test for fmax(NAN, 5.0)") {
	test_assert(fmax(NAN, 5.0) == 5.0);
}

TEST_CASE("Test for fmax(NAN, NAN)") {
	test_assert(isnan(fmax(NAN, NAN)));
}

TEST_CASE("Test for fmax(5.0, +INFINITY)") {
	test_assert(isinf(fmax(5.0, INFINITY)) && !signbit(fmax(5.0,INFINITY)));
}

TEST_CASE("Test for fmax(+INFINITY, 5.0)") {
	test_assert(isinf(fmax(INFINITY, 5.0)) && !signbit(fmax(5.0,INFINITY)));
}

TEST_CASE("Test for fmax(-INFINITY, 5.0)") {
	test_assert(fmax(-INFINITY, 5.0) == 5.0);
}

TEST_CASE("Test for fmax(5.0, -INFINITY)") {
	test_assert(fmax(5.0, -INFINITY) == 5.0);
}

TEST_CASE("Test for fmax(+INFINITY, +INFINITY)") {
	test_assert(isinf(fmax(INFINITY, INFINITY)));
}

TEST_CASE("Test for fmax(-INFINITY, -INFINITY)") {
	test_assert(isinf(fmax(-INFINITY, -INFINITY)) && signbit(fmax(-INFINITY, -INFINITY)));
}
