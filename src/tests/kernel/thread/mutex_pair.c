/**
 * @file
 * @brief The shape mutex_test has, repeated.
 *
 * One boot in 194 never reached the shell, its log ending at
 * `running embox.test.kernel.thread.sync.mutex_test'. 480 000 plain joins
 * across 300 boots then lost none, which rules out a generic lost join
 * wakeup: at the observed rate a per-join defect would have shown about a
 * thousand times. So what is left is the SHAPE of that test rather than
 * joining as such, and this repeats the shape.
 *
 * It found one: sched_change_priority() holds rq.lock and its callback took
 * sched_lock(), which spins for the BKL, while __schedule() holds the BKL and
 * takes rq.lock. Two cores, one in each, and the machine stops with
 * interrupts off on both. This wedged 13 runs out of 13 before the fix in
 * kernel/sched/priority/inherit.c and passed 13 out of 13 after it, so it
 * stays in the battery: 200 rounds cost about 400 ms.
 */

#include <unistd.h>

#include <embox/test.h>
#include <framework/test/api.h>
#include <framework/test/thread_pin.h>
#include <kernel/printk.h>
#include <kernel/thread.h>
#include <kernel/thread/sync/mutex.h>
#include <kernel/time/ktime.h>
#include <util/err.h>
#include <util/atomic_rmw.h>

EMBOX_TEST_SUITE("the mutex_test shape, repeated");

#define STALL_MS 2000
#define WATCH_SLICE_MS 50

static volatile unsigned long rounds_done;
static volatile int watch_stop;
static volatile int watch_stalled;

/* Separate thread, because the one being watched blocks inside join(). It
 * sleeps in slices so that stopping it is prompt: one long nap made
 * thread_join() on it add up to a whole STALL_MS to the measured time, which
 * showed up as one run in 267 taking twice as long and looked like a finding
 * until the code was read. */
static void *watchdog_run(void *arg) {
	unsigned long seen = rounds_done;
	unsigned waited = 0;

	(void)arg;
	while (!watch_stop) {
		usleep(WATCH_SLICE_MS * 1000);
		waited += WATCH_SLICE_MS;
		if (waited < STALL_MS) {
			continue;
		}
		waited = 0;
		if (!watch_stop && rounds_done == seen && !watch_stalled) {
			watch_stalled = 1;
			printk("mutex_pair: no round finished in %d ms, stuck after %lu\n",
			    STALL_MS, rounds_done);
		}
		seen = rounds_done;
	}
	return NULL;
}

/* The shape, in full:
 *
 *   two threads created suspended, priorities 200 and 210, both pinned to the
 *   core the test is running on; the low one takes a mutex and launches the
 *   high one, which preempts it and blocks on that mutex -- so the mutex
 *   hands the low thread the high one's priority and takes it back at the
 *   unlock; then the boot thread joins both.
 *
 * Priority inheritance, a launch from inside another thread, a preemption
 * that lands on a held lock, and two joins from a thread that is not in the
 * pool. mutex_test does that twice per boot; this does it ROUNDS times, and
 * every other round without the pinning, so the wakeups have to cross cores.
 */
#define PAIR_ROUNDS 200

static struct mutex pair_m;
static struct thread *pair_low, *pair_high;
/* The four stages, OR-ed atomically. They were a plain `|=' at first, which
 * is a read-modify-write: on the unpinned rounds the two threads run on
 * different cores at once and one update is lost. That showed up as one round
 * in about 14 000 asserting on a stage mask that was missing a bit -- a defect
 * in this test and not in what it is testing, which is exactly the kind of
 * thing a suite hunting for a rare wedge must not add. */
static volatile int pair_stage;

static void stage(int bit) {
	atomic_rmw_or_fetch(&pair_stage, bit, __ATOMIC_RELAXED);
}

static void *pair_high_run(void *arg) {
	(void)arg;
	stage(4);
	mutex_lock(&pair_m);
	stage(8);
	mutex_unlock(&pair_m);
	return NULL;
}

static void *pair_low_run(void *arg) {
	(void)arg;
	stage(1);
	mutex_lock(&pair_m);
	thread_launch(pair_high);
	stage(2);
	mutex_unlock(&pair_m);
	return NULL;
}

TEST_CASE("the mutex_test shape, repeated, always finishes") {
	uint64_t t0 = ktime_get_ns();
	uint64_t round_ns, pinned_ns = 0, free_ns = 0;
	unsigned pinned_n = 0, free_n = 0;
	int bad_round = -1, bad_stage = 0;
	int r;

	rounds_done = 0;
	watch_stop = 0;
	watch_stalled = 0;

	{
		struct thread *watch = thread_create(0, watchdog_run, NULL);

		test_assert_zero(ptr2err(watch));

		for (r = 0; r < PAIR_ROUNDS; r++) {
			mutex_init(&pair_m);
			pair_stage = 0;

			pair_low = thread_create(THREAD_FLAG_SUSPENDED, pair_low_run, NULL);
			test_assert_zero(ptr2err(pair_low));
			pair_high = thread_create(THREAD_FLAG_SUSPENDED, pair_high_run,
			    NULL);
			test_assert_zero(ptr2err(pair_high));

			test_assert_zero(schedee_priority_set(&pair_low->schedee, 200));
			test_assert_zero(schedee_priority_set(&pair_high->schedee, 210));

			if (r & 1) {
				/* As mutex_test does it: both on this core, taking turns. */
				test_thread_pin(pair_low);
				test_thread_pin(pair_high);
			}

			round_ns = ktime_get_ns();
			test_assert_zero(thread_launch(pair_low));
			test_assert_zero(thread_join(pair_low, NULL));
			test_assert_zero(thread_join(pair_high, NULL));
			round_ns = ktime_get_ns() - round_ns;
			if (r & 1) {
				pinned_ns += round_ns;
				pinned_n++;
			}
			else {
				free_ns += round_ns;
				free_n++;
			}

			/* All four stages, so a round that finished without the
			 * inheritance actually happening is not counted as one that did.
			 * Recorded rather than asserted here: an assertion inside the
			 * loop longjmps out of the case and leaves the watchdog thread
			 * running, which then reports a stall that is only this test
			 * having given up. */
			if (pair_stage != (1 | 2 | 4 | 8)) {
				bad_round = r;
				bad_stage = pair_stage;
				break;
			}
			rounds_done = r + 1;

		}

		watch_stop = 1;
		thread_join(watch, NULL);
	}

	if (bad_round >= 0) {
		printk("mutex_pair: round %d ended with stage mask %#x, wanted %#x\n",
		    bad_round, bad_stage, 1 | 2 | 4 | 8);
	}
	test_assert_equal(bad_round, -1);

	printk("mutex_pair: %d rounds in %lu ms: %u pinned at %lu us each, "
	       "%u unpinned at %lu us each\n",
	    PAIR_ROUNDS, (unsigned long)((ktime_get_ns() - t0) / 1000000ull),
	    pinned_n, (unsigned long)(pinned_n ? pinned_ns / pinned_n / 1000 : 0),
	    free_n, (unsigned long)(free_n ? free_ns / free_n / 1000 : 0));

	test_assert_equal(watch_stalled, 0);
}
