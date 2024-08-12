/**
 * @file
 *
 * @date July 07, 2024
 * @author Ankith Veldandi
 */

#include <math.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("tanh() tests");

TEST_CASE("Test for tanh(INFINITY)") {
	test_assert(tanh(INFINITY) == 1);
}

TEST_CASE("Test for tanh(-INFINITY)") {
	test_assert(tanh(-INFINITY) == -1);
}

TEST_CASE("Test for tanh(NAN)") {
	test_assert(isnan(tanh(NAN)));
}

TEST_CASE("Test for tanh(0)") {
	test_assert(tanh(0) == 0);
}

TEST_CASE("Test for tanh(45)") {
	test_assert(tanh(45) == 1);
}

TEST_CASE("Test for tanh(-45)") {
	test_assert(tanh(-45) == -1);
}
