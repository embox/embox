/**
 * @file
 *
 * @brief RISC-V build-in interrupt controller
 *
 * @date 04.10.2019
 * @author Anastasia Nizharadze
 */

#include <stdint.h>
#include <embox/unit.h>
#include <asm/regs.h>
#include <kernel/irq.h>
#include <assert.h>

EMBOX_UNIT_INIT(unit_init);

static int unit_init(void) {
	return 0;
}

void irqctrl_enable(unsigned int interrupt_nr) {
	set_csr_bit(mie, (1 << interrupt_nr));
}

void irqctrl_disable(unsigned int interrupt_nr) {
	clear_csr_bit(mie, (1 << interrupt_nr));
}
