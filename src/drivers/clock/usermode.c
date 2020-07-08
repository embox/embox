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

EMBOX_UNIT_INIT(umclock_init);

#define CLOCK_IRQ HOST_TIMER_IRQ

static irq_return_t clock_handler(unsigned int irq_nr, void *data) {

        clock_tick_handler(data);

	return IRQ_HANDLED;
}

static int clk_config(struct time_dev_conf *conf);

static struct time_event_device umclock_ev = {
	.config = clk_config,
	.event_hz = 1000,
	.irq_nr = CLOCK_IRQ,
};

static struct clock_source umclock_cs = {
	.name = "usermode clock",
	.event_device = &umclock_ev,
	.counter_device = NULL,
	.read = clock_source_read,
};

static int clk_config(struct time_dev_conf *conf) {

	host_timer_config(1000000 / umclock_ev.event_hz);

	return 0;
}

static int umclock_init(void) {

	clock_source_register(&umclock_cs);

	return irq_attach(CLOCK_IRQ, clock_handler, 0, &umclock_cs, "usermode clock");
}
