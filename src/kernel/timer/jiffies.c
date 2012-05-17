/**
 * @file
 *
 * @brief Time subsystem based on i8253 timer's jiffies.
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
#include <kernel/time/timecounter.h>
#include <util/array.h>
#include <string.h>

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
//TODO global time timecounter is bad
static struct timecounter sys_timecounter;

ns_t clock_get_systime(void) {
	return timecounter_read(&sys_timecounter);
}

/**
 * Initialization of the time subsystem.
 * It based on PIT timer's jiffies.
 *
 * @return 0 if success
 */
static int module_init(void) {
	const struct clock_event_device *dev;

	/* find PIT timer */
	dev = get_timer_by_name("pit");
	/* set periodic mode */
	dev->set_mode(0x04);
	/* install timecounter value to 0 */
	timecounter_init(&sys_timecounter, dev->cs->cc, 0);
	softirq_install(SOFTIRQ_NR_TIMER, soft_clock_handler,NULL);

	return 0;
}
