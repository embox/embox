/**
 * @file
 * @brief Low level functions for interrupt controller.
 *
 * @date 23.11.09
 * @author Anton Bondarev
 */

#include <stdint.h>

#include <drivers/common/memory.h>

#include <asm/bitops.h>
#include <hal/reg.h>

#include <drivers/irqctrl.h>

#include <embox/unit.h>

#include <module/embox/driver/interrupt/mb_intc.h>

EMBOX_UNIT_INIT(unit_init);

#define CONFIG_XILINX_INTC_BASEADDR OPTION_GET(NUMBER,mbintc_base)

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

void irqctrl_enable(unsigned int irq) {
	REG_STORE(&irqc->sie, 1UL << irq);
}

void irqctrl_disable(unsigned int irq) {
	REG_STORE(&irqc->cie, 1UL << irq);
}

//TODO this not set in microblaze
void irqctrl_force(unsigned int irq_num) {
}

void irqctrl_clear(unsigned int irq) {
	REG_STORE(&irqc->iar, 1UL << irq);
}

int irqctrl_pending(unsigned int irq) {
	return REG_LOAD(&irqc->isr) & (1 << irq);
}

/* microblaze specific */
unsigned int irqctrl_get_irq_num(void) {
	return REG_LOAD(&irqc->ivr);
}

PERIPH_MEMORY_DEFINE(mb_intc, CONFIG_XILINX_INTC_BASEADDR, sizeof(struct irqc_regs));
