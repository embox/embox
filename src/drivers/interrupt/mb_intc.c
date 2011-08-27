/**
 * @file
 * @brief Low level functions for interrupt controller.
 *
 * @date 23.11.09
 * @author Anton Bondarev
 */

#include <types.h>
#include <bitops.h>

#include <hal/interrupt.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(unit_init);

/**
 * Microblaze interrupt controller registers definitions.
 */
struct irqc_regs {
	/* 0x00 */uint32_t isr; /**< interrupt status register */
	/* 0x04 */uint32_t ipr; /**< interrupt pending register */
	/* 0x08 */uint32_t ier; /**< interrupt enable register */
	/* 0x0C */uint32_t iar; /**< interrupt acknowledge register */
	/* 0x10 */uint32_t sie; /**< set interrupt enable bits */
	/* 0x14 */uint32_t cie; /**< clear interrupt enable bits */
	/* 0x18 */uint32_t ivr; /**< interrupt vector register */
	/* 0x1C */uint32_t mer; /**< master enable register */
};

#define MER_HIE_BIT     30
#define MER_ME_BIT      31

#define MER_HIE         REVERSE_MASK(MER_HIE_BIT)
#define MER_ME          REVERSE_MASK(MER_ME_BIT)

static volatile struct irqc_regs *irqc =
		(struct irqc_regs *) CONFIG_XILINX_INTC_BASEADDR;

static int unit_init(void) {
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

void interrupt_enable(interrupt_nr_t irq_num) {
	set_bit(&irqc->ier, irq_num);
}

void interrupt_disable(interrupt_nr_t irq_num) {
	clear_bit(&irqc->ier, irq_num);
}

//TODO this not set in microblaze
void interrupt_force(interrupt_nr_t irq_num) {

}

void interrupt_clear(interrupt_nr_t irq_num) {
	set_bit(&irqc->iar,irq_num);
}

/*
 * Microblaze specific functions:
 */

__interrupt_mask_t interrupt_get_status(void) {
	return irqc->isr;
}

void irqc_set_mask(__interrupt_mask_t mask) {
	irqc->ier = mask;
}

__interrupt_mask_t irqc_get_mask(void) {
	return irqc->ier;
}
