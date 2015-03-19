/**
 * @file
 * @brief Link layer between timers and clock sources.
 *
 * @date 09.07.2012
 * @author Alexander Kalmuk
 */

#include <embox/unit.h>
#include <module/embox/kernel/time/slowdown.h>
#include <kernel/time/timer.h>
#include <kernel/time/clock_source.h>

#include <kernel/lthread/lthread.h>
#include <kernel/lthread/lthread_priority.h>
#include <util/err.h>

#define SLOWDOWN_SHIFT OPTION_MODULE_GET(embox__kernel__time__slowdown, NUMBER, shift)
#define CLOCK_HND_PRIORITY OPTION_GET(NUMBER, hnd_priority)

EMBOX_UNIT_INIT(init);

static int inited = 0;

static struct lthread *clock_handler_lt = NULL;
extern struct clock_source *cs_jiffies;

void clock_tick_handler(int irq_num, void *dev_id) {
	struct clock_source *cs = (struct clock_source *) dev_id;

	assert(cs);
	if (++cs->jiffies_cnt == (1 << SLOWDOWN_SHIFT)) {
		cs->jiffies_cnt = 0;
		cs->jiffies++;

		/* FIXME: Check for inited is necessary because this function
		   may be called before initialization of this module.*/
		if (!inited) {
			return;
		}

		if (cs_jiffies->event_device &&	irq_num == cs_jiffies->event_device->irq_nr) {
			lthread_launch(clock_handler_lt);
		}
	}
}

static void *clock_handler(void *data) {
	timer_strat_sched();
	return NULL;
}

static int init(void) {
	clock_handler_lt = lthread_create(&clock_handler, NULL);
	assert(!err(clock_handler_lt));
	lthread_priority_set(clock_handler_lt, CLOCK_HND_PRIORITY);

	inited = 1;

	return 0;
}
