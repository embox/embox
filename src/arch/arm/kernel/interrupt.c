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
#include <AT91SAM7S256.h>

//#define make_reg(offset) ((unsigned int *) (AT91C_BASE_AIC + offset))
static AT91PS_AIC dev_regs = AT91C_BASE_AIC;

#if 0
#define AIC_ISR  make_reg(0x108)
#define AIC_IPR	 make_reg(0x10c)
#define AIC_IMR	 make_reg(0x110)
#define AIC_IECR make_reg(0x120)
#define AIC_IDCR make_reg(0x124)
#define AIC_ICCR make_reg(0x128)
#define AIC_ISCR make_reg(0x12c)
#define AIC_DCR	 make_reg(0x138)

//#define PIO_ASR	  0xfffff470
#endif

#define IRQ_BIT  0x80
#define FIQ_BIT	 0x40

//int si_handler(irq_nr_t nr, void *data);

void interrupt_init(void) {
	//__set_cpsr(__get_cpsr() & (~(IRQ_BIT | FIQ_BIT)));
	REG_STORE(AT91C_PMC_PCER, 1 << 30);
	//REG_STORE(AIC_DCR, 1);
	REG_STORE(AT91C_PIOA_PDR, 1 << 20);
	REG_STORE(AT91C_PIOA_BSR, 1 << 20);
	REG_STORE(AT91C_AIC_IDCR, ~0);
	REG_STORE(AT91C_AIC_ICCR, ~0);
	//TODO this code architecture dependent not platform
	__set_cpsr(__get_cpsr() & (~(IRQ_BIT | FIQ_BIT)));
}

void interrupt_enable(interrupt_nr_t interrupt_nr) {
	assert(interrupt_nr_valid(interrupt_nr));
/* TODO arm interrupts what does it mean*/
	REG_STORE((unsigned int *) AT91C_BASE_AIC + interrupt_nr * sizeof(unsigned int), 0x20);
	REG_STORE((unsigned int *) AT91C_BASE_AIC + 0x80 + interrupt_nr * sizeof(unsigned int), 0);

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

