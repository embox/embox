/**
 * @file pl031.c
 * @brief ARM PrimeCell Real Time Clock driver
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2019-07-18
 */

#include <hal/reg.h>
#include <kernel/irq.h>
#include <kernel/printk.h>
#include <kernel/time/clock_source.h>
#include <kernel/time/time_device.h>

#include <framework/mod/options.h>
#include <embox/unit.h>

#define PL031_BASE      OPTION_GET(NUMBER, base_addr)
#define PL031_IRQ       OPTION_GET(NUMBER, irq_nr)
#define PL031_TARGET_HZ OPTION_GET(NUMBER, freq)

#define PL031_DR   (PL031_BASE + 0x00)
#define PL031_MR   (PL031_BASE + 0x04)
#define PL031_LR   (PL031_BASE + 0x08)
#define PL031_CR   (PL031_BASE + 0x0C)
#define PL031_IMSC (PL031_BASE + 0x10)
#define PL031_RIS  (PL031_BASE + 0x14)
#define PL031_MIS  (PL031_BASE + 0x18)
#define PL031_ICR  (PL031_BASE + 0x1C)

#define LOAD_VALUE 1024

#define PL031_CR_START     (1 << 0)
#define PL031_IMSC_EN      (1 << 0)
#define PL031_ICR_CLEAR    (1 << 0)

static struct clock_source pl031_clock_source;
static irq_return_t clock_handler(unsigned int irq_nr, void *data) {
	REG32_STORE(PL031_ICR_CLEAR, PL031_ICR);
	return IRQ_HANDLED;
}

static int pl031_init(void) {
	clock_source_register(&pl031_clock_source);

	return irq_attach(PL031_IRQ,
	                  clock_handler,
	                  0,
	                  &pl031_clock_source,
	                  "PL031");
}

static int pl031_config(struct time_dev_conf * conf) {
	REG32_STORE(LOAD_VALUE, PL031_LR);

	REG32_STORE(PL031_CR_START, PL031_CR);
	REG32_STORE(PL031_IMSC_EN, PL031_IMSC); /* Enable IRQ */
	return 0;
}

static cycle_t pl031_read(void) {
	return REG32_LOAD(PL031_DR);
}

static struct time_event_device pl031_event = {
	.config   = pl031_config,
	.event_hz = PL031_TARGET_HZ,
	.irq_nr   = PL031_IRQ,
};

static struct time_counter_device pl031_counter = {
	.read     = pl031_read,
	.cycle_hz = PL031_TARGET_HZ,
};

static struct clock_source pl031_clock_source = {
	.name           = "pl031",
	.event_device   = &pl031_event,
	.counter_device = &pl031_counter,
	.read           = clock_source_read,
};

EMBOX_UNIT_INIT(pl031_init);

STATIC_IRQ_ATTACH(PL031_IRQ, clock_handler, &imx6_clock_source);
