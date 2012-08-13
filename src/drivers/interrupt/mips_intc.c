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
#include <kernel/irq.h>
#include <assert.h>

EMBOX_UNIT_INIT(unit_init);

/**
 * Initialize MIPS build-in interrupts controller
 */
static int unit_init(void) {
	uint32_t c0_status;
	c0_status = mips_read_c0_status();
	c0_status &= ~(ST0_IM);
//	c0_status &= ~(ST0_ERL);
	c0_status &= ~(ST0_EXL);
	c0_status |= ST0_IE;
	mips_write_c0_status(c0_status);

	//mips_intr_enable();


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
