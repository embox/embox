/**
 * @file
 * @brief
 *
 * @author  Anton Bondarev
 * @date    05.08.2024
 */

#include <assert.h>
#include <stdint.h>

#include <asm/csr.h>
#include <drivers/irqctrl.h>
#include <framework/mod/options.h>

#define BASE_ADDR OPTION_GET(NUMBER, base_addr)

struct epic_regs {
	volatile uint32_t MASK_EDGE_SET;    /* 0x00 */
	volatile uint32_t MASK_EDGE_CLEAR;  /* 0x04 */
	volatile uint32_t MASK_LEVEL_SET;   /* 0x08 */
	volatile uint32_t MASK_LEVEL_CLEAR; /* 0x0C */
	volatile uint32_t reserved[2];      /* 0x10 */
	volatile uint32_t CLEAR;            /* 0x18 */
	volatile uint32_t STATUS;           /* 0x1C */
	volatile uint32_t RAW_STATUS;       /* 0x20 */
};

#define EPIC_REGS ((struct epic_regs *)(uintptr_t)(BASE_ADDR))

static int epic_init(void) {
	EPIC_REGS->CLEAR = 0xFFFFFFFF;

	csr_set(CSR_IE, CSR_IE_EIE);

	return 0;
}

void irqctrl_enable(unsigned int irq) {
	if (irq < EPIC_IRQS_TOTAL) {
		EPIC_REGS->MASK_EDGE_SET |= (1U << irq);
	}
	else {
		csr_set(CSR_IE, 1 << (irq - EPIC_IRQS_TOTAL));
	}
}

void irqctrl_disable(unsigned int irq) {
	if (irq < EPIC_IRQS_TOTAL) {
		EPIC_REGS->MASK_EDGE_CLEAR |= (1U << irq);
	}
	else {
		csr_clear(CSR_IE, 1 << (irq - EPIC_IRQS_TOTAL));
	}
}

void irqctrl_eoi(unsigned int irq) {
	if (irq < EPIC_IRQS_TOTAL) {
		EPIC_REGS->CLEAR |= (1U << irq);
	}
}

int irqctrl_get_intid(void) {
	int irq;

	irq = csr_read(CSR_CAUSE) & CSR_CAUSE_EC;

	if (irq == RISCV_IRQ_EXT) {
		if (EPIC_REGS->RAW_STATUS == 0) {
			return -1;
		}

		for (irq = 0; irq < EPIC_IRQS_TOTAL; irq++) {
			if (EPIC_REGS->RAW_STATUS & 1 << irq) {
				break;
			}
		}
	}
	else {
		irq += EPIC_IRQS_TOTAL;
	}

	return irq;
}

int irqctrl_set_level(unsigned int irq, int level) {
	switch (level) {
	case 1:
		return irq + EPIC_IRQS_TOTAL;
	case 2:
		return irq;
	default:
		return -1;
	}
}

IRQCTRL_DEF(mikron_epic, epic_init);
