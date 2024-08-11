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
#include <hal/cpu.h>

#include <hal/clock.h>
#include <kernel/irq.h>
#include <kernel/panic.h>
#include <kernel/time/clock_source.h>
#include <kernel/time/ktime.h>

#include <module/embox/driver/interrupt/lapic.h>

#define IRQ0               0x0
#define LAPIC_HZ           1000     /* You can change it */

static int lapic_clock_setup(struct clock_source *cs);

static irq_return_t clock_handler(unsigned int irq_nr, void *dev_id) {
	clock_tick_handler(dev_id);
	return IRQ_HANDLED;
}

static struct time_event_device lapic_event_device = {
	.set_periodic = lapic_clock_setup,
	.name = "lapic clock",
	.irq_nr = IRQ0,
};

static int lapic_timer_init(struct clock_source *cs) {
	ipl_t ipl = ipl_save();

	if (ENOERR != irq_attach(IRQ0, clock_handler, 0, cs, "lapic")) {
		panic("lapic timer irq_attach failed");
	}

	lapic_clock_setup(NULL);

	ipl_restore(ipl);

	return ENOERR;
}

int lapic_clock_setup(struct clock_source *cs) {
	static int initialized = 0;
	uint32_t ticks, cpubusfreq, counter;
	uint8_t tmp;

	if (initialized & 0x1 << cpu_get_id()) {
		return ENOERR;
	}else
		initialized |= 0x1 << cpu_get_id();

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

CLOCK_SOURCE_DEF(lapic_timer, lapic_timer_init, NULL,
	&lapic_event_device, NULL);
