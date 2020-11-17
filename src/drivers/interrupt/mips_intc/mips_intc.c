/**
 * @file
 *
 * @brief MIPS build-in interrupt controller
 *
 * @date 04.07.2012
 * @author Anton Bondarev
 */
#include <assert.h>
#include <stdint.h>
#include <stddef.h>

#include <asm/mipsregs.h>

#include <drivers/irqctrl.h>

void irqctrl_enable(unsigned int interrupt_nr) {
	uint32_t c0;

	c0 = mips_read_c0_status();
	c0 |= 1 << (interrupt_nr + ST0_IRQ_MASK_OFFSET);
	mips_write_c0_status(c0);
}

void irqctrl_disable(unsigned int interrupt_nr) {
	uint32_t c0;

	c0 = mips_read_c0_status();
	c0 &= ~(1 << (interrupt_nr + ST0_IRQ_MASK_OFFSET));
	mips_write_c0_status(c0);
}

IRQCTRL_DEF(mips_intc, NULL);

