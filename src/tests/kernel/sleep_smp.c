/**
 * @file
 * @brief Does a timed sleep always come back?
 *
 * Observed once on a four-core board: a loop deleting sixteen files stopped
 * after ten of them, did not reset, and left the volume clean under fsck.
 * The next boot deleted all sixteen. Nothing in the driver changed between
 * them, and why it stopped was never established.
 *
 * What that loop does thousands of times is sleep: an SD driver polls its
 * controller, and every poll that does not land inside the spin budget calls
 * usleep(1 ms). Such waits are bounded -- but bounded in NUMBER OF SLEEPS,
 * not in wall time. A sleep that never returns makes the whole operation
 * never return, with no error anywhere and the volume exactly as consistent
 * as it was: which is what that boot left behind.
 *
 * So this asks the question that needs no hardware: on several cores, does a
 * timed sleep ever fail to come back? Same shape -- many short sleeps,
 * several threads, the scheduler moving them between cores -- with a
 * watchdog, because a lost wakeup is a hang and a hang has to name itself
 * before it takes the boot down.
 *
 * The counters are printed on the way out whether it passes or not: "no sleep
 * was late" means nothing without "and this many were taken".
 */

#include <errno.h>
#include <pthread.h>
#include <unistd.h>

#include <embox/test.h>
#include <hal/cpu.h>
#include <kernel/printk.h>
#include <kernel/thread.h>
#include <kernel/time/ktime.h>
#include <util/err.h>

EMBOX_TEST_SUITE("a timed sleep comes back, on every core");

#define SLEEPERS   4
#define RUN_MS     2000
#define SLEEP_US   1000

/* A sleep is not a promise of precision: under four cores of load a 1 ms ask
 * routinely takes several. Late means late enough that no amount of ordinary
 * scheduling explains it. */
#define LATE_NS    (100ull * 1000 * 1000)

/* How long a worker may make no progress at all before it is named. A lost
 * wakeup shows here: iters stops moving and never moves again. */
#define STALL_MS   1500

struct sleeper {
	volatile unsigned long iters;
	volatile int stop;
	uint64_t max_ns;
	unsigned long late;
	int idx;
};

static struct sleeper sleepers[SLEEPERS];

/* See clock_tick.c. Weak, so a kernel built without
 * it reads nothing rather than failing to link. */
extern unsigned long clock_ticks_seen[] __attribute__((weak));

/* Which cores are still taking their timer interrupt. A sleep that never
 * returns has two possible causes and this separates them on the spot: a core
 * whose count stopped moving is not delivering the ticks that service its own
 * timer list, and every timed wait started there is stranded. A core that is
 * ticking normally while a sleep on it hangs is a different defect. */
static void report_ticks(const char *when) {
	unsigned i;

	if (&clock_ticks_seen == NULL) {
		printk("sleep_smp: %s, no per-cpu tick counter in this build\n", when);
		return;
	}
	printk("sleep_smp: %s, ticks per cpu:", when);
	for (i = 0; i < NCPU; i++) {
		printk(" %lu", clock_ticks_seen[i]);
	}
	printk("\n");
}

static void *sleeper_run(void *arg) {
	struct sleeper *s = arg;

	while (!s->stop) {
		uint64_t t0 = ktime_get_ns();
		uint64_t took;

		usleep(SLEEP_US);

		took = ktime_get_ns() - t0;
		if (took > s->max_ns) {
			s->max_ns = took;
		}
		if (took > LATE_NS) {
			s->late++;
		}
		/* Last, so that a reader of `iters' knows the numbers above it are
		 * already written. */
		s->iters++;
	}

	return NULL;
}

