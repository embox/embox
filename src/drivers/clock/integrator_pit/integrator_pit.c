/**
 * @file
 *
 * @data 05 aug 2015
 * @author: Anton Bondarev
 */
#include <sys/mman.h>

#include <hal/reg.h>
#include <hal/clock.h>
#include <kernel/time/clock_source.h>
#include <kernel/irq.h>

#include <embox/unit.h>
#include <drivers/common/memory.h>
#include <kernel/printk.h>

#define HZ 1000

#define TIMER_BASE   OPTION_GET(NUMBER,base_addr)

/* Interrupt vector for timer (TMR1) */
#define CLOCK_IRQ	5

/* The clock rate per second - 1Mhz */
#define CLOCK_RATE	10000000L

/* The initial counter value */
#define TIMER_COUNT	(CLOCK_RATE / HZ)

/* Timer 1 registers */
#define TMR_LOAD	(TIMER_BASE + 0x000)
#define TMR_VAL		(TIMER_BASE + 0x004)
#define TMR_CTRL	(TIMER_BASE + 0x008)
#define TMR_CLR		(TIMER_BASE + 0x00c)
#define TMR_BGLOAD	(TIMER_BASE + 0x024)

/* Timer control register */
#define TCTRL_DISABLE   0x00
#define TCTRL_ENABLE    0x80
#define TCTRL_PERIODIC  0x40
#define TCTRL_INTEN     0x20
#define TCTRL_SCALE256  0x08
#define TCTRL_SCALE16   0x04
#define TCTRL_32BIT     0x02
#define TCTRL_ONESHOT   0x01

EMBOX_UNIT_INIT(integratorcp_init);

static int integratorcp_clock_setup(struct time_dev_conf * conf) {
	/* Setup counter value */
	REG_STORE(TMR_CTRL, TCTRL_DISABLE);
	REG_STORE(TMR_LOAD, TIMER_COUNT);
	REG_ORIN(TMR_CTRL, (TCTRL_ENABLE | TCTRL_PERIODIC));

	/* Enable timer interrupt */
	REG_ORIN(TMR_CTRL, TCTRL_INTEN);

	return 0;
}

static struct time_event_device integratorcp_event_device = {
	.config = integratorcp_clock_setup,
	.event_hz = 1000, .name = "integratorcp_clk",
	.irq_nr = CLOCK_IRQ
};

static cycle_t integratorcp_counter_read(void) {
	return REG32_LOAD(TMR_VAL) & 0xFFFF;
}

static struct time_counter_device integratorcp_counter_device = {
	.cycle_hz = 1000,
	.read = integratorcp_counter_read,
};

static struct clock_source integratorcp_cs = {
	.name = "integratorcp_clk",
	.event_device = &integratorcp_event_device,
	.counter_device = &integratorcp_counter_device,
	.read = clock_source_read /* attach default read function */
};

static irq_return_t clock_handler(unsigned int irq_nr, void *dev_id) {
	clock_tick_handler(irq_nr, dev_id);
	REG_STORE(TMR_CLR, 0x01); /* Clear timer interrupt */
	return IRQ_HANDLED;
}

static int integratorcp_init(void) {
	clock_source_register(&integratorcp_cs);

	return irq_attach(CLOCK_IRQ, clock_handler, 0, &integratorcp_cs,
			"integratorcp_clk");
}

PERIPH_MEMORY_DEFINE(integratorcp_clock, TIMER_BASE, 0x30);
