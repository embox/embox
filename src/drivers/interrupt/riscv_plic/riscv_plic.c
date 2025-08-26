/**
 * @file
 * @brief Platform-level interrupt controller (PLIC)
 *
 * @date 04.10.2019
 * @author Anastasia Nizharadze
 */

#include <asm/csr.h>
#include <drivers/irqctrl.h>
#include <framework/mod/options.h>
#include <hal/cpu.h>
#include <hal/reg.h>

#define PLIC_BASE OPTION_GET(NUMBER, base_addr)

#ifdef SMP
#define __PLIC_HARTID cpu_get_id()
#else
#define __PLIC_HARTID 0
#endif

/**
 * Context is an abstract representation of
 * the hart x under privilege level y
 */
#if RISCV_SMODE
#define PLIC_CONTEXT (1 + __PLIC_HARTID * 2)
#else
#define PLIC_CONTEXT (0 + __PLIC_HARTID * 2)
#endif

/* Source Priority */
#define PLIC_SPR(n) (PLIC_BASE + 4 * (n))
/* Source Pending Bits */
#define PLIC_SPB(n) (PLIC_BASE + 0x1000 + 4 * (n))

/* Context Interrupt Enable */
#define PLIC_CIE(n) (PLIC_BASE + 0x2000 + PLIC_CONTEXT * 0x80 + 4 * (n))
/* Context Threshold */
#define PLIC_CTH    (PLIC_BASE + 0x200000 + PLIC_CONTEXT * 0x1000)
/* Context Claim/Complete */
#define PLIC_CCL    (PLIC_BASE + 0x200004 + PLIC_CONTEXT * 0x1000)

static int plic_init(void) {
	/* Configure PLIC for current context */
	REG32_STORE(PLIC_CTH, 0);

	csr_set(CSR_IE, CSR_IE_EIE);

	return 0;
}

void irqctrl_enable(unsigned int irq) {
	if (irq < PLIC_IRQS_TOTAL) {
		/* Set up interrupt priorty */
		REG32_STORE(PLIC_SPR(irq), 1);

		REG32_ORIN(PLIC_CIE(irq / 32), 1 << irq);
	}
	else {
		csr_set(CSR_IE, 1 << (irq - PLIC_IRQS_TOTAL));
	}
}

void irqctrl_disable(unsigned int irq) {
	if (irq < PLIC_IRQS_TOTAL) {
		REG32_CLEAR(PLIC_CIE(irq / 32), 1 << irq);
	}
	else {
		csr_clear(CSR_IE, 1 << (irq - PLIC_IRQS_TOTAL));
	}
}

void irqctrl_eoi(unsigned int irq) {
	if (irq < PLIC_IRQS_TOTAL) {
		REG32_STORE(PLIC_CCL, irq);
	}
}

int irqctrl_get_intid(void) {
	int irq;

	irq = csr_read(CSR_CAUSE) & CSR_CAUSE_EC;

	if (irq == RISCV_IRQ_EXT) {
		irq = REG32_LOAD(PLIC_CCL);
		if (!irq) {
			irq = -1;
		}
	}
	else {
		irq += PLIC_IRQS_TOTAL;
	}

	return irq;
}

int irqctrl_set_level(unsigned int irq, int level) {
	switch (level) {
	case 1:
		return irq + PLIC_IRQS_TOTAL;
	case 2:
		return irq;
	default:
		return -1;
	}
}

#if 0
void irqctrl_set_prio(unsigned int irq, unsigned int prio) {
	REG32_STORE(PLIC_SPR(irq), prio);
}

unsigned int irqctrl_get_prio(unsigned int irq) {
	return REG32_LOAD(PLIC_SPR(irq));
}
#endif

IRQCTRL_DEF(riscv_plic, plic_init);
