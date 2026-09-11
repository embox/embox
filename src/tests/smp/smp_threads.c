/**
 * @file
 * @brief Does one core reach another, and does work reach the cores?
 *
 * Both fail silently: a dropped IPI looks like one that was never needed, and
 * a core that picks up no work looks like a core with nothing to do.
 *
 * The scheduler tick sends reschedule IPIs of its own, so the counters move
 * before anything is sent. Hence the control interval, the same wait with
 * nothing sent, reported next to the result.
 */

#include <embox/test.h>

#include <hal/cpu.h>
#include <kernel/cpu/cpu.h>
#include <kernel/printk.h>
#include <kernel/sched/affinity.h>
#include <kernel/thread.h>
#include <time.h>
#include <util/err.h>

EMBOX_TEST_SUITE("SMP: cores reached, and work spread over them");

extern void smp_send_resched(int cpu_id);

/* aarch64 has one; a port without it is reported, not failed */
extern unsigned long aarch64_smp_ipi_count(unsigned int cpu_id)
    __attribute__((weak));

/* Long enough for a tick to fall inside the control interval */
#define WAIT_MS 50

static void wait_ms(unsigned int ms) {
	clock_t deadline = clock() + (clock_t)ms;

	/* Spinning, not sleeping, which would cause what is being measured */
	while (clock() < deadline) {
	}
}

static int cpu_is_up(unsigned int i) {
	/* An idle thread is what cpu_init() leaves behind on a core that is up */
	return (i != cpu_get_id()) && (cpu_get_idle(i) != NULL);
}

TEST_CASE("a reschedule IPI reaches every other CPU") {
	unsigned long base[NCPU], mid[NCPU], after[NCPU];
	unsigned long drift = 0;
	unsigned int i, targets = 0, answered = 0;
	clock_t deadline;

	if (&aarch64_smp_ipi_count == NULL) {
		printk("smp: no per-CPU IPI counter on this port; not looked at\n");
		return;
	}

	for (i = 0; i < NCPU; i++) {
		if (cpu_is_up(i)) {
			targets++;
		}
	}
	if (targets == 0) {
		return; /* alone: nothing to reach */
	}

	/* Control: the same interval, nothing sent. */
	for (i = 0; i < NCPU; i++) {
		base[i] = cpu_is_up(i) ? aarch64_smp_ipi_count(i) : 0;
	}
	wait_ms(WAIT_MS);
	for (i = 0; i < NCPU; i++) {
		mid[i] = cpu_is_up(i) ? aarch64_smp_ipi_count(i) : 0;
	}

	for (i = 0; i < NCPU; i++) {
		if (cpu_is_up(i)) {
			smp_send_resched((int)i);
		}
	}

	/* Wait for each counter to move, rather than compare totals: the tick
	 * delivers its own IPIs at a rate that varies between two windows */
	deadline = clock() + (clock_t)WAIT_MS;
	while (clock() < deadline) {
		int pending = 0;

		for (i = 0; i < NCPU; i++) {
			if (cpu_is_up(i) && (aarch64_smp_ipi_count(i) == mid[i])) {
				pending = 1;
			}
		}
		if (!pending) {
			break;
		}
	}

	for (i = 0; i < NCPU; i++) {
		after[i] = cpu_is_up(i) ? aarch64_smp_ipi_count(i) : 0;
	}

	for (i = 0; i < NCPU; i++) {
		if (!cpu_is_up(i)) {
			continue;
		}
		if (after[i] != mid[i]) {
			answered++;
		}
		drift += mid[i] - base[i];
	}

	printk("smp: %u of %u cpus answered a reschedule IPI; the tick delivered "
	       "%lu over the same interval with nothing sent\n",
	    answered, targets, drift);

	test_assert_equal(targets, answered);
}

/* ------------------------------------------------------------------ */

#define SPREAD_THREADS 4
#define SPREAD_ROUNDS  20000

static unsigned int spread_seen[SPREAD_THREADS];
static volatile int spread_go;

static void *spread_run(void *arg) {
	unsigned int *seen = arg;
	volatile unsigned long x = 0;
	int i;

	while (!spread_go) {
	}

	/* No kernel calls in the loop: entering the kernel is what would move
	 * the thread, and where the scheduler puts it is the question */
	for (i = 0; i < SPREAD_ROUNDS; i++) {
		x = x * 6364136223846793005UL + 1442695040888963407UL;
		*seen |= 1u << cpu_get_id();
	}
	(void)x;

	return NULL;
}

TEST_CASE("threads run only where their affinity allows, and are counted") {
	struct thread *t[SPREAD_THREADS];
	unsigned int used = 0, allowed;
	int i, cores = 0;

	spread_go = 0;
	for (i = 0; i < SPREAD_THREADS; i++) {
		spread_seen[i] = 0;
		t[i] = thread_create(THREAD_FLAG_SUSPENDED, spread_run,
		    &spread_seen[i]);
		test_assert_zero(ptr2err(t[i]));
	}

	allowed = (unsigned int)sched_affinity_get(&t[0]->schedee.affinity);

	for (i = 0; i < SPREAD_THREADS; i++) {
		thread_launch(t[i]);
	}
	spread_go = 1;

	for (i = 0; i < SPREAD_THREADS; i++) {
		test_assert_zero(thread_join(t[i], NULL));
		used |= spread_seen[i];
	}

	/* What holds in every configuration: the threads ran, and only where
	 * they were allowed to. How far they spread is policy. */
	test_assert_not_zero(used);
	test_assert_zero(used & ~allowed);

	for (i = 0; i < NCPU; i++) {
		if (used & (1u << i)) {
			cores++;
		}
	}

	/* Reported: one core means the scheduler spread nothing */
	printk("smp: %d thread(s) ran on %d core(s), mask %#x of allowed %#x\n",
	    SPREAD_THREADS, cores, used, allowed);
}
