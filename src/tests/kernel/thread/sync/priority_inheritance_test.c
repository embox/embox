/**
 * @file
 * @brief Tests thread priority inheritance.
 *
 * @details The test creates three threads: with low, mid and high priorities.
 *      All of them is created with (suspend) flag THREAD_FLAG_SUSPENDED. Than
 *      low priority thread is waked up and it is executing while it will not
 *      finish. The test checks a correct sequence of stages.
 *
 * @date 01.05.11
 * @author Alina Kramar
 */

#include <string.h>
#include <unistd.h>

#include <embox/test.h>
#include <framework/test/thread_pin.h>

#include <kernel/printk.h>
#include <kernel/sched/schedee_priority.h>
#include <kernel/thread.h>
#include <util/err.h>
#include <kernel/thread/sync/mutex.h>

EMBOX_TEST_SUITE("Priority inheritance for mutex");

/* An order test that fails without saying what order it got is the
 * same as an assertion that says nothing -- and the order IS the result here.
 * Print it, then assert on it. */
static void expect_emitted(const char *want) {
	const char *got = test_get_emitted();

	if (0 != strcmp(got, want)) {
		printk("priority_inheritance: emitted \"%s\", wanted \"%s\"\n", got,
		    want);
	}

	test_assert_emitted(want);
}

static void *low_run(void *);
static void *mid_run(void *);
static void *high_run(void *);

static void *queued_low_run(void *);
static void *queued_mid_run(void *);
static void *queued_high_run(void *);

static struct thread *low;
static struct thread *mid;
static struct thread *high;

TEST_CASE("with inheritance") {
	struct mutex mutex;
	int l = 200, m = 210, h = 220;

	mutex_init(&mutex);

	low = thread_create(THREAD_FLAG_SUSPENDED, low_run, &mutex);
	test_assert_zero(ptr2err(low));

	test_assert_not_null(low);
	test_assert_zero(schedee_priority_set(&low->schedee, l));

	mid = thread_create(THREAD_FLAG_SUSPENDED, mid_run, &mutex);
	test_assert_zero(ptr2err(mid));

	test_assert_not_null(mid);
	test_assert_zero(schedee_priority_set(&mid->schedee, m));

	high = thread_create(THREAD_FLAG_SUSPENDED, high_run, &mutex);
	test_assert_zero(ptr2err(high));

	test_assert_not_null(high);
	test_assert_zero(schedee_priority_set(&high->schedee, h));

	/* This one passes unpinned, because the mutex it is about makes its three
	 * threads take turns anyway. That is luck, not design -- the assertion is
	 * still an exact sequence. See test_thread_pin(). */
	test_thread_pin(low);
	test_thread_pin(mid);
	test_thread_pin(high);

	thread_launch(low);

	test_assert_zero(thread_join(low, NULL));
	test_assert_zero(thread_join(mid, NULL));
	test_assert_zero(thread_join(high, NULL));

	expect_emitted("abcdefghijk");
}

static void *low_run(void *arg) {
	struct mutex *m = (struct mutex *) arg;

	test_emit('a');

	mutex_lock(m);

	test_emit('b');

	thread_launch(high);
	test_emit('d');
	thread_launch(mid);

	test_emit('e');

	mutex_unlock(m);

	test_emit('k');

	return NULL;
}

static void *mid_run(void *arg) {
	test_emit('i');
	test_emit('j');

	return NULL;
}

static void *high_run(void *arg) {
	struct mutex *m = (struct mutex *) arg;

	test_emit('c');

	mutex_lock(m);

	test_emit('f');
	test_emit('g');

	mutex_unlock(m);

	test_emit('h');

	return NULL;
}

/* The half the case above does not cover.
 *
 * There, the mutex holder is RUNNING when the waiter arrives, and raising its
 * priority in place is enough -- the scheduler reads the number off the
 * schedee when it next files it. Here the holder is PREEMPTED, sitting in the
 * run queue with the mutex in its hands, which is the case priority
 * inheritance exists for: the holder cannot give the mutex back until it runs,
 * and it will not run until it is boosted.
 *
 * The order the three threads produce is the whole test, and the two answers
 * differ in one place:
 *
 *   inheritance moves the holder   a b i c | e f g h | j | k
 *   inheritance only writes a number  a b i c | j | e f g h | k
 *
 * -- with `j' before `e' in the second, because `mid' keeps the CPU it took
 * from a holder that was never refiled.
 *
 * One core on purpose: an order between threads only exists while they take
 * turns. See test_thread_pin().
 */
