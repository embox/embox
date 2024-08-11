/**
 * @file
 *
 * @date July 07, 2024
 * @author Ankith Veldandi
 */

#include <math.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("round() tests");

TEST_CASE("Test for round(0.23)") {
	test_assert(round(0.23) == 0.0);
}

TEST_CASE("Test for round(0.73)") {
	test_assert(round(0.73) == 1.0);
}

TEST_CASE("Test for round(0.0)") {
	test_assert(round(0.0) == 0.0);
}

TEST_CASE("Test for round(3.5)") {
	test_assert(round(3.5) == 4.0);
}

TEST_CASE("Test for round(-3.5)") {
	test_assert(round(-3.5) == -4.0);
}

TEST_CASE("Test for round(+INFINITY)") {
	test_assert(isinf(round(INFINITY)));
}

TEST_CASE("Test for round(NAN)") {
	test_assert(isnan(round(NAN)));
}
