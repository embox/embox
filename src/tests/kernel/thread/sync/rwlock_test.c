/**
 * @file
 * @brief Tests read-write lock up/down methods.
 *
 * @date 04.09.2012
 * @author Anton Bulychev
 */

#include <embox/test.h>
#include <kernel/thread/sync/rwlock.h>
#include <kernel/thread.h>
#include <util/err.h>

static struct thread *low, *mid, *high;
static rwlock_t r;

EMBOX_TEST_SUITE("Read-write lock test");

TEST_SETUP(setup);

TEST_CASE("General") {
	test_assert_zero(thread_launch(low));
	test_assert_zero(thread_join(low, NULL));
	test_assert_zero(thread_join(mid, NULL));
	test_assert_zero(thread_join(high, NULL));
	test_assert_emitted("abcdefghijk");
}

static void *low_run(void *arg) {
	test_emit('a');
	rwlock_write_up(&r);
	test_emit('b');
	test_assert_zero(thread_launch(mid));
	test_emit('d');
	rwlock_write_down(&r);
	test_emit('k');
	return NULL;
}

static void *mid_run(void *arg) {
	test_emit('c');
	rwlock_read_up(&r);
	test_emit('e');
	test_assert_zero(thread_launch(high));
	test_emit('i');
	rwlock_read_down(&r);
	test_emit('j');
	return NULL;
}

static void *high_run(void *arg) {
	test_emit('f');
	rwlock_read_up(&r);
	test_emit('g');
	rwlock_read_down(&r);
	test_emit('h');
	return NULL;
}

static int setup(void) {
	int l = 200, m = 210, h = 220;

	rwlock_init(&r);

	low = thread_create(THREAD_FLAG_SUSPENDED, low_run, NULL);
	test_assert_zero(ptr2err(low));

	mid = thread_create(THREAD_FLAG_SUSPENDED, mid_run, NULL);
	test_assert_zero(ptr2err(mid));

	high = thread_create(THREAD_FLAG_SUSPENDED, high_run, NULL);
	test_assert_zero(ptr2err(high));

	test_assert_zero(schedee_priority_set(&low->schedee, l));
	test_assert_zero(schedee_priority_set(&mid->schedee, m));
	test_assert_zero(schedee_priority_set(&high->schedee, h));

	return 0;
}
