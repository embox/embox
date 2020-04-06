/**
 * @file
 * @brief Link layer between timers and clock sources.
 *
 * @date 09.07.2012
 * @author Alexander Kalmuk
 */

#include <module/embox/kernel/time/slowdown.h>
#include <kernel/irq_lock.h>
#include <kernel/time/timer.h>
#include <kernel/time/clock_source.h>

#include <kernel/sched/schedee_priority.h>
#include <kernel/lthread/lthread.h>

#define SLOWDOWN_SHIFT OPTION_MODULE_GET(embox__kernel__time__slowdown, NUMBER, shift)
#define CLOCK_HND_PRIORITY OPTION_GET(NUMBER, hnd_priority)

extern struct clock_source *cs_jiffies;
static clock_t timer_sched_cnt;

static int clock_handler(struct lthread *self);
static LTHREAD_DEF(clock_handler_lt, clock_handler, CLOCK_HND_PRIORITY);

void clock_tick_handler(int irq_num, void *dev_id) {
	struct clock_source *cs = (struct clock_source *) dev_id;

	assert(cs);
	if (++cs->jiffies_cnt == (1 << SLOWDOWN_SHIFT)) {
		cs->jiffies_cnt = 0;
		cs->jiffies++;

		if (cs == cs_jiffies) {
			timer_sched_cnt++;
			lthread_launch(&clock_handler_lt);
		}
	}
}

static int clock_handler(struct lthread *self) {

	int sched_cnt;
	irq_lock();
		sched_cnt = timer_sched_cnt;
		timer_sched_cnt = 0;
	irq_unlock();

	while (0 < sched_cnt--) {
		timer_strat_sched();
	}
	return 0;
}
