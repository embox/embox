/**
 * @file
 *
 * @date May 18, 2019
 * @author Anton Bondarev
 */

#include <embox/test.h>

#include <string.h>
#include <time.h>

EMBOX_TEST_SUITE("time/time");

TEST_CASE("time(NULL)") {
	time(NULL);
}
