/**
 * @file
 * @brief Tests semaphore enter/leave methods.
 *
 * @date Sep 03, 2012
 * @author Bulychev Anton
 */

#include <embox/test.h>
#include <kernel/thread/sync/semaphore.h>
#include <kernel/thread.h>
#include <util/err.h>

static struct thread *low, *mid, *high, *l_low, *h_high;
static struct sem s;

EMBOX_TEST_SUITE("Semaphore test");

static void *low_run(void *arg) {
	test_emit('a');
	semaphore_enter(&s);
	test_emit('b');
	test_assert_zero(thread_launch(mid));
	test_emit('j');
	semaphore_leave(&s);
	test_emit('k');
	return NULL;
}

static void *mid_run(void *arg) {
	test_emit('c');
	semaphore_enter(&s);
	test_emit('d');
	test_assert_zero(thread_launch(high));
	test_emit('f');
	semaphore_leave(&s);
	test_emit('i');
	return NULL;
}

static void *high_run(void *arg) {
	test_emit('e');
	semaphore_enter(&s);
	test_emit('g');
	semaphore_leave(&s);
	test_emit('h');
	return NULL;
}

TEST_CASE("General") {
	int l = 200, m = 210, h = 220;

	semaphore_init(&s, 2);

	low = thread_create(THREAD_FLAG_SUSPENDED, low_run, NULL);
	test_assert_zero(ptr2err(low));

	mid = thread_create(THREAD_FLAG_SUSPENDED, mid_run, NULL);
	test_assert_zero(ptr2err(mid));

	high = thread_create(THREAD_FLAG_SUSPENDED, high_run, NULL);
	test_assert_zero(ptr2err(high));

	test_assert_zero(schedee_priority_set(&low->schedee, l));
	test_assert_zero(schedee_priority_set(&mid->schedee, m));
	test_assert_zero(schedee_priority_set(&high->schedee, h));

	test_assert_zero(thread_launch(low));
	test_assert_zero(thread_join(low, NULL));
	test_assert_zero(thread_join(mid, NULL));
	test_assert_zero(thread_join(high, NULL));
	test_assert_emitted("abcdefghijk");
}

static void *h_high_run(void *arg) {
	struct timespec time;
	clock_gettime(CLOCK_REALTIME, &time);
	test_assert_not_zero(semaphore_timedwait(&s, &time));
	clock_gettime(CLOCK_REALTIME, &time);
	time.tv_nsec += 10000000;
	test_assert_zero(semaphore_timedwait(&s, &time));
	return NULL;
}

static void *l_low_run(void *arg) {
	semaphore_enter(&s);
	test_assert_zero(thread_launch(h_high));
	semaphore_leave(&s);
	return NULL;
}

TEST_CASE("Correctness of semaphore_timedwait") {
	int ll = 200, hh = 220;

	semaphore_init(&s, 1);

	l_low = thread_create(THREAD_FLAG_SUSPENDED, l_low_run, NULL);
	test_assert_zero(ptr2err(l_low));

	h_high = thread_create(THREAD_FLAG_SUSPENDED, h_high_run, NULL);
	test_assert_zero(ptr2err(h_high));

	test_assert_zero(schedee_priority_set(&l_low->schedee, ll));
	test_assert_zero(schedee_priority_set(&h_high->schedee, hh));
	test_assert_zero(thread_launch(l_low));

	test_assert_zero(thread_join(l_low, NULL));
	test_assert_zero(thread_join(h_high, NULL));
}
