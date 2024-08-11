/**
 * @file
 *
 * @date Jun 8, 2023
 * @author Anton Bondarev
 */

#include <math.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("fmod() tests");

TEST_CASE("Test for fmod(98860, 10)") {
	test_assert(fmod(98860, 10) == 0.0);
}

TEST_CASE("Test for fmod(1.25, 0.5)") {
	test_assert(fmod(1.25, 0.5) == 0.25);
}

TEST_CASE("Test for fmod(-5.25, -3.0) == -2.25") {
	test_assert(fmod(-5.25, -3.0) == -2.25);
}

TEST_CASE("Test for isnan(fmod(NAN, 1.0))") {
	test_assert(isnan(fmod(NAN, 1.0)));
}

TEST_CASE("Test for isnan(fmod(1.0, NAN))") {
	test_assert(isnan(fmod(1.0, NAN)));
}

TEST_CASE("Test for isnan(fmod(NAN, NAN))") {
	test_assert(isnan(fmod(NAN, NAN)));
}

TEST_CASE("Test for isnan(fmod(INFINITY, 1.0))") {
	test_assert(isnan(fmod(INFINITY, 1.0)));
}

TEST_CASE("Test for isnan(fmod(1.0, 0))") {
	test_assert(isnan(fmod(1.0, 0)));
}

TEST_CASE("Test for fmod(+0.0, 1.0) == 0") {
	test_assert(fmod(+0.0, 1.0) == 0);
}

TEST_CASE("Test for fmod(-0.0, 1.0) == -0") {
	test_assert(fmod(-0.0, 1.0) == -0);
}
