/**
 * @file
 * @brief PowerPC Microprocessor Family interrupt controller
 *
 * @date 06.11.12
 * @author Ilia Vaprol
 */

#include <asm/psr.h>
#include <asm/regs.h>

#include <drivers/irqctrl.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(ppc_intc_init);

static int ppc_intc_init(void) {
	return 0;
}

void irqctrl_enable(unsigned int interrupt_nr) {
}

void irqctrl_disable(unsigned int interrupt_nr) {
}

void irqctrl_clear(unsigned int interrupt_nr) {
}

void irqctrl_force(unsigned int interrupt_nr) {
}

void interrupt_handle(void) {
}

