/**
 * @file
 *
 * @brief Time subsystem based on timer's jiffies.
 *
 * @date 10.04.2012
 * @author Anton Bondarev
 */
#include <embox/unit.h>

#include <kernel/clock_source.h>
#include <kernel/clock_event.h>
#include <kernel/softirq.h>

#include <hal/clock.h>
#include <kernel/timer.h>
#include <kernel/ktime.h>


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

/**
 * Initialization of the time subsystem.
 *
 * @return 0 if success
 */
static int module_init(void) {
	const struct clock_event_device *dev;

	/* find clock_event_device with maximal resolution  */
	dev = cedev_get_best(JIFFIES);
	assert(dev);

	/* set and register clock_source */
	jiffies.resolution = dev->resolution;
	clock_source_register(&jiffies);
	/* set periodic mode */
	dev->set_mode(CLOCK_EVT_MODE_PERIODIC);

	softirq_install(SOFTIRQ_NR_TIMER, soft_clock_handler,NULL);

	return 0;
}
