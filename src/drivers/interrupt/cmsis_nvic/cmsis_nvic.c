/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    02.07.2012
 */

#include <kernel/irq.h>
#include <module/embox/arch/arm/cmsis.h>

#include <drivers/irqctrl.h>

#ifndef STATIC_IRQ_EXTENTION
#error "Dynamic IRQS aren't supported!"
#endif

void irqctrl_enable(unsigned int interrupt_nr) {
	NVIC_EnableIRQ(interrupt_nr);
}

void irqctrl_disable(unsigned int interrupt_nr) {
	NVIC_DisableIRQ(interrupt_nr);
}

void irqctrl_clear(unsigned int interrupt_nr) {
	NVIC_ClearPendingIRQ(interrupt_nr);

}

void irqctrl_force(unsigned int interrupt_nr) {
	NVIC_SetPendingIRQ(interrupt_nr);
}

static int cmsis_nvic_init(void) {
	return 0;
}

IRQCTRL_DEF(cortexm_nvic, cmsis_nvic_init);
