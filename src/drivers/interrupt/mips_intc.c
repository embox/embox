/**
 * @file
 *
 * @brief MIPS build-in interrupt controller
 *
 * @date 04.07.2012
 * @author Anton Bondarev
 */

#include <stdint.h>
#include <embox/unit.h>
#include <asm/mipsregs.h>
#include <asm/entry.h>
#include <kernel/irq.h>
#include <assert.h>

EMBOX_UNIT_INIT(unit_init);

extern void mips_c_interrupt_handler(pt_regs_t *regs);
/**
 * Initialize MIPS build-in interrupts controller
 */
static int unit_init(void) {
#if 0
	unsigned int c0_reg;
	/* set interrupt handler exception */
	mips_exception_setup(MIPS_EXCEPTION_TYPE_IRQ, mips_c_interrupt_handler);

	/* read status registers for cleaning interrupts mask */
	c0_reg = mips_read_c0_status();
	c0_reg &= ~(ST0_IM);           /* clear all interrupts mask */
	c0_reg |= ST0_IE;              /* global enable interrupt */
	mips_write_c0_status(c0_reg);  /* write back status register */

	/* read cause register for cleaning all pending bits */
	c0_reg = mips_read_c0_cause();
	c0_reg &= ~(ST0_IM);           /* clear all interrupts pending bits */
	mips_write_c0_cause(c0_reg);   /* write back cause register */
#endif
	return 0;
}


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
