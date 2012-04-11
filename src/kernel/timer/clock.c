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

EMBOX_UNIT_INIT(module_init);

static clock_t sys_ticks = 0; /* ticks after start system. */

clock_t clock(void) {
	return sys_ticks;
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

/**
 * Initialization of the timer subsystem.
 *
 * @return 0 if success
 */
static int module_init(void) {
	clock_init();
	clock_setup(clock_source_get_precision(NULL));
	softirq_install(SOFTIRQ_NR_TIMER, soft_clock_handler,NULL);
	return 0;
}

