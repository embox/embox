/**
 * @file
 * @brief Interrupt interface realization
 *
 * @date 11.07.10
 * @author Anton Kozlov
 */

#include <assert.h>
#include <hal/arch.h>
#include <hal/interrupt.h>
#include <hal/reg.h>
#include <asm/psr.h>
#include <asm/modes.h>
#include <types.h>
#include <drivers/at91sam7s256.h>

#include <kernel/irq.h>

extern void IRQ_def_handler(void);

void interrupt_init(void) {
	size_t i;
	REG_STORE(AT91C_PMC_PCER, 1 << AT91C_ID_IRQ0 |
			1 << AT91C_ID_IRQ1 | 1 << AT91C_ID_FIQ);
	REG_STORE(AT91C_PIOA_PDR, AT91C_PA20_IRQ0 |
			AT91C_PA30_IRQ1 | AT91C_PA19_FIQ);
	REG_STORE(AT91C_PIOA_BSR, AT91C_PA20_IRQ0 |
			AT91C_PA30_IRQ1 | AT91C_PA19_FIQ);
	for (i = 0; i < 32; i++) {
		REG_STORE(AT91C_AIC_SVR[i],(uint32_t) &IRQ_def_handler);
		REG_STORE(AT91C_AIC_SMR + i,
				AT91C_AIC_SRCTYPE_INT_EDGE_TRIGGERED);
	}
	REG_STORE(AT91C_AIC_IDCR, ~0); /* disabling all interrupts */
	REG_STORE(AT91C_AIC_ICCR, ~0); /* clearing all pending interrupts */
}

void interrupt_enable(interrupt_nr_t interrupt_nr) {
	assert(interrupt_nr_valid(interrupt_nr));
	REG_STORE(AT91C_AIC_SMR + interrupt_nr,
			AT91C_AIC_SRCTYPE_INT_EDGE_TRIGGERED);
	REG_STORE(AT91C_AIC_SVR + interrupt_nr, 0);
	REG_STORE(AT91C_AIC_IECR, 1 << interrupt_nr);
}

void interrupt_disable(interrupt_nr_t interrupt_nr) {
	assert(interrupt_nr_valid(interrupt_nr));
	REG_STORE(AT91C_AIC_IDCR, 1 << interrupt_nr);
}

void interrupt_clear(interrupt_nr_t interrupt_nr) {
	assert(interrupt_nr_valid(interrupt_nr));
	REG_STORE(AT91C_AIC_ICCR, 1 << interrupt_nr);
}

void interrupt_force(interrupt_nr_t interrupt_nr) {
	assert(interrupt_nr_valid(interrupt_nr));
	REG_STORE(AT91C_AIC_ISCR, 1 << interrupt_nr);
}

interrupt_mask_t interrupt_get_status(void) {
	return REG_LOAD(AT91C_AIC_IMR);
}

static inline void disable_interrupts(void) {
	__set_cpsr(__get_cpsr() | I_BIT | F_BIT);
}

static inline void enable_interrupts(void) {
	__set_cpsr(__get_cpsr() & ~(I_BIT | F_BIT));
}


void interrupt_handle(void) {
	uint32_t source;
	REG_LOAD(AT91C_AIC_IVR);
	source = REG_LOAD(AT91C_AIC_ISR) & 0x1f;
#ifdef CONFIG_NESTED_INTERRUPTS
	disable_interrupts();
#endif
	irq_dispatch(source);
#ifdef CONFIG_NESTED_INTERRUPTS
	enable_interrupts();
#endif
	REG_STORE(AT91C_AIC_EOICR, 1); /* write anything */
}
