/**
 * @file
 * @brief MIPS build-in interrupt controller
 *
 * @date 04.07.2012
 * @author Anton Bondarev
 */

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include <asm/mipsregs.h>
#include <drivers/irqctrl.h>
#include <kernel/irq.h>

void irqctrl_enable(unsigned int irq) {
	uint32_t c0;

	assert(irq_nr_valid(irq));

	c0 = mips_read_c0_status();
	c0 |= 1U << (irq + ST0_IRQ_MASK_OFFSET);
	mips_write_c0_status(c0);
}

void irqctrl_disable(unsigned int irq) {
	uint32_t c0;

	assert(irq_nr_valid(irq));

	c0 = mips_read_c0_status();
	c0 &= ~(1U << (irq + ST0_IRQ_MASK_OFFSET));
	mips_write_c0_status(c0);
}

void irqctrl_force(unsigned int irq) {
}

int irqctrl_pending(unsigned int irq) {
	return 0;
}

void irqctrl_eoi(unsigned int irq) {
}

int irqctrl_get_intid(void) {
	unsigned int irq;
	uint32_t pending;

	pending = (mips_read_c0_cause() & mips_read_c0_status() & CAUSE_IM) >> ST0_IRQ_MASK_OFFSET;

	for (irq = IRQCTRL_IRQS_TOTAL - 1; irq >=0 ; irq--) {
		if (pending & (1U << irq)) {
			return irq;
		}
	}

	return -1;
}

IRQCTRL_DEF(mips_intc, NULL);
