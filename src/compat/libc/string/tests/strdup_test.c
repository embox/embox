/**
 * @file
 *
 * @date Aug 28, 2013
 * @author: Anton Bondarev
 */

#include <string.h>
#include <stdlib.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("test suit for strndup()");

TEST_CASE("duplicate full string with strndup()") {
	char src[] = "string";
	char *dst;

	dst = strndup(src, sizeof(src));
	test_assert_not_null(dst);
	test_assert_zero(strcmp(dst, src));
	free(dst);
}

TEST_CASE("duplicate part of a string with strndup()") {
	char src[] = "string";
	const int prefix_len = 3;
	char *dst;
	int i;

	/* performing allocation many times causes more chances to get error,
 	 * esspecially when allocating from heap.
	 */
	for (i = 0; i < 5; i++) {
		dst = strndup(src, prefix_len);
		test_assert_not_null(dst);
		test_assert_zero(strncmp(dst, src, prefix_len));
		test_assert_equal(dst[prefix_len], '\0');
		free(dst);
	}
}
