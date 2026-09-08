/**
 * @file
 * @brief Is a finished thread always joined?
 *
 * One boot in 194 never reached the shell: the log ends at `running
 * embox.test.kernel.thread.sync.mutex_test' with no exception, no assertion
 * and no further line. What mutex_test does after that print is two
 * thread_join() calls, and a join is an unbounded wait -- so a wakeup that
 * never arrives is exactly a boot that ends there, silently, for ever.
 *
 * The same area has failed before: thread_join() on a schedee that had
 * already been released, found in round 62 of an acceptance hour. That one
 * was fixed. Whether anything is left is what this asks, and it asks it
 * thousands of times per boot instead of the twice mutex_test manages.
 *
 * The shapes that matter are mixed on purpose, because a join races the
 * thread it is joining and which of the two wins is what decides:
 *
 *   - a thread that exits long before the join (launch, then a delay)
 *   - a thread that exits long after it (join first, thread sleeps)
 *   - a thread that exits at about the moment of the join, which is the
 *     interesting one and cannot be aimed at directly -- so it is repeated
 *   - joiner and joinee on the same core, and on different ones
 *
 * If a wakeup is lost the join does not return and the watchdog names the
 * round it stopped in. A hang that says where it is worth more than a tidy
 * failure.
 */

#include <unistd.h>

#include <embox/test.h>
#include <framework/test/api.h>
#include <kernel/printk.h>
#include <kernel/thread/sync/mutex.h>
#include <kernel/sched/affinity.h>
#include <kernel/thread.h>
#include <kernel/time/ktime.h>
#include <util/err.h>

EMBOX_TEST_SUITE("a finished thread is always joined");

#define ROUNDS   400
#define WORKERS  4
#define STALL_MS 2000

static volatile unsigned long rounds_done;
static volatile int watch_stop;
static volatile int watch_stalled;

static void *worker_immediate(void *arg) {
	(void)arg;
	return NULL;
}

static void *worker_brief(void *arg) {
	(void)arg;
	usleep(200);
	return NULL;
}

/* Separate thread, because the one being watched is blocked inside join().
 *
 * It sleeps in slices rather than one STALL_MS nap, so that stopping it is
 * prompt. It was one nap at first, and thread_join() on it then added up to a
 * whole STALL_MS to the measured time -- which showed up as one run in 267
 * taking 4006 ms against 2001 ms for the rest, and looked like a finding for
 * as long as it took to read the code. The elapsed time is taken before the
 * watchdog is stopped now as well; both halves of that mistake are worth not
 * repeating. */
#define WATCH_SLICE_MS 50

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
			printk("join_smp: no round finished in %d ms, stuck after %lu\n",
			    STALL_MS, rounds_done);
		}
		seen = rounds_done;
	}
	return NULL;
}

TEST_CASE("threads created and joined in a tight loop always come back") {
	struct thread *w[WORKERS];
	struct thread *watch;
	uint64_t t0, block_ns;
	unsigned long took_ms;
	int r, i;

	rounds_done = 0;
	watch_stop = 0;
	watch_stalled = 0;

	watch = thread_create(0, watchdog_run, NULL);
	test_assert_zero(ptr2err(watch));

	t0 = block_ns = ktime_get_ns();
	for (r = 0; r < ROUNDS; r++) {
		for (i = 0; i < WORKERS; i++) {
			/* Half exit at once, half after a short sleep, so the join
			 * lands on both sides of the exit. */
			w[i] = thread_create(THREAD_FLAG_SUSPENDED,
			    (r & 1) ? worker_immediate : worker_brief, NULL);
			test_assert_zero(ptr2err(w[i]));
			if (r % 4 == 0) {
				/* Every fourth round, pin the workers to one core and leave
				 * the joiner wherever it is: a wakeup that has to cross
				 * cores is the one at risk. */
				sched_affinity_set(&w[i]->schedee.affinity, 1);
			}
		}
		for (i = 0; i < WORKERS; i++) {
			test_assert_zero(thread_launch(w[i]));
		}
		for (i = 0; i < WORKERS; i++) {
			test_assert_zero(thread_join(w[i], NULL));
		}
		rounds_done = r + 1;

		/* Printed per block rather than once at the end, because the
		 * interesting number is not the total: it is whether the Nth block
		 * costs more than the first. thread_create() allocates a stack out of
		 * the task mspace, and mspace_do_alloc() walks every segment it has
		 * for every allocation -- so a run that creates and returns thousands
		 * of threads can leave the allocator slow enough that a later creation
		 * takes SECONDS. Measured here because "it stopped" and "it is inside
		 * malloc" look the same from outside. */
		if ((r + 1) % 100 == 0) {
			uint64_t now = ktime_get_ns();

			printk("join_smp: rounds %d..%d took %lu ms\n", r - 98, r + 1,
			    (unsigned long)((now - block_ns) / 1000000ull));
			block_ns = now;
		}
	}

	/* Before the watchdog is stopped: what is being measured is the rounds,
	 * not how long a sleeping thread takes to notice a flag. */
	took_ms = (unsigned long)((ktime_get_ns() - t0) / 1000000ull);

	watch_stop = 1;
	thread_join(watch, NULL);

	printk("join_smp: %d rounds of %d threads created, launched and joined "
	       "in %lu ms\n",
	    ROUNDS, WORKERS, took_ms);

	test_assert_equal(watch_stalled, 0);
	test_assert_equal(rounds_done, (unsigned long)ROUNDS);
}
