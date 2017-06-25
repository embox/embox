/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    23.03.2015
 */

#include <net/skbuff.h>
#include <sys/uio.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("skbuff iovec operations");

TEST_CASE("whole iovec should be read") {
	char buf[16];
	struct iovec iov[] = { {
		.iov_base = "abc",
		.iov_len = 3,
	}, {
		.iov_base = "def",
		.iov_len = 4,
	} };
	int res;

	res = skb_buf_iovec(buf, sizeof(buf), iov, 2);
	test_assert_equal(res, 7);
	test_assert_str_equal(buf, "abcdef");
}

TEST_CASE("part of iovec should be read") {
	char buf[4];
	struct iovec iov[] = { {
		.iov_base = "abc",
		.iov_len = 3,
	}, {
		.iov_base = "def",
		.iov_len = 4,
	} };
	int res;

	res = skb_buf_iovec(buf, sizeof(buf), iov, 2);
	test_assert_equal(res, 4);
	test_assert_zero(strncmp(buf, "abcd", 4));
}

TEST_CASE("zero-length iovec should be skipped when reading") {
	char buf[16];
	struct iovec iov[] = { {
		.iov_base = "abc",
		.iov_len = 0,
	}, {
		.iov_base = "ghi",
		.iov_len = 4,
	} };
	int res;

	res = skb_buf_iovec(buf, sizeof(buf), iov, 2);
	test_assert_equal(res, 4);
	test_assert_str_equal(buf, "ghi");
}

TEST_CASE("whole iovec should be written") {
	char buf[2][4];
	struct iovec iov[] = { {
		.iov_base = buf[0],
		.iov_len = 4,
	}, {
		.iov_base = buf[1],
		.iov_len = 4,
	} };
	int res;

	res = skb_iovec_buf(iov, 2, "abcdef", 6);
	test_assert_equal(res, 6);
	test_assert_zero(strncmp(buf[0], "abcd", 4));
	test_assert_zero(strncmp(buf[1], "ef", 2));
}

TEST_CASE("part of iovec should be written") {
	char buf[2][4];
	struct iovec iov[] = { {
		.iov_base = buf[0],
		.iov_len = 4,
	}, {
		.iov_base = buf[1],
		.iov_len = 4,
	} };
	int res;

	res = skb_iovec_buf(iov, 2, "abcdefghijklmnop", 16);
	test_assert_equal(res, 8);
	test_assert_zero(strncmp(buf[0], "abcd", 4));
	test_assert_zero(strncmp(buf[1], "efgh", 2));
}

TEST_CASE("zero-length iovec should be skipped when writing") {
	char buf[2][4];
	struct iovec iov[] = { {
		.iov_base = buf[0],
		.iov_len = 0,
	}, {
		.iov_base = buf[1],
		.iov_len = 4,
	} };
	int res;

	res = skb_iovec_buf(iov, 2, "ab", 2);
	test_assert_equal(res, 2);
	test_assert_zero(strncmp(buf[1], "ab", 2));
}
