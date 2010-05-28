/**
 * @file
 *
 * @brief Low level functions for interrupt controller
 *
 * @date 23.11.2009
 * @author: Anton Bondarev
 */

#include <types.h>
#include <bitops.h>
#include <hal/interrupt.h>
#include <embox/kernel.h>
#include <asm/cpu_conf.h>

/**
 * Microblaze interrupt controller registers definitions.
 */
typedef volatile struct irqc_regs {
	uint32_t isr; /**< interrupt status register */
	uint32_t ipr; /**< interrupt pending register */
	uint32_t ier; /**< interrupt enable register */
	uint32_t iar; /**< interrupt acknowledge register */
	uint32_t sie; /**< set interrupt enable bits */
	uint32_t cie; /**< clear interrupt enable bits */
	uint32_t ivr; /**< interrupt vector register */
	uint32_t mer; /**< master enable register */
} irqc_regs_t;

#define MER_HIE_BIT     30
#define MER_ME_BIT      31

#define MER_HIE              REVERSE_MASK(MER_HIE_BIT)
#define MER_ME               REVERSE_MASK(MER_ME_BIT)

static irqc_regs_t * irqc = (irqc_regs_t * )XILINX_INTC_BASEADDR;

void interrupt_init(void){
	irqc->mer = 0;
	irqc->ier = 0;
	irqc->iar = ~(0x0);
	/* after set HIE bit we will not can clear it again.
	 * It's a problem because we will couldn't testing interrupts by soft
	 * (we will could use function irqc_force).
	 */
	irqc->mer = MER_HIE | MER_ME;
}

void interrupt_enable(interrupt_nr_t irq_num){
	set_bit(&irqc->ier, irq_num);
}

void interrupt_disable(interrupt_nr_t irq_num){
	clear_bit(&irqc->ier, irq_num);
}



//TODO this not set in microblaze
void interrupt_force(interrupt_nr_t irq_num){

}

void interrupt_clear(interrupt_nr_t irq_num){
	set_bit(&irqc->iar,irq_num);
}

/*
 * Microblaze specific functions:
 */

__interrupt_mask_t irqc_get_isr_reg(void) {
	return irqc->isr;
}

void irqc_set_mask(__interrupt_mask_t mask){
	irqc->ier = mask;
}

__interrupt_mask_t irqc_get_mask(void){
	return irqc->ier;
}

/* Following functions never be used */
#if 0
int irqc_get_status(interrupt_nr_t irq_num){
	return get_bit(irqc->ier, irq_num);
}

int irqc_disable_all(void){
	irqc->mer &= ~MER_ME;
	return 0;
}
#endif
