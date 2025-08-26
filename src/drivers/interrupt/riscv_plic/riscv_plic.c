/**
 * @file
 *
 * @brief RISC-V build-in interrupt controller
 *
 * @date 04.10.2019
 * @author Anastasia Nizharadze
 */

#include <assert.h>
#include <stdint.h>

#include <asm/interrupts.h>
#include <asm/csr.h>
#include <drivers/irqctrl.h>
#include <embox/unit.h>
#include <hal/reg.h>
#include <hal/cpu.h>

#define PLIC_ADDR                                OPTION_GET(NUMBER, base_addr)
#define PRIORITY_THRESHOLD_OFFSET                OPTION_GET(NUMBER, threshold_offset)
#define INTERRUPT_ENABLE_OFFSRT                  OPTION_GET(NUMBER, interrupt_enable_offset)

/* In case of same priority, lowest ID have the highest effective priority */
#define IPL_ADDR(num)                            (PLIC_ADDR + (num) * 4)

#define PRIORITY_THRESHOLD_ADDR(hart)            \
	(PLIC_ADDR + PRIORITY_THRESHOLD_OFFSET + (hart)*0x1000)

#define CLAIM_COMPLETE_ADDR(hart)                \
	(PLIC_ADDR + PRIORITY_THRESHOLD_OFFSET + 4 + (hart)*0x1000)

#define INTERRUPT_ENABLE_REG(irq_num, hart)      \
	(PLIC_ADDR + INTERRUPT_ENABLE_OFFSRT + ((irq_num)/32)*4 + (hart)*0x80)

/* Configure PLIC for current CPU */
static int plic_init(void) {
	uint32_t hart = cpu_get_id();
	REG32_STORE(PRIORITY_THRESHOLD_ADDR(hart), 0);
	enable_external_interrupts();
	return 0;
}

void irqctrl_enable(unsigned int interrupt_nr) {
	uint32_t reg;
	uint32_t hart = cpu_get_id();

	/* Set up interrupt priorty */
	REG32_STORE(IPL_ADDR(interrupt_nr), 1);

	reg = REG32_LOAD(INTERRUPT_ENABLE_REG(interrupt_nr, hart));
	reg |= (1U << interrupt_nr);
	REG32_STORE(INTERRUPT_ENABLE_REG(interrupt_nr, hart), reg);
}

/* Close the irq signal from outside entirely*/
void irqctrl_disable(unsigned int interrupt_nr) {
	/* 0 means No interrupt PLIC do not detect it */
	REG32_STORE(IPL_ADDR(interrupt_nr), 0);
}

/* Setting that signal will forward as interrupt request to specific CPU */
void irqctrl_enable_in_cpu(uint32_t hartid, unsigned int interrupt_nr) {
	uint32_t reg;

	reg = REG32_LOAD(INTERRUPT_ENABLE_REG(interrupt_nr, hartid));
	reg |= (1U << interrupt_nr);
	REG32_STORE(INTERRUPT_ENABLE_REG(interrupt_nr, hartid), reg);
}

/**
 * Setting that signal will not forward as interrupt request to specific CPU
 * do nothing with whether interrupt signal can be recevied by PLIC
 */
void irqctrl_disable_in_cpu(uint32_t hartid, unsigned int interrupt_nr) {
	uint32_t reg;

	reg = REG32_LOAD(INTERRUPT_ENABLE_REG(interrupt_nr, hartid));
	reg &= ~(1U << interrupt_nr);
	REG32_STORE(INTERRUPT_ENABLE_REG(interrupt_nr, hartid), reg);
}

void irqctrl_eoi(unsigned int irq) {
	uint32_t hart = cpu_get_id();
	REG32_STORE(CLAIM_COMPLETE_ADDR(hart), irq);
}

int irqctrl_get_intid(void) {
	uint32_t hart = cpu_get_id();
	return REG32_LOAD(CLAIM_COMPLETE_ADDR(hart));
}

void irqctrl_set_prio(unsigned int interrupt_nr, unsigned int prio) {
	REG32_STORE(IPL_ADDR(interrupt_nr), prio);
}

unsigned int irqctrl_get_prio(unsigned int interrupt_nr) {
	return REG32_LOAD(IPL_ADDR(interrupt_nr));
}

IRQCTRL_DEF(riscv_plic, plic_init);
