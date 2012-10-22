/**
 * @file
 * @brief OMAP35x clock device driver
 *
 * @date 15.10.12
 * @author Ilia Vaprol
 */

#include <types.h>

#include <drivers/irqctrl.h>
#include <hal/clock.h>
#include <hal/reg.h>
#include <hal/system.h>
#include <kernel/irq.h>
#include <kernel/time/clock_source.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(omap3_clk_init);

extern int printf(const char *, ...);
static irq_return_t clock_handler(unsigned int irq_nr, void *data) {
	printf("clock_handler\n");
	clock_tick_handler(irq_nr, data);
	return IRQ_HANDLED;
}

#if 0
static int omap3_clk_config(struct time_dev_conf *conf) {
	return 0;
}

static cycle_t omap3_clk_read(void) {
	return 0;
}

static struct time_event_device omap3_clk_event = {
	.config = omap3_clk_config,
	.resolution = 1000,
	.irq_nr = SYSTICK_IRQ,
};


static struct time_counter_device omap3_clk_counter = {
	.read = omap3_clk_read,
	.resolution = SYS_CLOCK / CLOCK_DIVIDER,
};

static struct clock_source omap3_clk_clock_source = {
	.name = "omap3_clk",
	.event_device = &omap3_clk_event,
	.counter_device = &omap3_clk_counter,
	.read = clock_source_read,
};
#endif

static int omap3_clk_init(void) {
	clock_source_register(NULL);
	return irq_attach(-1/*SYSTICK_IRQ*/, clock_handler, 0, NULL, "omap3_clk");
}

