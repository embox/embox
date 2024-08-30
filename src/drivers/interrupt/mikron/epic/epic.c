/**
 * @file
 *
 * @date 11.07.10
 * @author Anton Kozlov
 */

#include <assert.h>
#include <stdint.h>

#include <asm/interrupts.h>
#include <drivers/irqctrl.h>

#include <framework/mod/options.h>

struct epic_regs {  
	volatile uint32_t MASK_EDGE_SET;        /* 0x00 */
	volatile uint32_t MASK_EDGE_CLEAR;      /* 0x04 */
	volatile uint32_t MASK_LEVEL_SET;       /* 0x08 */
	volatile uint32_t MASK_LEVEL_CLEAR;     /* 0x0C */
	volatile uint32_t reserved[2];
	volatile uint32_t CLEAR;                /* 0x18 */
	volatile uint32_t STATUS;               /* 0x1C */
	volatile uint32_t RAW_STATUS;           /* 0x20 */
};

#define BASE_ADDR            OPTION_GET(NUMBER, base_addr)

#define EPIC_REGS   ((struct epic_regs *)(uintptr_t)(BASE_ADDR))

static int epic_init(void) {
	EPIC_REGS->CLEAR = 0xFFFFFFFF;
	enable_external_interrupts();
	return 0;
}

void irqctrl_enable(unsigned int interrupt_nr) {
	EPIC_REGS->MASK_EDGE_SET |= (1U << interrupt_nr);
}

void irqctrl_disable(unsigned int interrupt_nr) {
	EPIC_REGS->MASK_EDGE_CLEAR |= (1U << interrupt_nr);
}

void irqctrl_eoi(unsigned int irq) {
	EPIC_REGS->CLEAR |= (1U << irq);
}

unsigned int irqctrl_get_intid(void) {
	int i;

	if (EPIC_REGS->RAW_STATUS == 0) {
		return 0;
	}

	for (i = 0; i < 32; i ++) {
		if (EPIC_REGS->RAW_STATUS & 1 << i) {
			break;
		}
	}

	return i;
}

IRQCTRL_DEF(mikron_epic, epic_init);
