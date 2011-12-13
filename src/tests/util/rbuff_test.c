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
CIRCULAR_BUFFER_DEF(test_rbuff, int, 10);

TEST_CASE("Write/read single element to ring buffer") {
	int rd = 5;
	int wr = 0;

	c_buf_add(&test_rbuff, rd);
	c_buf_get(&test_rbuff, wr);
	test_assert_equal(wr, rd);
}
