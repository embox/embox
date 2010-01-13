/**
 * @file mb_irq_ctrl.c
 *
 * @brief Low level functions for interrupt controller
 *
 * @date 23.11.2009
 * @author: Anton Bondarev
 */

#include <autoconf.h>
#include <types.h>
#include <hal/irq_ctrl.h>
#include <common.h>

/** microblaze interrupt controller registers definitions */
typedef volatile struct irqc_regs {
	uint32_t isr;                               /**< interrupt status register */
	uint32_t ipr;                               /**< interrupt pending register */
	uint32_t ier;                               /**< interrupt enable register */
	uint32_t iar;                               /**< interrupt acknowledge register */
	uint32_t sie;                               /**< set interrupt enable bits */
	uint32_t cie;                               /**< clear interrupt enable bits */
	uint32_t ivr;                               /**< interrupt vector register */
	uint32_t mer;                               /**< master enable register */
} irqc_regs_t;

#define MER_HIE_BIT     30
#define MER_ME_BIT      31

/*It's necessary put 31 here because microblaze have bit reverse*/
#define REVERSE_MASK(bit_num) (1<<(31-bit_num))

#define MER_HIE              REVERSE_MASK(MER_HIE_BIT)
#define MER_ME               REVERSE_MASK(MER_ME_BIT)

static irqc_regs_t * irqc = (irqc_regs_t * )XILINX_INTC_BASEADDR;

int irqc_init(){
	irqc->mer = 0;
	irqc->ier = 0;
	irqc->iar = ~(0x0);
	/* after set HIE bit we will not can clear it again.
	 * It's a problem because we will couldn't testing interrupts by soft
	 * (we will could use function irqc_force).
	 */
	irqc->mer = MER_HIE | MER_ME;
	return 0;
}

int irqc_enable_irq(irq_num_t irq_num){
	/*save previous state of bit*/
	irq_mask_t old_mask = irqc->ier;
	set_bit(irqc->ier, irq_num);
	return get_bit(old_mask, irq_num);
}

int irqc_disable_irq(irq_num_t irq_num){
	/*save previous state of bit*/
	irq_mask_t old_mask = irqc->ier;
	clear_bit(irqc->ier, irq_num);
	return get_bit(old_mask, irq_num);
}

int irqc_get_status(irq_num_t irq_num){
	return get_bit(irqc->ier, irq_num);
}

irq_mask_t irqc_set_mask(irq_mask_t mask){
	irq_mask_t old_mask = irqc->ier;
	irqc->ier = mask;
	return old_mask;
}

irq_mask_t irqc_get_mask(){
	return irqc->ier;
}

int irqc_disable_all(){
	irqc->mer &= ~MER_ME;
	return 0;
}

//TODO this not set in microblaze
int irqc_force(irq_num_t irq_num){
	return 0;
}

int irqc_clear(irq_num_t irq_num){
	irq_mask_t old_mask = irqc->isr;
	set_bit(irqc->iar,irq_num);
	return old_mask;
}

irq_mask_t irqc_get_isr_reg() {
	return irqc->isr;
}
