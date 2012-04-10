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

#include <hal/clock.h>
#include <kernel/timer.h>

EMBOX_UNIT_INIT(module_init);

static clock_t sys_ticks; /* ticks after start system. */

clock_t clock(void) {
	return sys_ticks;
}

/**
 * Handling of the clock tick.
 */
void clock_tick_handler(int irq_num, void *dev_id) {
	sys_ticks++;
	timer_sched();
}

/**
 * Initialization of the timer subsystem.
 *
 * @return 0 if success
 */
static int module_init(void) {
	sys_ticks = 0;
	clock_init();
	clock_setup(clock_source_get_precision(NULL));
	return 0;
}

