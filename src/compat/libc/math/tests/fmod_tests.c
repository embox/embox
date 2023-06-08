/**
 * @file
 *
 * @date Jun 8, 2023
 * @author Anton Bondarev
 */

#include <embox/test.h>
#include <math.h>

EMBOX_TEST_SUITE("fmod() tests");

TEST_CASE("Test for fmod(988600, 10)") {
	test_assert(fmod(988600, 10) == 0.0);
}

TEST_CASE("Test for fmod(98860, 10)") {
	test_assert(fmod(98860, 10) == 0.0);
}
