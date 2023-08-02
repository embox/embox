/**
 * @file
 *
 * @date May 18, 2019
 * @author Anton Bondarev
 */

#include <embox/test.h>

#include <string.h>
#include <time.h>

EMBOX_TEST_SUITE("time/asctime");

#define RIGHT_STRING    "Sat May 18 16:14:56 2019\n"
TEST_CASE("asctime ") {
	const struct tm tm = {
			56,
			14,
			16,
			18,
			4,
			119,
			6,
			137,
			0
	};

	test_assert(0 == strncmp(asctime(&tm), RIGHT_STRING, sizeof(RIGHT_STRING)-1));
}
