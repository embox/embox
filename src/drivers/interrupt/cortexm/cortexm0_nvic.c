/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    02.07.2012
 */

#include <hal/reg.h>
#include <drivers/irqctrl.h>
#include <kernel/irq.h>

#define NVIC_BASE          0xe000e100
#define NVIC_ENABLE_BASE   NVIC_BASE
#define NVIC_CLEAR_BASE    (NVIC_BASE + 0x80)
#define NVIC_SET_PEND_BASE (NVIC_BASE + 0x100)
#define NVIC_CLR_PEND_BASE (NVIC_BASE + 0x180)
#define NVIC_PRIORITY_BASE (NVIC_BASE + 0x300)

void irqctrl_enable(unsigned int interrupt_nr) {
	int nr = (int) interrupt_nr - 16;
	if (nr >= 0) {
		REG_STORE(NVIC_ENABLE_BASE + 4 * (nr / 32), 1 << (nr % 32));
	}
}

void irqctrl_disable(unsigned int interrupt_nr) {
	int nr = (int) interrupt_nr - 16;
	if (nr >= 0) {
		REG_STORE(NVIC_CLEAR_BASE + 4 * (nr / 32), 1 << (nr % 32));
	}
}

void irqctrl_clear(unsigned int interrupt_nr) {
	int nr = (int) interrupt_nr - 16;
	if (nr >= 0) {
		REG_STORE(NVIC_CLR_PEND_BASE + 4 * (nr / 32), 1 << (nr % 32));
	}
}

void irqctrl_force(unsigned int interrupt_nr) {
	int nr = (int) interrupt_nr - 16;
	if (nr >= 0) {
		REG_STORE(NVIC_SET_PEND_BASE + 4 * (nr / 32), 1 << (nr % 32));
	}
}

void irqctrl_set_prio(unsigned int interrupt_nr, unsigned int prio) {
	int nr = (int) interrupt_nr - 16;

	if (prio > 15) {
		return;
	}
	/* In NVIC the lower priopity means higher IRQ prioriry. */
	prio = 15 - prio;

	if (nr >= 0) {
		REG8_STORE(NVIC_PRIORITY_BASE + nr,
			((prio << NVIC_PRIO_SHIFT) & 0xff));
	}
}

unsigned int irqctrl_get_prio(unsigned int interrupt_nr) {
	int nr = (int) interrupt_nr - 16;
	if (nr >= 0) {
		/* In NVIC the lower priopity means higher IRQ prioriry. */
		return 15 - REG8_LOAD(NVIC_PRIORITY_BASE + nr);
	}
	return 0;
}
