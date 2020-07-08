/**
 * @file
 *
 * @brief RISC-V build-in interrupt controller
 *
 * @date 04.10.2019
 * @author Anastasia Nizharadze
 */

#include <stdint.h>
#include <assert.h>

#include <hal/reg.h>

#include <asm/regs.h>
#include <asm/interrupts.h>

#include <drivers/irqctrl.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(plic_init);

#define PLIC_ADDR                 OPTION_GET(NUMBER, base_addr)

#define IPL_ADDR(num)            (PLIC_ADDR + (num * 4))

#define PRIORITY_THRESHOLD_ADDR  (PLIC_ADDR + 0x200000U)
#define CLAIM_COMPLETE_ADDR      (PLIC_ADDR + 0x200004U) /* offset for hart 0 claim/complere reg */
#define INTERRUPT_ENABLE_REG_1   (PLIC_ADDR + 0x2000U)
#define INTERRUPT_ENABLE_REG_2   (PLIC_ADDR + 0x2004U)


static int plic_init(void) {
	REG32_STORE(PRIORITY_THRESHOLD_ADDR, 0);

	enable_external_interrupts();
	return 0;
}

int irqctrl_get_num(void) {
	uint32_t claim;

	claim = REG32_LOAD(CLAIM_COMPLETE_ADDR);

	return claim;
}

void irqctrl_enable(unsigned int interrupt_nr) {
	uint32_t reg;

	REG32_STORE(IPL_ADDR(interrupt_nr), 1);

	if (interrupt_nr < 32) {
		reg = REG32_LOAD(INTERRUPT_ENABLE_REG_1);
		reg |= (1U << interrupt_nr);
		REG32_STORE(INTERRUPT_ENABLE_REG_1, reg);
	} else {
		reg = REG32_LOAD(INTERRUPT_ENABLE_REG_2);
		reg |= (1U << (interrupt_nr - 32));
		REG32_STORE(INTERRUPT_ENABLE_REG_2, reg);
	}
}

void irqctrl_disable(unsigned int interrupt_nr) {
	uint32_t reg;

	if (interrupt_nr < 32) {
		reg = REG32_LOAD(INTERRUPT_ENABLE_REG_1);
		reg &= ~(1U << interrupt_nr);
		REG32_STORE(INTERRUPT_ENABLE_REG_1, reg);
	} else {
		reg = REG32_LOAD(INTERRUPT_ENABLE_REG_2);
		reg &= ~(1U << (interrupt_nr - 32));
		REG32_STORE(INTERRUPT_ENABLE_REG_2, reg);
	}
}

void irqctrl_eoi(unsigned int irq) {
	REG32_STORE(CLAIM_COMPLETE_ADDR, irq);
}
