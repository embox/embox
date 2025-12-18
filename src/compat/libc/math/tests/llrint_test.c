/**
 * @file
 *
 * @date April 05, 2025
 * @author Atharv Dubey , Biancaa Ramesh
 */

#include <math.h>
#include <fenv.h>
#include <embox/test.h>
#include <limits.h>

EMBOX_TEST_SUITE("llrint() tests");

TEST_CASE("Test for llrint(0.23)") {
	test_assert(llrint(0.23) == 0LL);
}

TEST_CASE("Test for llrint(0.73)") {
	test_assert(llrint(0.73) == 1LL);
}

TEST_CASE("Test for llrint(0.0)") {
	test_assert(llrint(0.0) == 0LL);
}

TEST_CASE("Test for llrint(3.5)") {
	test_assert(llrint(3.5) == 4LL);
}

TEST_CASE("Test for llrint(-3.5)") {
	test_assert(llrint(-3.5) == -4LL);
}

TEST_CASE("Test llrint with large values") {
	test_assert(llrint((double)LLONG_MAX) == LLONG_MAX);
	test_assert(llrint((double)LLONG_MIN) == LLONG_MIN);
}

TEST_CASE("Test llrint with FE_DOWNWARD") {
	fesetround(FE_DOWNWARD);

	test_assert(llrint(2.9) == 2LL);
	test_assert(llrint(-2.1) == -3LL);
}

TEST_CASE("Test llrint with FE_UPWARD") {
	fesetround(FE_UPWARD);

	test_assert(llrint(2.1) == 3LL);
	test_assert(llrint(-2.9) == -2LL);
}

