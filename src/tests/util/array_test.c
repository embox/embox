/**
 * @file
 * @brief Tests array utilities.
 *
 * @date 14.03.11
 * @author Nikolay Korotky
 */

#include <embox/test.h>

#include <string.h>
#include <util/array.h>

EMBOX_TEST_SUITE("util/array test");

static const char *test_array[] = { "foo", "bar" };

TEST_CASE("array_static_foreach should properly iterate over an array"
		" with size known at the compile-time.") {
	char buf[5];
	int count = 0;
	const char *tmp;

	array_static_foreach(tmp, test_array) {
		strncpy(buf, tmp, sizeof(buf));
		count++;
	}

	test_assert_equal(count, 2);
}

