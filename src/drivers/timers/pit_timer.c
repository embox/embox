/**
 * @file
 * @brief Programmable Interval Timer (PIT) timer for x86
 *
 * @date 27.12.2010
 * @author Nikolay Korotky
 */

#include <hal/clock.h>
#include <hal/reg.h>
#include <kernel/irq.h>
#include <types.h>
#include <hal/interrupt.h>
#include <asm/io.h>

#define INPUT_CLOCK 1193180
#define IRQ0 0x0

irq_return_t clock_handler(int irq_nr, void *dev_id) {
	clock_tick_handler(irq_nr, dev_id);
	return IRQ_HANDLED;
}

void clock_init(void) {
	irq_attach((irq_nr_t) IRQ0,
		(irq_handler_t) &clock_handler, 0, NULL, "PIT");
}

void clock_setup(useconds_t useconds) {
	uint32_t divisor = INPUT_CLOCK / useconds;

	/* Set control byte */
	out8(0x43, 0x36);

	/* Send divisor */
	out8(0x40, divisor & 0xFF);
	out8(0x40, (divisor >> 8) & 0xFF);
}

