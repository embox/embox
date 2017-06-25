/**
 * @file
 * @brief
 *
 * @date 28.12.2012
 * @author Anton Bulychev
 */

#include <embox/unit.h>

#include <stdint.h>
#include <errno.h>

#include <asm/io.h>

#include <hal/ipl.h>

#include <hal/clock.h>
#include <kernel/irq.h>
#include <kernel/panic.h>
#include <kernel/time/clock_source.h>
#include <kernel/time/ktime.h>

#include <module/embox/driver/interrupt/lapic.h>

#define IRQ0               0x0
#define LAPIC_HZ           1000     /* You can change it */

static int lapic_clock_setup(struct time_dev_conf *conf);

static struct clock_source lapic_clock_source;
static struct time_event_device lapic_event_device;

static irq_return_t clock_handler(unsigned int irq_nr, void *dev_id) {
        clock_tick_handler(irq_nr, dev_id);
        return IRQ_HANDLED;
}

static struct time_event_device lapic_event_device = {
	.config = lapic_clock_setup,
	.event_hz = LAPIC_HZ,
	.name = "lapic clock",
	.irq_nr = IRQ0,
};

static struct clock_source lapic_clock_source = {
	.name = "lapic clock",
	.event_device = &lapic_event_device,
	.read = clock_source_read /* attach default read function */
};

EMBOX_UNIT_INIT(lapic_clock_init);

static int lapic_clock_init(void) {
	ipl_t ipl = ipl_save();

	clock_source_register(&lapic_clock_source);

	if (ENOERR != irq_attach(IRQ0, clock_handler, 0, &lapic_clock_source, "lapic")) {
		panic("lapic timer irq_attach failed");
	}

	lapic_clock_setup(NULL);

	ipl_restore(ipl);

	return ENOERR;
}

int lapic_clock_setup(struct time_dev_conf *conf) {
	static int initialized = 0;
	uint32_t ticks, cpubusfreq, counter;
	uint8_t tmp;

	if (initialized) {
		return ENOERR;
	}
	initialized = 1;

	/*
	 * Map APIC timer to an interrupt, and by that enable it in
	 * one-shot mode.
	 */
	lapic_write(LAPIC_LVT_TR, 32);

	/* Set up divide value to 16 */
	lapic_write(LAPIC_TIMER_DCR, 0x03);

	/*
	 * Initialize PIT Ch 2 in one-shot mode.
	 * Waiting 1/100 sec.
	 */
	outb((inb(0x61) & 0xFD) | 1, 0x61);
	outb(0xB2, 0x43);

	/* 1193180/100 Hz = 11931 = 2e9bh */
	outb(0x9B, 0x42);	/* LSB */
	inb(0x60);	/* short delay */
	outb(0x2E, 0x42);	/* MSB */

	/* Reset PIT one-shot counter (start counting) */
	tmp = inb(0x61) & 0xFE;
	outb((uint8_t) tmp, 0x61);     /* Gate low */
	outb((uint8_t) tmp | 1, 0x61); /* Gate high */

	/* Reset APIC timer (set counter to -1) */
	lapic_write(LAPIC_TIMER_ICR, 0xFFFFFFFF);

	/* Now wait until PIT counter reaches zero */
	while(!(inb(0x61) & 0x20));

	/* Stop APIC timer */
	lapic_write(LAPIC_LVT_TR, 0x10000);

	/* Now do the math... */
	ticks = (0xFFFFFFFF - lapic_read(LAPIC_TIMER_CCR)) + 1;
	cpubusfreq = ticks * 16 * 100;
	counter = cpubusfreq / LAPIC_HZ / 16;

	/* Set APIC timer counter initializer */
	lapic_write(LAPIC_TIMER_ICR, counter < 16 ? 16 : counter);

	/* Finally re-enable timer in periodic mode. */
	lapic_write(LAPIC_LVT_TR, 32 | 0x20000);

#if 0
	/*
	 * Setting divide value register again not needed by the manuals
	 * although I have found buggy hardware that required it
	 */
	lapic_write(LAPIC_TIMER_DCR, 0x03);
#endif

	return ENOERR;
}
