/**
 * @file
 * @brief Interrupt interface realization
 *
 * @date 11.07.2010
 * @author Anton Kozlov
 */
#include <assert.h>
#include <hal/arch.h>
#include <hal/interrupt.h>
#include <hal/reg.h>
#include <asm/psr.h>
#include <types.h>
#include <AT91SAM7_AIC.h>
#include <AT91SAM7_PIO.h>
#include <AT91SAM7_PMC.h>
#include <AT91SAM7S256.h>

void interrupt_init(void) {
	//__set_cpsr(__get_cpsr() & (~(IRQ_BIT | FIQ_BIT)));
	REG_STORE(AT91C_PMC_PCER, 1 << 30);
	//REG_STORE(AIC_DCR, 1);
	REG_STORE(AT91C_PIOA_PDR, 1 << 20);
	REG_STORE(AT91C_PIOA_BSR, 1 << 20);
	REG_STORE(AT91C_AIC_IDCR, ~0);
	REG_STORE(AT91C_AIC_ICCR, ~0);
}

void interrupt_enable(interrupt_nr_t interrupt_nr) {
	assert(interrupt_nr_valid(interrupt_nr));
	REG_STORE(AT91C_AIC_SMR[interrupt_nr], AT91C_AIC_SRCTYPE_INT_EDGE_TRIGGERED);
	REG_ORIN(AT91C_AIC_IECR, 1 << interrupt_nr);
}

void interrupt_disable(interrupt_nr_t interrupt_nr) {
	assert(interrupt_nr_valid(interrupt_nr));
	REG_ORIN(AT91C_AIC_IDCR, 1 << interrupt_nr);
	REG_ANDIN(AT91C_AIC_IECR, ~(1 << interrupt_nr));
}

void interrupt_clear(interrupt_nr_t interrupt_nr) {
	assert(interrupt_nr_valid(interrupt_nr));
	REG_ORIN(AT91C_AIC_ICCR, 1 << interrupt_nr);
}

void interrupt_force(interrupt_nr_t interrupt_nr) {
	assert(interrupt_nr_valid(interrupt_nr));
	REG_STORE(AT91C_AIC_ISCR, 1 << interrupt_nr);
}

interrupt_mask_t interrupt_get_status(void) {
	return REG_LOAD(AT91C_AIC_IMR);
}

