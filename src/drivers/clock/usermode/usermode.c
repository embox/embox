/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    08.03.2013
 */

#include <embox/unit.h>
#include <hal/clock.h>
#include <hal/system.h>
#include <kernel/irq.h>
#include <kernel/time/clock_source.h>
#include <kernel/host.h>

#define CLOCK_IRQ HOST_TIMER_IRQ

static irq_return_t clock_handler(unsigned int irq_nr, void *data) {

	clock_tick_handler(data);

	return IRQ_HANDLED;
}

static int clk_set_periodic(struct clock_source *cs);

static struct time_event_device umclock_ev = {
	.set_periodic = clk_set_periodic,
	.irq_nr = CLOCK_IRQ,
};

static int clk_set_periodic(struct clock_source *cs) {

	host_timer_config(1000000 / 1000);

	return 0;
}

static int umclock_init(struct clock_source *cs) {
	return irq_attach(CLOCK_IRQ, clock_handler, 0, cs, "usermode clock");
}

CLOCK_SOURCE_DEF(usermode,  umclock_init, NULL,
	&umclock_ev,NULL);
