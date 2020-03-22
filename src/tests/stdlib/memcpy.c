/**
 * @file
 *
 * @date Mar 22, 2020
 * @author: Athreya Nagaraj
 */

#include <string.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("memcpy test suit");

TEST_CASE("memcpy with zero size") {
	char src[] = "string";
	char dst[sizeof(src) + 1];

	dst[0] = '\0';
	test_assert_zero(strlen(memcpy(dst, src, 0)));
}

TEST_CASE("copy string with memcpy") {
	char src[] = "string";
	char dst[sizeof(src) + 1];

	memset(dst, 0x55, sizeof(dst));

	test_assert_equal(strlen(src),\
		strlen(memcpy(dst, src, sizeof(dst))));
	test_assert_zero(strcmp(dst, src));
}

TEST_CASE("copy integer array with memcpy") {
	int src[] = {1, 2, 3, 4, 5};
	int dst[sizeof(src) / sizeof(int)];

	memset(dst, 0x55, sizeof(dst));
	memcpy(dst, src, sizeof(dst));

	test_assert_zero(memcmp(src, dst,\
		(sizeof(dst) / sizeof(int))));
}

TEST_CASE("copy char array with null in middle") {
	char src[] = {'a', 't', 'h', '\0', '-', '9', '0'};
	char dst[sizeof(src)];

	memset(dst, 0x55, sizeof(dst));
	memcpy(dst, src, sizeof(dst));
	test_assert_zero(memcmp(src, dst, sizeof(dst)));
}

TEST_CASE("copy char array without overflow") {
	char src[50];
	char dst[150];

	memset(dst, 0, sizeof(dst));
	memset(src, 0x55, sizeof(src));
	memcpy(&dst[50], src, sizeof(src));

	test_assert_zero(memcmp(src, &dst[50], sizeof(src)));

	/* Check if memcpy has not modified beyond the boundary */
	test_assert_zero(dst[49]);
	test_assert_zero(dst[100]);
}

