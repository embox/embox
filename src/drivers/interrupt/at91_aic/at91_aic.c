/**
 * @file
 * @brief Driver for Advanced Interrupt Controller.
 *
 * @date 11.07.10
 * @author Anton Kozlov
 */

#include <assert.h>
#include <stdint.h>

#include <drivers/at91sam7s256.h>
#include <drivers/irqctrl.h>
#include <embox/unit.h>
#include <hal/arch.h>
#include <hal/ipl.h>
#include <hal/reg.h>
#include <kernel/irq.h>
#include <kernel/panic.h>

static void irq_def_handler(void) {
	panic(__func__);
}

static int at91_intc_init(void) {
	REG_STORE(AT91C_PMC_PCER,
	    1 << AT91C_ID_IRQ0 | 1 << AT91C_ID_IRQ1 | 1 << AT91C_ID_FIQ);
	REG_STORE(AT91C_PIOA_PDR,
	    AT91C_PA20_IRQ0 | AT91C_PA30_IRQ1 | AT91C_PA19_FIQ);
	REG_STORE(AT91C_PIOA_BSR,
	    AT91C_PA20_IRQ0 | AT91C_PA30_IRQ1 | AT91C_PA19_FIQ);
	for (int i = 0; i < 32; i++) {
		REG_STORE(AT91C_AIC_SVR[i], (uint32_t)&irq_def_handler);
		REG_STORE(AT91C_AIC_SMR + i, AT91C_AIC_SRCTYPE_INT_EDGE_TRIGGERED);
	}

	REG_STORE(AT91C_AIC_IDCR, ~0); /* disabling all interrupts */
	REG_STORE(AT91C_AIC_ICCR, ~0); /* clearing all pending interrupts */

	return 0;
}

void irqctrl_enable(unsigned int interrupt_nr) {
	REG_STORE(AT91C_AIC_SMR + interrupt_nr,
	    AT91C_AIC_SRCTYPE_INT_EDGE_TRIGGERED);
	REG_STORE(AT91C_AIC_SVR + interrupt_nr, 0);
	REG_STORE(AT91C_AIC_IECR, 1 << interrupt_nr);
}

void irqctrl_disable(unsigned int interrupt_nr) {
	REG_STORE(AT91C_AIC_IDCR, 1 << interrupt_nr);
}

void irqctrl_clear(unsigned int interrupt_nr) {
	REG_STORE(AT91C_AIC_ICCR, 1 << interrupt_nr);
}

void irqctrl_force(unsigned int interrupt_nr) {
	REG_STORE(AT91C_AIC_ISCR, 1 << interrupt_nr);
}

void irqctrl_eoi(unsigned int irq) {
	REG_STORE(AT91C_AIC_EOICR, irq); /* write anything */
}

int irqctrl_get_intid(void) {
	unsigned int irq;

	REG_LOAD(AT91C_AIC_IVR);
	irq = REG_LOAD(AT91C_AIC_ISR);

	return irq;
}

IRQCTRL_DEF(at91_intc, at91_intc_init);
