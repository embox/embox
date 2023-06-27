/**
 * @file
 *
 * @date Jun 8, 2023
 * @author Anton Bondarev
 */

#include <embox/test.h>
#include <math.h>

EMBOX_TEST_SUITE("fmod() tests");

TEST_CASE("Test for fmod(98860, 10)") {
    test_assert(fmod(98860, 10) == 0.0);
}

TEST_CASE("Test for fmod(1.25, 0.5)") {
	test_assert(fmod(1.25, 0.5) == 0.25);
}

TEST_CASE("Test for fmod(+0.0, 1.0) == 0") {
	test_assert(fmod(+0.0, 1.0) == 0);
}

TEST_CASE("Test for fmod(-0.0, 1.0) == -0") {
	test_assert(fmod(-0.0, 1.0) == -0);
}
