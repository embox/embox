/**
 * @file
 *
 * @brief MIPS build-in interrupt controller
 *
 * @date 04.07.2012
 * @author Anton Bondarev
 */

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
	/* set interrupt handler exception */
	mips_exception_setup(MIPS_EXCEPTION_TYPE_IRQ, mips_c_interrupt_handler);
	return 0;
}


void interrupt_enable(interrupt_nr_t interrupt_nr) {
	uint32_t c0;

	assert(interrupt_nr_valid(interrupt_nr));

	c0 = mips_read_c0_status();
	c0 |= 1 << (interrupt_nr + ST0_IRQ_MASK_OFFSET);
	mips_write_c0_status(c0);
}

void interrupt_disable(interrupt_nr_t interrupt_nr) {
	uint32_t c0;

	assert(interrupt_nr_valid(interrupt_nr));

	c0 = mips_read_c0_status();
	c0 &= ~(1 << (interrupt_nr + ST0_IRQ_MASK_OFFSET));
	mips_write_c0_status(c0);

}