TEST_CASE("many short sleeps on four cores all return") {
	struct thread *t[SLEEPERS];
	unsigned long seen[SLEEPERS];
	unsigned long total = 0, late = 0;
	uint64_t worst = 0;
	clock_t deadline;
	int stalled = -1;
	int i;

	for (i = 0; i < SLEEPERS; i++) {
		sleepers[i] = (struct sleeper){.idx = i};
	}
	for (i = 0; i < SLEEPERS; i++) {
		/* Unpinned: letting the scheduler place them is part of the
		 * question, since a wakeup that crosses cores is the one at risk. */
		t[i] = thread_create(0, sleeper_run, &sleepers[i]);
		test_assert_zero(ptr2err(t[i]));
	}

	/* The watchdog is this thread. Every STALL_MS it asks each worker whether
	 * it has moved at all; the first one that has not is named, with the
	 * numbers it had reached. */
	for (i = 0; i < SLEEPERS; i++) {
		seen[i] = 0;
	}
	deadline = clock() + (clock_t)RUN_MS;
	while (clock() < deadline) {
		usleep(STALL_MS * 1000);
		for (i = 0; i < SLEEPERS; i++) {
			if (sleepers[i].iters == seen[i] && stalled < 0) {
				stalled = i;
				printk("sleep_smp: worker %d made no progress in %d ms: "
				       "%lu sleep(s) so far, longest %llu ms, %lu late\n",
				    i, STALL_MS, sleepers[i].iters,
				    (unsigned long long)(sleepers[i].max_ns / 1000000ull),
				    sleepers[i].late);
				report_ticks("at the stall");
			}
			seen[i] = sleepers[i].iters;
		}
	}

	for (i = 0; i < SLEEPERS; i++) {
		sleepers[i].stop = 1;
	}
	/* If a wakeup really was lost this join does not return, and that is the
	 * intended end: the boot stalls with the line above already printed, so
	 * whatever is watching it can take the machine apart at the moment it
	 * stopped. A hang that names itself is worth more than a tidy failure. */
	for (i = 0; i < SLEEPERS; i++) {
		thread_join(t[i], NULL);
		total += sleepers[i].iters;
		late += sleepers[i].late;
		if (sleepers[i].max_ns > worst) {
			worst = sleepers[i].max_ns;
		}
	}

	printk("sleep_smp: %lu sleep(s) of %d us over %d ms on %d thread(s); "
	       "longest %llu ms, %lu over %llu ms\n",
	    total, SLEEP_US, RUN_MS, SLEEPERS,
	    (unsigned long long)(worst / 1000000ull), late,
	    (unsigned long long)(LATE_NS / 1000000ull));

	/* Proof the path was exercised: a green result on four sleeps says
	 * nothing. */
	report_ticks("after the run");

	test_assert(total > (unsigned long)SLEEPERS * 100);
	test_assert_equal(stalled, -1);
}

/* The board's shape more exactly. The FAT driver is serialised behind one
 * mutex, and the card driver sleeps inside it: every poll that misses its spin
 * budget calls usleep(1 ms) with that mutex held. So the sleep that matters is
 * not a sleep on an idle system -- it is a sleep with three other cores queued
 * behind the lock its sleeper is holding, which is also where priority
 * inheritance gets involved.
 *
 * If a wakeup is lost here the whole thing stops, and every other thread stops
 * with it, holding a lock nobody can take. That is what boot 60 looked like
 * from the outside. */
static pthread_mutex_t held = PTHREAD_MUTEX_INITIALIZER;

static void *holder_run(void *arg) {
	struct sleeper *s = arg;

	while (!s->stop) {
		uint64_t t0, took;

		pthread_mutex_lock(&held);
		t0 = ktime_get_ns();
		usleep(SLEEP_US);
		took = ktime_get_ns() - t0;
		pthread_mutex_unlock(&held);

		if (took > s->max_ns) {
			s->max_ns = took;
		}
		if (took > LATE_NS) {
			s->late++;
		}
		s->iters++;
	}

	return NULL;
}

TEST_CASE("a sleep with a contended mutex held still returns") {
	struct thread *t[SLEEPERS];
	unsigned long seen[SLEEPERS];
	unsigned long total = 0, late = 0;
	uint64_t worst = 0;
	clock_t deadline;
	int stalled = -1;
	int i;

	for (i = 0; i < SLEEPERS; i++) {
		sleepers[i] = (struct sleeper){.idx = i};
		seen[i] = 0;
	}
	for (i = 0; i < SLEEPERS; i++) {
		t[i] = thread_create(0, holder_run, &sleepers[i]);
		test_assert_zero(ptr2err(t[i]));
	}

	deadline = clock() + (clock_t)RUN_MS;
	while (clock() < deadline) {
		usleep(STALL_MS * 1000);
		for (i = 0; i < SLEEPERS; i++) {
			if (sleepers[i].iters == seen[i] && stalled < 0) {
				stalled = i;
				printk("sleep_smp: holder %d made no progress in %d ms: "
				       "%lu sleep(s) so far, longest %llu ms\n",
				    i, STALL_MS, sleepers[i].iters,
				    (unsigned long long)(sleepers[i].max_ns / 1000000ull));
				report_ticks("at the stall");
			}
			seen[i] = sleepers[i].iters;
		}
	}

	for (i = 0; i < SLEEPERS; i++) {
		sleepers[i].stop = 1;
	}
	for (i = 0; i < SLEEPERS; i++) {
		thread_join(t[i], NULL);
		total += sleepers[i].iters;
		late += sleepers[i].late;
		if (sleepers[i].max_ns > worst) {
			worst = sleepers[i].max_ns;
		}
	}

	printk("sleep_smp: %lu sleep(s) under a contended mutex; longest "
	       "%llu ms, %lu over %llu ms\n",
	    total, (unsigned long long)(worst / 1000000ull), late,
	    (unsigned long long)(LATE_NS / 1000000ull));

	test_assert(total > (unsigned long)SLEEPERS * 20);
	test_assert_equal(stalled, -1);
}
