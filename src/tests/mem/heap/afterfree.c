/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    13.05.2014
 */

#include <stdlib.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("afterfree");

static void mfill(char *dst, size_t len, const char *src) {
	const size_t slen = strlen(src);
	int i, n;

	n = len / slen;
	for (i = 0; i < n; i++) {
		strcpy(&dst[i * slen], src);
	}

	strncpy(dst, src, len % slen);
}

static int mcheck(const char *dst, size_t len, const char *src) {
	int i, slen, n;

	n = 0;
	slen = strlen(src);
	for (i = 0; i < len; i++) {
		if (dst[i] == src[i % slen]) {
			n ++;
		}
	}

	return n;
}

#define MLEN 256
static const char test_str[] = "abcdefgh";
TEST_CASE("free'd memory shouldn't contain old data") {
	char *mem = malloc(MLEN);
	test_assert_not_null(mem);

	mfill(mem, MLEN, test_str);
	test_assert_equal(MLEN, mcheck(mem, MLEN, test_str));

	free(mem);
	test_assert(MLEN / strlen(test_str) >= mcheck(mem, MLEN, test_str));
}

