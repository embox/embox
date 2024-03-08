/**
 * @file
 * @brief Tests condition variable wait/signal methods.
 *
 * @date Sep 03, 2012
 * @author Bulychev Anton
 */

#include <embox/test.h>
#include <kernel/thread/sync/cond.h>
#include <kernel/thread/sync/mutex.h>
#include <kernel/thread.h>
#include <kernel/task.h>
#include <sys/wait.h>
#include <util/err.h>

static struct thread *low, *high;
static cond_t c, c_private;
static struct mutex m;

EMBOX_TEST_SUITE("Condition variable test");

static void *low_run(void *arg) {
	test_emit('a');
	test_assert_zero(thread_launch(high));
	test_emit('d');
	mutex_lock(&m);
	test_emit('e');
	cond_signal(&c);
	test_emit('f');
	mutex_unlock(&m);
	test_emit('h');
	return NULL;
}

static void *high_run(void *arg) {
	test_emit('b');
	mutex_lock(&m);
	test_emit('c');
	cond_wait(&c, &m);
	test_emit('g');
	return NULL;
}

TEST_CASE("General") {
	int l = 200, h = 210;

	mutex_init(&m);
	cond_init(&c, NULL);

	low = thread_create(THREAD_FLAG_SUSPENDED, low_run, NULL);
	test_assert_zero(ptr2err(low));

	high = thread_create(THREAD_FLAG_SUSPENDED, high_run, NULL);
	test_assert_zero(ptr2err(high));

	test_assert_zero(schedee_priority_set(&low->schedee, l));
	test_assert_zero(schedee_priority_set(&high->schedee, h));

	test_assert_zero(thread_launch(low));
	test_assert_zero(thread_join(low, NULL));
	test_assert_zero(thread_join(high, NULL));
	test_assert_emitted("abcdefgh");
}

static void * try_signal_private(void *unused) {
	test_assert_not_zero(cond_signal(&c_private));
	return NULL;
}

static void * try_signal_shared(void *unused) {
	test_assert_zero(cond_signal(&c));
	return NULL;
}

TEST_CASE("PROCESS_PRIVATE") {
	int p1, p2;

	cond_init(&c_private, NULL);
	test_assert(0 <= (p1 = new_task("", try_signal_private, NULL)));
	cond_init(&c, NULL);
	condattr_setpshared(&c.attr, PROCESS_SHARED);
	test_assert(0 <= (p2 = new_task("", try_signal_shared, NULL)));
	waitpid(p1, NULL, 0);
	waitpid(p2, NULL, 0);
}
