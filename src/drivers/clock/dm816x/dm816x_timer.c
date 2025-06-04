/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    14.10.2013
 */

#include <drivers/irqctrl.h>
#include <hal/clock.h>
#include <hal/reg.h>
#include <hal/system.h>
#include <kernel/irq.h>
#include <kernel/time/clock_source.h>
#include <kernel/printk.h>

#include <drivers/common/memory.h>
#include <hal/mmu.h>
#include <mem/vmem.h>

#include <embox/unit.h>

#define TI8168_CLKIN_HZ      27000000

#define CM_TIMER1_CLKSEL     0x48180390
#define CM_CLKIN             0x2

#define GPTIMER31_01_TIDR    0xfff1301
#define GPTIMER31_01_MASK    0xfffffff

#define TI8168_GPTIMER1_BASE ((void *) 0x4802e000)
#define TI8168_GPTIMER1_IRQ  67

#define TI8168_LOAD_VALUE    (0xffffffff - (TI8168_CLKIN_HZ / 1000) + 1)

/* common register values */

#define GPTIMER_TCLR_START	    (1 << 0)
#define GPTIMER_TCLR_AUTORELOAD (1 << 1)

#define GPTIMER_TIER_OVERFLOW   (1 << 1)
#define GPTIMER_TISR_OVERFLOW   (1 << 1)

struct gptimer13_1 {
	uint32_t tidr; 		/* 0x00 */
	uint32_t unused0[3];	/* 0x04 */
	uint32_t cfg;		/* 0x10 */
	uint32_t unused1[3];	/* 0x14 */
	uint32_t eoi;		/* 0x20 */
	uint32_t irqstat_raw;	/* 0x24 */
	uint32_t irqstat;	/* 0x28 */
	uint32_t irqenable_set;	/* 0x2c */
	uint32_t irqenable_clr;	/* 0x30 */
	uint32_t irqwakeen;	/* 0x34 */
	uint32_t tclr;		/* 0x38 */
	uint32_t tcrr;		/* 0x3c */
	uint32_t tldr;		/* 0x40 */
} __attribute__((packed)) __attribute__((aligned(sizeof(uint32_t))));

static irq_return_t ti8168_clock_handler(unsigned int irq_nr, void *data) {
	volatile struct gptimer13_1 *gptimer = TI8168_GPTIMER1_BASE;

	clock_tick_handler(data);

	REG_STORE(&gptimer->irqstat, GPTIMER_TISR_OVERFLOW);
	REG_STORE(&gptimer->eoi, 0);

	return IRQ_HANDLED;
}

static int ti8168_clk_set_periodic(struct clock_source *cs) {
	volatile struct gptimer13_1 *gptimer = TI8168_GPTIMER1_BASE;

	REG_STORE(CM_TIMER1_CLKSEL, CM_CLKIN);

	REG_STORE(&gptimer->cfg, 0x01);

	while (REG_LOAD(&gptimer->cfg) & 0x01);

	REG_STORE(&gptimer->tcrr, TI8168_LOAD_VALUE);
	REG_STORE(&gptimer->tldr, TI8168_LOAD_VALUE);

	REG_STORE(&gptimer->tclr, GPTIMER_TCLR_START | GPTIMER_TCLR_AUTORELOAD);

	REG_STORE(&gptimer->irqenable_set, GPTIMER_TIER_OVERFLOW);
	REG_STORE(&gptimer->irqwakeen, GPTIMER_TIER_OVERFLOW);

	return 0;
}

static struct time_event_device ti8168_clk_event = {
	.set_periodic = ti8168_clk_set_periodic,
	.irq_nr = TI8168_GPTIMER1_IRQ,
};

/* TODO */
static cycle_t ti8168_get_cycles(struct clock_source *cs) {
	return 0;
}

static struct time_counter_device ti8168_counter_device = {
	.get_cycles = ti8168_get_cycles,
	.cycle_hz = TI8168_CLKIN_HZ,
};

static int dm816x_timer_init(struct clock_source *cs) {
	return irq_attach(ti8168_clk_event.irq_nr, ti8168_clock_handler, 0, cs, "omap3_clk");
}

PERIPH_MEMORY_DEFINE(dm816x_timer, (uintptr_t) TI8168_GPTIMER1_BASE, 0x1000);

PERIPH_MEMORY_DEFINE(dm816x_timer_cm, 0x48180000, 0x1000);

CLOCK_SOURCE_DEF(dm816x_timer, dm816x_timer_init, NULL,
	&ti8168_clk_event, &ti8168_counter_device);
