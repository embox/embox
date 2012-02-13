/**
 * @file
 *
 * @brief
 *
 * @date 13.12.2011
 * @author Anton Bondarev
 */

#include <embox/test.h>
#include <util/ring_buff.h>

EMBOX_TEST_SUITE("util/ring_buff test");

#define BUFF_LENGTH (0x10)
RING_BUFFER_DEF(test_rbuff, int, BUFF_LENGTH);

TEST_CASE("Write/read single element to ring buffer") {
	int rd = 5;
	int wr = 0;

	ring_buff_enque(&test_rbuff, &rd);
	ring_buff_deque(&test_rbuff, &wr);

	test_assert_equal(wr, rd);
}

TEST_CASE("Test capacity of buffer") {
	int rd = 5;
	int wr = 0;
	int cnt = BUFF_LENGTH;

	while(-1 != ring_buff_enque(&test_rbuff, &rd)) {
		cnt --;
	}
	test_assert_zero(cnt);
	while(-1 != ring_buff_deque(&test_rbuff, &wr)) {
		cnt ++;
	}

	test_assert_equal(cnt, BUFF_LENGTH);
}
