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

#define GPTIMER1_BASE 0x48318000
#define GPTIMER1_IRQ 37

#define GPTIMER_POS_INC (GPTIMER1_BASE + 0x48)
#define GPTIMER_NEG_INC (GPTIMER1_BASE + 0x4c)
#define GPTIMER_TWPS    (GPTIMER1_BASE + 0x34)
#define GPTIMER_TLDR    (GPTIMER1_BASE + 0x2c)
#define GPTIMER_TCRR    (GPTIMER1_BASE + 0x28)
#define GPTIMER_TCLR    (GPTIMER1_BASE + 0x24)
#define GPTIMER_TWER    (GPTIMER1_BASE + 0x20)
#define GPTIMER_TIER    (GPTIMER1_BASE + 0x1c)
#define GPTIMER_TISTAT  (GPTIMER1_BASE + 0x14)
#define GPTIMER_CFG     (GPTIMER1_BASE + 0x10)

#define GPTIMER_TCLR_START	(1 << 0)
#define GPTIMER_TCLR_AUTORELOAD (1 << 1)

#define GPTIMER_TIER_OVERFLOW   (1 << 1)

#define CM_FCLKEN_WKUP  0x48004C00
#define CM_ICLKEN_WKUP  0x48004C10

extern int prom_printf(const char *, ...);
static irq_return_t clock_handler(unsigned int irq_nr, void *data) {
	prom_printf("\t\tclock_handler\n");
	clock_tick_handler(irq_nr, data);
	return IRQ_HANDLED;
}

static int omap3_clk_config(struct time_dev_conf *conf) {

	REG_STORE(CM_FCLKEN_WKUP, 1);
	REG_STORE(CM_ICLKEN_WKUP, 1);

	REG_STORE(GPTIMER_CFG, 0x2);

	while (0 == REG_LOAD(GPTIMER_TISTAT));

#if 0
	REG_STORE(GPTIMER_POS_INC, 232000);
	REG_STORE(GPTIMER_NEG_INC, -768000);
#endif

	REG_STORE(GPTIMER_TCRR, 0xffffffe0);
	REG_STORE(GPTIMER_TLDR, 0xffffffe0);

	REG_STORE(GPTIMER_TCLR, GPTIMER_TCLR_START | GPTIMER_TCLR_AUTORELOAD);

	REG_STORE(GPTIMER_TIER, GPTIMER_TIER_OVERFLOW);
	/*REG_STORE(GPTIMER_TWER, GPTIMER_TIER_OVERFLOW);*/

	return 0;
}

static cycle_t omap3_clk_read(void) {
	return 0;
}

static struct time_event_device omap3_clk_event = {
	.config = omap3_clk_config,
	.resolution = 1000,
//	.irq_nr = SYSTICK_IRQ,
};


static struct time_counter_device omap3_clk_counter = {
	.read = omap3_clk_read,
//	.resolution = SYS_CLOCK / CLOCK_DIVIDER,
};

static struct clock_source omap3_clk_clock_source = {
	.name = "omap3_clk",
	.event_device = &omap3_clk_event,
	.counter_device = &omap3_clk_counter,
	.read = clock_source_read,
};

static int omap3_clk_init(void) {
	clock_source_register(&omap3_clk_clock_source);
	return irq_attach(GPTIMER1_IRQ, clock_handler, 0, NULL, "omap3_clk");
}

