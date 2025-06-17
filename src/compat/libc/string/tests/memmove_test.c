/**
 * @file
 *
 * @date June 30, 2020
 * @author: KOUAM Ledoux
 */

#include <string.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("memmove test suit");

TEST_CASE("Move without memory overlapping") {
	const char src[] = "string";
	char dest[sizeof(src)];

	memmove(dest, src, sizeof(src));

	test_assert_zero(memcmp(dest, src, sizeof(src)));
}

TEST_CASE("Move with memory overlapping") {
	const char src[] = "string";
	char dest[32];

	memmove(&dest[0], src, sizeof(src));
	memmove(&dest[1], &dest[0], sizeof(src));
	memmove(&dest[2], &dest[1], sizeof(src));

	test_assert_zero(memcmp(&dest[2], src, sizeof(src)));
}
