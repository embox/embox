/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    26.08.2013
 */

#include <hal/reg.h>

#include <drivers/irqctrl.h>

#define TACTL 	  0x180

#define TACTL_IE  (1 << 1)
#define TACTL_IFG (1 << 0)

#define TRIGGERABLE_IRQ 28

void irqctrl_enable(unsigned int interrupt_nr) {
	if (interrupt_nr != TRIGGERABLE_IRQ) {
		return;
	}
	REG_ORIN(TACTL, TACTL_IE);
}

void irqctrl_disable(unsigned int interrupt_nr) {
	if (interrupt_nr != TRIGGERABLE_IRQ) {
		return;
	}
	REG_ANDIN(TACTL, ~TACTL_IE);
}

void irqctrl_clear(unsigned int interrupt_nr) {
	if (interrupt_nr != TRIGGERABLE_IRQ) {
		return;
	}
	REG_ANDIN(TACTL, ~TACTL_IFG);
}

void irqctrl_force(unsigned int interrupt_nr) {
	if (interrupt_nr != TRIGGERABLE_IRQ) {
		return;
	}
	REG_ORIN(TACTL, TACTL_IFG);
}


