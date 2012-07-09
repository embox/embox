/**
 * @file
 *
 * @brief Time subsystem based on timer's jiffies.
 *
 * @date 10.04.2012
 * @author Anton Bondarev
 */
#include <embox/unit.h>

#include <kernel/time/clock_source.h>
#include <kernel/softirq.h>

#include <kernel/time/timer.h>


EMBOX_UNIT_INIT(module_init);

static cycle_t volatile sys_ticks = 0; /* ticks after start system. */

clock_t clock_sys_ticks(void) {
	return (clock_t)sys_ticks;
}

/**
 * Handling of the clock tick.
 */
void clock_tick_handler(int irq_num, void *dev_id) {
	sys_ticks++;
	softirq_raise(SOFTIRQ_NR_TIMER);
}

static void soft_clock_handler(softirq_nr_t softirq_nr, void *data) {
	timer_strat_sched();
}

static struct clock_source jiffies = {
	.flags = 1,
};


uint32_t clock_sys_sec(void) {
	return clock_source_clock_to_sec(&jiffies, clock_sys_ticks());
}

struct time_dev_conf jiffies_conf = {
	HW_TIMER_PERIOD
};

/**
 * Initialization of the time subsystem.
 *
 * @return 0 if success
 */
static int module_init(void) {
	const struct clock_source *cs;

	/* find clock_event_device with maximal resolution  */
	cs = clock_source_get_best(JIFFIES);
	assert(cs);

	jiffies.event_device = cs->event_device;

	/* set periodic mode */
	cs->event_device->config(&jiffies_conf);

	softirq_install(SOFTIRQ_NR_TIMER, soft_clock_handler,NULL);

	return 0;
}
