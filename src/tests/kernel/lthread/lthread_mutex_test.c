/**
 * @file
 * @brief
 *
 * @author  Vita Loginova
 * @date    Jul 31, 2014
 */
#include <util/err.h>
#include <embox/test.h>
#include <kernel/sched.h>
#include <kernel/sched/waitq.h>
#include <kernel/sched/schedee_priority.h>
#include <kernel/lthread/lthread.h>
#include <kernel/thread.h>
#include <kernel/time/ktime.h>
#include <kernel/sched/sync/mutex.h>
#include <kernel/lthread/sync/mutex.h>
#include <kernel/thread/sync/mutex.h>

static struct lthread high;
static struct thread *low;
static struct mutex m;

EMBOX_TEST_SUITE("test for lthread API");

TEST_SETUP(setup);

TEST_CASE("General") {
	test_assert_zero(thread_launch(low));
	test_assert_zero(thread_join(low, NULL));
	test_assert_emitted("abcde");
}

static void *low_run(void *arg) {
	test_emit('a');
	mutex_lock(&m);
	test_emit('b');
	lthread_launch(&high);
	test_emit('c');
	mutex_unlock(&m);
	test_emit('e');

	return NULL;
}

static int high_run(struct lthread *self) {
	if (mutex_trylock_lthread(self, &m) == -EAGAIN) {
		return 0;
	}

	test_emit('d');
	mutex_unlock_lthread(self, &m);

	return 0;
}

static int setup(void) {
	int l = 200, h = 210;

	mutex_init_schedee(&m);

	low = thread_create(THREAD_FLAG_SUSPENDED, low_run, NULL);
	test_assert_zero(ptr2err(low));

	lthread_init(&high, high_run);

	test_assert_zero(schedee_priority_set(&low->schedee, l));
	test_assert_zero(schedee_priority_set(&high.schedee, h));

	return 0;
}
