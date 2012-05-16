/**
 * @file
 *
 * @brief
 *
 * @date 10.04.2012
 * @author Anton Bondarev
 */
#include <embox/unit.h>

#include <kernel/clock_source.h>
#include <kernel/softirq.h>

#include <hal/clock.h>
#include <kernel/timer.h>
#include <kernel/ktime.h>
#include <kernel/time/timecounter.h>

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
 * Initialization of the timer subsystem.
 *
 * @return 0 if success
 */
static int module_init(void) {
	struct clock_source *cs;
	//TODO clock_init must be dynamic
	clock_init();

	cs = clock_source_get_default();
	clock_setup(clock_source_get_precision(cs));

	timecounter_init(&sys_timecounter, cs->cc, 0);

	softirq_install(SOFTIRQ_NR_TIMER, soft_clock_handler,NULL);
	return 0;
}
