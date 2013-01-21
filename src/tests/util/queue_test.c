/**
 * @file
 * @brief Tests methods of util/queue
 *
 * @date 21.01.13
 * @author Ilia Vaprol
 */

#include <embox/test.h>
#include <util/queue.h>

EMBOX_TEST_SUITE("util/queue test");

static struct queue *q;
static int items[] = { 0, 2, 3, 4, 5, 6, 10 };

TEST_CASE("queue_create should return not null") {
	test_assert_not_null((q = queue_create()));
}

TEST_CASE("queue_create should return empty queue") {
	test_assert_not_zero(queue_empty(q));
	test_assert_equal(queue_size(q), 0);
}

TEST_CASE("after inserting n elements, queue contains n elements") {
	size_t i;

	for (i = 0; i < sizeof items / sizeof items[0]; ++i) {
		test_assert_zero(queue_push(q, (void *)items[i]));
	}

	test_assert_false(queue_empty(q));
	test_assert_equal(queue_size(q), sizeof items / sizeof items[0]);

	for (i = 0; i < sizeof items / sizeof items[0]; ++i) {
		test_assert_false(queue_empty(q));
		queue_pop(q);
	}

	test_assert_true(queue_empty(q));
	test_assert_equal(queue_size(q), 0);
}

TEST_CASE("queue_front should return first element, "
		"queue_back should return last element") {
	size_t i;

	for (i = 0; i < sizeof items / sizeof items[0]; ++i) {
		test_assert_zero(queue_push(q, (void *)items[i]));
		test_assert_equal(queue_front(q), (void *)items[0]);
		test_assert_equal(queue_back(q), (void *)items[i]);
	}

	for (i = 0; i < sizeof items / sizeof items[0]; ++i) {
		queue_pop(q);
	}
}

TEST_CASE("queue_pop should remove first element") {
	size_t i;

	for (i = 0; i < sizeof items / sizeof items[0]; ++i) {
		test_assert_zero(queue_push(q, (void *)items[i]));
	}

	for (i = 0; i < sizeof items / sizeof items[0]; ++i) {
		test_assert_equal(queue_front(q), (void *)items[i]);
		queue_pop(q);
	}
}

TEST_CASE("after queue_pop queue shouldn't contain anything") {
	test_assert_true(queue_empty(q));
	queue_destroy(q);
}
