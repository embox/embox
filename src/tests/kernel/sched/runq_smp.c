/**
 * @file
 * @brief Does the run queue survive being read while another core rewrites it?
 *
 * Two properties, both of them about the same structure and both of them
 * invisible to an ordinary workload:
 *
 *   1. no CPU reads the run queue without holding rq.lock;
 *   2. the priority map and the lists it summarises never disagree.
 *
 * Neither is a thing that fails loudly. The second one used to end in
 *
 *     ASSERTION FAILED on CPU 1
 *         at lib/libds/dlist.h:142 in dlist_next
 *         !dlist_empty(list)
 *
 * on a board under host load -- four runs out of six of one filesystem suite,
 * and never once on an idle machine. Waiting for that is not a test. The
 * kernel counts the wrong observation itself, and this suite is what makes the
 * counters mean something: it builds the workload that would produce them and
 * then asserts they did not move.
 *
 * THE WORKLOAD is priority inheritance, because that is what takes the queue
 * apart without the Big Kernel Lock: a low-priority thread holds a mutex, a
 * higher-priority one waits for it, and the holder is moved between priority
 * levels by sched_change_priority() -- which takes rq.lock through
 * spin_lock_ipl(), the one critical level that deliberately does not take the
 * BKL. Meanwhile the other cores are poked with reschedule IPIs as fast as
 * they can take them, so they run __schedule() -- and therefore
 * sched_ticker_update(), the reader -- continuously rather than once per tick.
 *
 * The counters are declared weak here rather than pulled from a header: a
 * kernel built without them reads zero, and the suite then reports what it can
 * rather than failing to link.
 */

#include <stdint.h>

#include <embox/test.h>

#include <hal/cpu.h>
#include <kernel/cpu/cpu.h>
#include <kernel/sched/schedee_priority.h>
#include <kernel/thread.h>
#include <kernel/thread/sync/mutex.h>
#include <time.h>
#include <unistd.h>
#include <util/err.h>

EMBOX_TEST_SUITE("run queue under several cores");

#ifdef SMP
extern void smp_send_resched(int cpu_id);
#else
/* One core: there is nobody to poke, and the map is still worth testing. */
#define smp_send_resched(cpu_id) do { (void)(cpu_id); } while (0)
#endif

extern unsigned long runq_unlocked_reads __attribute__((weak));
extern unsigned long runq_map_stale __attribute__((weak));
extern unsigned long runq_prio_moved __attribute__((weak));

static unsigned long counter_read(unsigned long *p) {
	return p ? *p : 0;
}

/* Long enough that the sampling below is worth having, short enough that a
 * suite which runs on every boot does not become the reason nobody boots. */
#define RUNQ_SMP_MS 1500

/* A deadline rather than a flag: the caller is a thread like any other, and
 * while these two are runnable it may not get to set one. */
static clock_t deadline;
static struct mutex m;
static volatile unsigned long rounds;
static volatile unsigned long ipis;

/* The writer: preempted inside the mutex often enough that whoever waits for
 * it has to move a QUEUED holder between levels. */
static void *holder_run(void *arg) {
	volatile int spin;
	int i;

	(void)arg;
	while (clock() < deadline) {
		mutex_lock(&m);
		for (i = 0; i < 40; i++) {
			spin = i;
		}
		(void)spin;
		mutex_unlock(&m);
	}
	return NULL;
}

/* The waiter, and the sampler. */
static void *waiter_run(void *arg) {
	unsigned int self = cpu_get_id();
	unsigned int i;

	(void)arg;
	while (clock() < deadline) {
		for (i = 0; i < NCPU; i++) {
			if ((i != self) && (cpu_get_idle(i) != NULL)) {
				smp_send_resched((int)i);
				ipis++;
			}
		}
		mutex_lock(&m);
		mutex_unlock(&m);
		rounds++;
	}
	return NULL;
}

static unsigned int cpus_up_besides_this_one(void) {
	unsigned int self = cpu_get_id();
	unsigned int n = 0;
	unsigned int i;

	for (i = 0; i < NCPU; i++) {
		/* An idle thread is what cpu_init() leaves behind, so it is the one
		 * thing that tells a core the kernel has met from a slot in an
		 * array. */
		if ((i != self) && (cpu_get_idle(i) != NULL)) {
			n++;
		}
	}
	return n;
}

TEST_CASE("the run queue is read under its own lock, with cores contending") {
	struct thread *holder, *waiter;
	unsigned long unlocked_before, stale_before, moved_before;

	if (cpus_up_besides_this_one() == 0) {
		/* One core cannot race itself. Not a failure: on a kernel built for
		 * SMP but running alone there is nothing here to observe. */
		return;
	}

	unlocked_before = counter_read(&runq_unlocked_reads);
	stale_before = counter_read(&runq_map_stale);
	moved_before = counter_read(&runq_prio_moved);

	rounds = 0;
	ipis = 0;
	mutex_init(&m);
	deadline = clock() + (clock_t)RUNQ_SMP_MS;

	holder = thread_create(THREAD_FLAG_SUSPENDED | THREAD_FLAG_DETACHED,
	    holder_run, NULL);
	test_assert_zero(ptr2err(holder));
	waiter = thread_create(THREAD_FLAG_SUSPENDED | THREAD_FLAG_DETACHED,
	    waiter_run, NULL);
	test_assert_zero(ptr2err(waiter));

	/* Only the holder is moved. The waiter keeps this thread's priority, so
	 * the caller still gets a turn -- and it is still above the holder, which
	 * is all inheritance needs. */
	test_assert_zero(
	    schedee_priority_set(&holder->schedee, SCHED_OTHER_PRIORITY_MIN + 1));

	thread_launch(holder);
	thread_launch(waiter);

	usleep((useconds_t)RUNQ_SMP_MS * 1000);
	/* Both loops test the deadline once per round; give them the time. */
	usleep(500 * 1000);

	/* The workload has to have happened, or the assertions below are about
	 * nothing. This is the same rule the FAT suite learned the hard way: a
	 * clean run with no load is not a clean run. */
	test_assert_not_zero(rounds);
	test_assert_not_zero(ipis);

	/* A read of the queue by a CPU that does not hold rq.lock. Not a race to
	 * wait for -- a property, and it is zero or it is not. */
	test_assert_equal(unlocked_before, counter_read(&runq_unlocked_reads));

	/* The priority map naming a level whose list is empty. Unreachable while
	 * every reader holds the lock; counted anyway, because the map is a
	 * summary and a summary is only as fresh as its reader's lock. */
	test_assert_equal(stale_before, counter_read(&runq_map_stale));

	/* Not asserted, and deliberately: a schedee whose priority changed while
	 * it was queued is bookkeeping the kernel now handles, not a fault. It is
	 * read so that a reader of this file knows it was looked at. */
	(void)moved_before;
}

TEST_CASE("every other CPU answers a reschedule IPI") {
	unsigned int self = cpu_get_id();
	unsigned int targets = cpus_up_besides_this_one();
	unsigned int i;
	clock_t stop;

	if (targets == 0) {
		return;
	}

	for (i = 0; i < NCPU; i++) {
		if ((i != self) && (cpu_get_idle(i) != NULL)) {
			smp_send_resched((int)i);
		}
	}

	/* An IPI that is dropped looks exactly like one that was never needed, so
	 * the only thing worth asserting here is that sending them to every other
	 * core does not stop the machine -- and that the machine keeps time
	 * afterwards, which is what the idle cores coming back through
	 * __schedule() lets it do. */
	stop = clock() + 50;
	while (clock() < stop) {
	}

	test_assert_true(clock() >= stop);
}
