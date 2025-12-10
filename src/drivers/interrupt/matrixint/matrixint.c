/**
 * @file
 *
 * @author Efim Perevalov
 * @date 8.12.2025
 */
#include <drivers/irqctrl.h>
#include <framework/mod/options.h>
#include <hal/reg.h>

#define BASE_ADDR OPTION_GET(NUMBER, base_addr)

#define INTERRUPT_CORE0_INTR_STATUS_0 	(BASE_ADDR + 0x00F8)
#define INTERRUPT_CORE0_CPU_INT_ENABLE	(BASE_ADDR + 0x0104)
#define mcause 							(BASE_ADDR + 0x342)
#define mtvec 							(BASE_ADDR + 0x305)

void irqctrl_enable(unsigned int irq) {
	REG32_ORIN(INTERRUPT_CORE0_CPU_INT_ENABLE, 1 << irq);
}

void irqctrl_disable(unsigned int irq) {
	REG32_CLEAR(INTERRUPT_CORE0_CPU_INT_ENABLE, 1 << irq);
}

// void irqctrl_eoi(unsigned int irq) {
// }

int irqctrl_get_intid(void) {
	return (REG32_LOAD(mcause) - mtvec) / 4;
}

int irqctrl_set_level(unsigned int irq, int level) {
	return 0;
}

static int matrixint_init(void) {
	REG32_STORE(INTERRUPT_CORE0_CPU_INT_ENABLE, 0);
	return 0;
}

IRQCTRL_DEF(matrixint, matrixint_init);
