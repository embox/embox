/**
 * @file
 * @brief Tests methods of util/queue.
 *
 * @date 22.01.13
 * @author Ilia Vaprol
 */

#include <alloca.h>
#include <embox/test.h>
#include <string.h>
#include <util/array.h>
#include <util/queue.h>

EMBOX_TEST_SUITE("util/queue test");

TEST_SETUP(setup);

struct element {
	int some_stuff;
	struct queue_link lnk;
};

typedef member_t(struct element, lnk) element_in_queue;

static struct element x, y, z;
static struct queue m, n;

static struct element * const xyz[] = { &x, &y, &z, NULL };
static struct element * const xy[] = { &x, &y, NULL };
static struct element * const yz[] = { &y, &z, NULL };
static struct element * const xz[] = { &x, &z, NULL };

TEST_CASE("queue_element should cast link member out to its container") {
	struct queue_link *link = &x.lnk;
	test_assert_equal(queue_element(link, struct element, lnk), &x);
}

TEST_CASE("queue_init should return its argument") {
	struct queue q;
	test_assert_equal(queue_init(&q), &q);
}

TEST_CASE("queue_link_init should return its argument") {
	struct element e;
	test_assert_equal(queue_link_init(&e.lnk), &e.lnk);
}

TEST_CASE("queue_init should have the same effect as static initializer") {
	struct queue q = QUEUE_INIT(&q);
	char *buff = alloca(sizeof q);
	memcpy(buff, &q, sizeof q);
	memset(&q, 0xA5, sizeof q); /* poison. */

	test_assert_mem_equal(buff, queue_init(&q), sizeof q);
}

TEST_CASE("queue_link_init should have the same effect as static initializer") {
	struct element e = { .lnk = QUEUE_LINK_INIT(&e.lnk), .some_stuff = 42, };
	char *buff = alloca(sizeof e.lnk);
	memcpy(buff, &e.lnk, sizeof e.lnk);
	memset(&e.lnk, 0xA5, sizeof e.lnk); /* poison. */

	test_assert_mem_equal(buff, queue_link_init(&e.lnk), sizeof e.lnk);
}

TEST_CASE("queue_is_empty should return true for just created queue") {
	test_assert_true(queue_is_empty(&m));
}

TEST_CASE("queue_front should return null for empty queue") {
	test_assert_null(queue_front(element_in_queue, &m));
}

TEST_CASE("queue_back should return null for empty queue") {
	test_assert_null(queue_back(element_in_queue, &m));
}

TEST_CASE("queue_front_link should return null for empty queue") {
	test_assert_null(queue_front_link(&m));
}

TEST_CASE("queue_back_link should return null for empty queue") {
	test_assert_null(queue_back_link(&m));
}

TEST_CASE("queue_push should make the queue non empty "
		"and the size not zero") {
	queue_push(element_in_queue, &x, &m);

	test_assert_false(queue_is_empty(&m));
	test_assert_not_zero(queue_size(&m));
}

TEST_CASE("queue_push_link should make the queue non empty") {
	queue_push_link(&x.lnk, &m);

	test_assert_false(queue_is_empty(&m));
}

TEST_CASE("queue_front_link and queue_back_link on a single element queue "
		"constructed using queue_push should return the element's link") {
	queue_push(element_in_queue, &x, &m);

	test_assert_equal(queue_front_link(&m), &x.lnk);
	test_assert_equal(queue_back_link(&m), &x.lnk);
}

TEST_CASE("queue_front_link and queue_back_link should return a new and an old "
		"element accordingly after adding a new one with queue_push") {
	test_assert_not_equal(&x, &y);

	queue_push(element_in_queue, &x, &m);
	queue_push(element_in_queue, &y, &m);

	test_assert_equal(queue_front_link(&m), &x.lnk);
	test_assert_equal(queue_back_link(&m), &y.lnk);
}

TEST_CASE("queue_pop should return null for empty queue") {
	test_assert_null(queue_pop(element_in_queue, &m));
}

TEST_CASE("queue_pop on a single element queue should return the "
		"element and make the queue empty and size zero again") {
	queue_push(element_in_queue, &x, &m);

	test_assert_equal(queue_pop(element_in_queue, &m), &x);

	test_assert_true(queue_is_empty(&m));
	test_assert_equal(queue_size(&m), 0);
}

static int setup(void) {
	queue_init(&m);
	queue_init(&n);
	queue_link_init(&x.lnk);
	queue_link_init(&y.lnk);
	queue_link_init(&z.lnk);
	return 0;
}
