/**
 * @file
 * @brief Test of bcopy() function.
 *
 * @date Mar 11, 2026
 * @author: Piyush Patle
 */

#include <string.h>
#include <strings.h>
#include <embox/test.h>

#define BCOPY_LARGE_BUF_LEN 1024

EMBOX_TEST_SUITE("bcopy test");

TEST_CASE("bcopy basic copy") {
	const char src[] = {'a', 'b', 'c', 'd'};
	char dest[sizeof(src)];

	bcopy(src, dest, sizeof(src));

	test_assert_zero(memcmp(dest, src, sizeof(src)));
}

TEST_CASE("bcopy partial copy") {
	const char src[] = {'a', 'b', 'c', 'd', 'e'};
	char dest[sizeof(src)];
	const size_t copy_len = 3;

	memset(dest, 0, sizeof(dest));
	bcopy(src, dest, copy_len);

	test_assert_zero(memcmp(dest, src, copy_len));
    test_assert_zero(memcmp(dest + copy_len,
                        "\0\0",
                        sizeof(src) - copy_len));
}

TEST_CASE("bcopy forward overlap") {
	char buf[] = {'a', 'b', 'c', 'd', 'e', 'f'};
	const char expected[] = {'a', 'b', 'a', 'b', 'c', 'd'};
	const size_t offset = 2;

	bcopy(buf, buf + offset, sizeof(buf) - offset);

	test_assert_zero(memcmp(buf, expected, sizeof(buf)));
}

TEST_CASE("bcopy backward overlap") {
	char buf[] = {'a', 'b', 'c', 'd', 'e', 'f'};
	const char expected[] = {'c', 'd', 'e', 'f', 'e', 'f'};
	const size_t offset = 2;

	bcopy(buf + offset, buf, sizeof(buf) - offset);

	test_assert_zero(memcmp(buf, expected, sizeof(buf)));
}

TEST_CASE("bcopy zero length") {
	const char src[] = {'a', 'b', 'c'};
	char dest[] = {'x', 'y', 'z'};

	bcopy(src, dest, 0);

	test_assert_equal(dest[0], 'x');
	test_assert_equal(dest[1], 'y');
	test_assert_equal(dest[2], 'z');
}

TEST_CASE("bcopy self copy") {
	char buf[] = {'a', 'b', 'c', 'd'};
	const char expected[] = {'a', 'b', 'c', 'd'};

	bcopy(buf, buf, sizeof(buf));

	test_assert_zero(memcmp(buf, expected, sizeof(buf)));
}

TEST_CASE("bcopy single byte") {
	char src[] = {'a'};
	char dest[] = {'x'};

	bcopy(src, dest, 1);

	test_assert_equal(dest[0], 'a');
}

TEST_CASE("bcopy full buffer boundary") {
	const char src[] = {'a', 'b', 'c', 'd', 'e'};
	char dest[sizeof(src)];
	const size_t n = sizeof(src);

	bcopy(src, dest, n);

	test_assert_zero(memcmp(dest, src, n));
}

TEST_CASE("bcopy minimum forward overlap") {
	char buf[] = {'a', 'b', 'c', 'd', 'e', 'f'};
	const char expected[] = {'a', 'a', 'b', 'c', 'd', 'e'};
	const size_t offset = 1;

	bcopy(buf, buf + offset, sizeof(buf) - offset);

	test_assert_zero(memcmp(buf, expected, sizeof(buf)));
}

TEST_CASE("bcopy minimum backward overlap") {
	char buf[] = {'a', 'b', 'c', 'd', 'e', 'f'};
	const char expected[] = {'b', 'c', 'd', 'e', 'f', 'f'};
	const size_t offset = 1;

	bcopy(buf + offset, buf, sizeof(buf) - offset);

	test_assert_zero(memcmp(buf, expected, sizeof(buf)));
}

TEST_CASE("bcopy large buffer") {
	static char src[BCOPY_LARGE_BUF_LEN];
	static char dest[BCOPY_LARGE_BUF_LEN];

	for (size_t i = 0; i < sizeof(src); i++) {
		src[i] = (char)(i & 0xff);
	}

	memset(dest, 0, sizeof(dest));
	bcopy(src, dest, sizeof(src));

	test_assert_zero(memcmp(dest, src, sizeof(src)));
}
