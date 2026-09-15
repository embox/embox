/**
 * @file
 * @brief Link layer between timers and clock sources.
 *
 * @date 09.07.2012
 * @author Alexander Kalmuk
 */

#include <assert.h>
#include <sys/types.h>

#include <framework/mod/options.h>
#include <hal/clock.h>
#include <hal/cpu.h>
#include <kernel/irq_lock.h>
#include <kernel/lthread/lthread.h>
#include <kernel/sched/affinity.h>
#include <kernel/sched/schedee_priority.h>
#include <kernel/time/clock_source.h>
#include <kernel/time/sys_timer.h>

#define CLOCK_HND_PRIORITY OPTION_GET(NUMBER, hnd_priority)

#ifdef SMP
/* One handler per CPU, each pinned to its own core, because what it services
 * -- the timer list -- is per-CPU too. Pinning is the whole point: an unpinned
 * handler would service whichever list the scheduler happened to put it next
 * to. */
static struct lthread clock_handler_lt[NCPU];
static unsigned int clock_tick_owner;

#define CLOCK_HANDLER_LT (&clock_handler_lt[cpu_get_id()])
#else
static struct lthread clock_handler_lt;

#define CLOCK_HANDLER_LT (&clock_handler_lt)
#endif /* SMP */
/* from jiffies.c */
extern struct clock_source *cs_jiffies;

void clock_tick_handler(void *dev_id) {
	clock_handle_ticks(dev_id, 1);
}

void jiffies_update(int ticks) {
	clock_handle_ticks(cs_jiffies, ticks);
}

/* How many timer interrupts each core has taken.
 *
 * The timer list is per-CPU and is serviced only by that CPU's own tick, so a
 * core that stops taking its interrupt stops waking every thread that started
 * a timed wait on it -- silently, and with the other cores none the wiser.
 * That is one of the two shapes a lost wakeup can have, and without this
 * counter it is indistinguishable from the other. Plain increments: the writer
 * is the only writer, and a reader wants the trend, not a transaction.
 */
#ifdef SMP
unsigned long clock_ticks_seen[NCPU];
#define CLOCK_TICK_CPU cpu_get_id()
#else
unsigned long clock_ticks_seen[1];
#define CLOCK_TICK_CPU 0
#endif

void clock_handle_ticks(void *dev_id, unsigned ticks) {
	clock_t next_event;
	struct clock_source *cs = (struct clock_source *)dev_id;

	assert(cs);

	clock_ticks_seen[CLOCK_TICK_CPU] += ticks;

	/* Jiffies is elapsed time, not a count of interrupts taken. Every CPU has
	 * its own generic-timer interrupt at the same rate, so only one of them
	 * may advance the clock. */
#ifdef SMP
	if (cpu_get_id() == clock_tick_owner)
#endif
	{
		cs->event_device->jiffies += ticks;
	}

	if ((timer_strat_get_next_event(&next_event) == 0)
	    && (cs_jiffies->event_device->jiffies >= next_event)) {
		lthread_launch(CLOCK_HANDLER_LT);
	}
}

static int clock_handler(struct lthread *self) {
	timer_strat_sched(cs_jiffies->event_device->jiffies);
	return 0;
}

int clock_tick_init(void) {
#ifdef SMP
	unsigned int i;

	/* Whoever runs this owns the clock: it is the boot CPU, at runlevel 0,
	 * long before any other core exists. Recorded rather than assumed to be
	 * zero. */
	clock_tick_owner = cpu_get_id();

	for (i = 0; i < NCPU; i++) {
		lthread_init(&clock_handler_lt[i], &clock_handler);
		schedee_priority_set(&clock_handler_lt[i].schedee, CLOCK_HND_PRIORITY);
		sched_affinity_set(&clock_handler_lt[i].schedee.affinity, 1 << i);
	}
#else
	lthread_init(&clock_handler_lt, &clock_handler);
	schedee_priority_set(&clock_handler_lt.schedee, CLOCK_HND_PRIORITY);
#endif /* SMP */

	return 0;
}
