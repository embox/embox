/**
 * @file
 * @brief Link layer between timers and clock sources.
 *
 * @date 09.07.2012
 * @author Alexander Kalmuk
 */

#include <kernel/irq_lock.h>
#include <kernel/time/timer.h>
#include <kernel/time/clock_source.h>

#include <kernel/sched/schedee_priority.h>
#include <kernel/lthread/lthread.h>

#include <hal/clock.h>

#include <framework/mod/options.h>

#define CLOCK_HND_PRIORITY OPTION_GET(NUMBER, hnd_priority)

static struct lthread clock_handler_lt;
/* from jiffies.c */
extern struct clock_source *cs_jiffies;

void clock_tick_handler(void *dev_id) {
	clock_handle_ticks(dev_id, 1);
}

void clock_handle_ticks(void *dev_id, unsigned ticks) {
	struct clock_source *cs = (struct clock_source *) dev_id;

	assert(cs);

	cs->jiffies += ticks;

	if (timer_strat_need_sched(cs_jiffies->jiffies)) {
		lthread_launch(&clock_handler_lt);
	}
}

static int clock_handler(struct lthread *self) {
	timer_strat_sched(cs_jiffies->jiffies);
	return 0;
}

int clock_tick_init(void) {
	static int inited = 0;

	if (!inited) {
		lthread_init(&clock_handler_lt, &clock_handler);
		schedee_priority_set(&clock_handler_lt.schedee, CLOCK_HND_PRIORITY);

		inited = 1;
	}

	return 0;
}
