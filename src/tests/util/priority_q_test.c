/**
 * @file
 * @brief Test for priority queue
 *
 * @author Nikolay Malkovsky
 * @date 12.04.12
 */

#include <embox/test.h>
#include <util/priority_q.h>

EMBOX_TEST_SUITE("util/priority_q test");

int cmp(void *a, void *b) {
	return (int)a < (int)b;
}

TEST_CASE("General test for priority queue") {
	int i;
	CREATE_PRIORITY_QUEUE(pq, int, 32, cmp);

	for(i = 0; i < 32; ++i) {
		priority_q_insert(pq, (void*)(32 - i));
	}

	for(i = 0; i < 32; ++i) {
		test_assert_equal((int)(priority_q_front(pq)), i + 1);
		priority_q_pop(pq);
	}
}
