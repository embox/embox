/**
 * @file e2k.c
 * @brief Stub
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 20.12.2017
 */
#include <stdint.h>
#include <asm/io.h>

#include <embox/unit.h>
#include <framework/mod/options.h>

#include <hal/clock.h>
#include <kernel/irq.h>
#include <kernel/printk.h>
#include <kernel/time/clock_source.h>
#include <kernel/time/time_device.h>

#define E2K_CLOCK_BASE (uintptr_t)0x83200000
#define IRQ_NR     OPTION_GET(NUMBER, irq_num)
#define LT_FREQ    OPTION_GET(NUMBER, freq)

#define E2K_COUNTER_LIMIT	(E2K_CLOCK_BASE + 0x00)
#define E2K_COUNTER_START_VALUE	(E2K_CLOCK_BASE + 0x04)
#define E2K_COUNTER		(E2K_CLOCK_BASE + 0x08)
#define E2K_COUNTER_CONTROL	(E2K_CLOCK_BASE + 0x0c)
#define E2K_WD_COUNTER		(E2K_CLOCK_BASE + 0x10)
#define E2K_WD_COUNTER_LOW	(E2K_CLOCK_BASE + 0x10)
#define E2K_WD_COUNTER_HIGH	(E2K_CLOCK_BASE + 0x14)
#define E2K_WD_LIMIT		(E2K_CLOCK_BASE + 0x18)
#define E2K_POWER_COUNTER	(E2K_CLOCK_BASE + 0x1c)
#define E2K_POWER_COUNTER_LOW	(E2K_CLOCK_BASE + 0x1c)
#define E2K_POWER_COUNTER_HIGH	(E2K_CLOCK_BASE + 0x20)
#define E2K_WD_CONTROL		(E2K_CLOCK_BASE + 0x24)
#define E2K_RESET_COUNTER	(E2K_CLOCK_BASE + 0x28)
#define E2K_RESET_COUNTER_LOW	(E2K_CLOCK_BASE + 0x28)
#define E2K_RESET_COUNTER_HIGH	(E2K_CLOCK_BASE + 0x2c)

/* counters registers structure */
#define	LT_COUNTER_SHIFT	9	/* [30: 9] counters value */
#define	LT_COUNTER_LIMIT_SHIFT	31	/* [31] Limit bit */
#define	LT_COUNTER_LIMIT_BIT	(1 << LT_COUNTER_LIMIT_SHIFT)

#define	LT_WRITE_COUNTER_VALUE(count)	((count) << LT_COUNTER_SHIFT)
#define	LT_READ_COUNTER_VALUE(count)	((count) >> LT_COUNTER_SHIFT)
#define	LT_NSEC_PER_COUNTER_INCR	100	/* 10 MHz == 100 nunosec */

/* counter control register structure */
#define	LT_COUNTER_CNTR_START	0x00000001	/* start/stop timer */
#define	LT_COUNTER_CNTR_INVERTL	0x00000002	/* invert limit bit */
#define	LT_COUNTER_CNTR_LINIT	0x00000004	/* Limit bit initial state */
						/* 1 - limit bit set to 1 */

#define	LT_COUNTER_CNTR_LAUNCH	(LT_COUNTER_CNTR_START)
#define	LT_INVERT_COUNTER_CNTR_LAUNCH	(LT_COUNTER_CNTR_LAUNCH | \
						LT_COUNTER_CNTR_INVERTL | \
						LT_COUNTER_CNTR_LINIT)
#define	LT_COUNTER_CNTR_STOP	(0)

#define WD_CLOCK_TICK_RATE 10000000L
#define WD_LATCH(tick_rate) (((tick_rate) + HZ/2) / HZ)
#define WD_LIMIT_SHIFT	12
#define	WD_WRITE_COUNTER_VALUE(count) (count)
#define	WD_READ_COUNTER_VALUE(count) ((count) << WD_LIMIT_SHIFT)
#define WD_SET_COUNTER_VAL(sek)	\
		(WD_WRITE_COUNTER_VALUE(WD_CLOCK_TICK_RATE * (sek)))

#define	WD_INTR_MODE	0x1
#define	WD_ENABLE	0x2
#define	WD_EVENT	0x4

static struct clock_source e2k_clock_source;
static irq_return_t e2k_clock_handler(unsigned int irq_nr, void *dev_id) {
	clock_tick_handler(irq_nr, dev_id);
	return IRQ_HANDLED;
}

static int e2k_clock_init(void) {
	uint32_t clock_hz = (10000000 + LT_FREQ / 2) / LT_FREQ;

	clock_source_register(&e2k_clock_source);
	irq_attach(IRQ_NR, e2k_clock_handler, 0, &e2k_clock_source, "e2k clock");

	/* Setup frequency */
	e2k_write32(clock_hz << 9, E2K_COUNTER_LIMIT);
	e2k_write32(LT_INVERT_COUNTER_CNTR_LAUNCH, E2K_COUNTER_CONTROL);

	return 0;
}

static int e2k_clock_config(struct time_dev_conf * conf) {
	return 0;
}

static cycle_t e2k_clock_read(void) {
#if 0
	/* TODO Should be checked, because values are looking a bit weird. */
	return e2k_read32((void*)E2K_RESET_COUNTER_LOW); /* Ignore high 32 bits */
#endif
	return 0;
}

static struct time_event_device e2k_clock_event = {
	.config   = e2k_clock_config,
	.event_hz = LT_FREQ,
	.irq_nr = IRQ_NR
};

static struct time_counter_device e2k_clock_counter = {
	.read     = e2k_clock_read,
	.cycle_hz = 1000,
};

static struct clock_source e2k_clock_source = {
	.name           = "e2k_clock",
	.event_device   = &e2k_clock_event,
	.counter_device = &e2k_clock_counter,
	.read           = clock_source_read,
};

EMBOX_UNIT_INIT(e2k_clock_init);
