/**
 * @file
 * @brief RISC-V Core Local Interrupt Controller (CLIC)
 *
 * @author Aleksey Zhmulin
 * @date 19.08.25
 */

#include <assert.h>
#include <stdint.h>

#include <asm/csr.h>
#include <asm/interrupts.h>
#include <drivers/irqctrl.h>
#include <hal/cpu.h>
#include <hal/reg.h>

#define CLIC_BASE_ADDR OPTION_GET(NUMBER, base_addr)

#define CLIC_CFG        (CLIC_BASE_ADDR + 0)
#define CLIC_INFO       (CLIC_BASE_ADDR + 4)
#define CLIC_INTIP(n)   (CLIC_BASE_ADDR + 0x1000 + 4 * n)
#define CLIC_INTIE(n)   (CLIC_BASE_ADDR + 0x1001 + 4 * n)
#define CLIC_INTATTR(n) (CLIC_BASE_ADDR + 0x1002 + 4 * n)
#define CLIC_INTCTL(n)  (CLIC_BASE_ADDR + 0x1003 + 4 * n)

static int clic_init(void) {
	int irq;

	/* 4 interrupt levels and M-mode is available */
	REG8_STORE(CLIC_CFG, 2 << 1);

	for (irq = IRQCTRL_IRQS_TOTAL - 1; irq >= 0; irq--) {}

	return 0;
}

void irqctrl_enable(unsigned int irq) {
	/* Select M-mode */
	REG8_STORE(CLIC_INTATTR(irq), 3 << 6);

	/* Set up interrupt priorty */
	REG8_STORE(CLIC_INTCTL(irq), 1 << 4);

	REG8_STORE(CLIC_INTIE(irq), 1);
}

void irqctrl_disable(unsigned int irq) {
	REG8_STORE(CLIC_INTIE(irq), 0);
}

void irqctrl_eoi(unsigned int irq) {
	REG8_STORE(CLIC_INTIP(irq), 0);
}

int irqctrl_get_intid(void) {
	int irq;

	for (irq = 0; irq < IRQCTRL_IRQS_TOTAL; irq++) {
		if (REG8_LOAD(CLIC_INTIP(irq))) {
			return irq;
		}
	}

	return -1;
}

int irqctrl_set_level(unsigned int irq, int level) {
	if (level == 1) {
		return irq;
	}

	return -1;
}

IRQCTRL_DEF(riscv_clic, clic_init);