TEST_CASE("with inheritance while the holder waits in the run queue") {
	struct mutex mutex;
	int l = 200, m = 210, h = 220;

	mutex_init(&mutex);

	low = thread_create(THREAD_FLAG_SUSPENDED, queued_low_run, &mutex);
	test_assert_zero(ptr2err(low));
	test_assert_zero(schedee_priority_set(&low->schedee, l));

	mid = thread_create(THREAD_FLAG_SUSPENDED, queued_mid_run, &mutex);
	test_assert_zero(ptr2err(mid));
	test_assert_zero(schedee_priority_set(&mid->schedee, m));

	high = thread_create(THREAD_FLAG_SUSPENDED, queued_high_run, &mutex);
	test_assert_zero(ptr2err(high));
	test_assert_zero(schedee_priority_set(&high->schedee, h));

	test_thread_pin(low);
	test_thread_pin(mid);
	test_thread_pin(high);

	thread_launch(low);

	test_assert_zero(thread_join(low, NULL));
	test_assert_zero(thread_join(mid, NULL));
	test_assert_zero(thread_join(high, NULL));

	expect_emitted("abicefghjk");
}

static void *queued_low_run(void *arg) {
	struct mutex *m = (struct mutex *)arg;

	test_emit('a');

	mutex_lock(m);

	test_emit('b');

	/* Mid is higher, so this hands the CPU away while the mutex is held --
	 * which is the state the case is about. */
	thread_launch(mid);

	/* Reached only once somebody puts this thread back on a CPU. */
	test_emit('e');

	mutex_unlock(m);

	test_emit('k');

	return NULL;
}

static void *queued_mid_run(void *arg) {
	(void)arg;

	test_emit('i');

	/* High is higher still, so this hands the CPU away too -- and high is
	 * what makes the waiter arrive while the holder is queued. */
	thread_launch(high);

	test_emit('j');

	return NULL;
}

static void *queued_high_run(void *arg) {
	struct mutex *m = (struct mutex *)arg;

	test_emit('c');

	mutex_lock(m);

	test_emit('f');
	test_emit('g');

	mutex_unlock(m);

	test_emit('h');

	return NULL;
}

/* A waiter must not be able to LOWER the holder.
 *
 * mutex_priority_inherit() is three lines:
 *
 *     prior = schedee_priority_get(self);
 *     if (prior != schedee_priority_inherit(m->holder, prior))
 *             schedee_priority_set(m->holder, prior);
 *
 * The fallback is there for the build with no inheritance at all, where
 * schedee_priority_inherit() answers 0 and the set is the only thing that
 * happens. With inheritance built in it fires in a second case nobody meant:
 * when the holder is ALREADY higher than the waiter. Then inherit correctly
 * changes nothing, returns the holder's own priority, which differs from the
 * waiter's -- and the fallback sets the holder's BASE priority to the
 * waiter's. A low-priority thread taking a lock for a moment permanently
 * demotes whoever was holding it.
 *
 * The check is the holder's own priority after it has unlocked and its boost,
 * if any, has been given back. No ordering is asserted, so this case says the
 * same thing on one core and on four.
 */
static int demote_holder_prio;

static void *demote_holder_run(void *arg) {
	struct mutex *m = (struct mutex *)arg;

	mutex_lock(m);

	test_emit('a');

	/* Sleeping is what lets the lower-priority waiter run at all: it cannot
	 * preempt this thread, so it has to be given the CPU. */
	usleep(20000);

	mutex_unlock(m);

	demote_holder_prio = schedee_priority_get(&thread_self()->schedee);

	test_emit('c');

	return NULL;
}

static void *demote_waiter_run(void *arg) {
	struct mutex *m = (struct mutex *)arg;

	test_emit('b');

	mutex_lock(m);
	mutex_unlock(m);

	return NULL;
}

TEST_CASE("a lower-priority waiter does not demote the holder") {
	struct mutex mutex;
	int holder_prio = 220, waiter_prio = 200;

	mutex_init(&mutex);
	demote_holder_prio = -1;

	low = thread_create(THREAD_FLAG_SUSPENDED, demote_holder_run, &mutex);
	test_assert_zero(ptr2err(low));
	test_assert_zero(schedee_priority_set(&low->schedee, holder_prio));

	mid = thread_create(THREAD_FLAG_SUSPENDED, demote_waiter_run, &mutex);
	test_assert_zero(ptr2err(mid));
	test_assert_zero(schedee_priority_set(&mid->schedee, waiter_prio));

	test_thread_pin(low);
	test_thread_pin(mid);

	thread_launch(low);
	thread_launch(mid);

	test_assert_zero(thread_join(low, NULL));
	test_assert_zero(thread_join(mid, NULL));

	if (demote_holder_prio != holder_prio) {
		printk("priority_inheritance: the holder came out of the mutex at %d, "
		       "it went in at %d\n", demote_holder_prio, holder_prio);
	}
	test_assert_equal(demote_holder_prio, holder_prio);
}
