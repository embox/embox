/**
 * @file
 * @brief No interrupts.
 *
 * @date 25.03.2016
 * @author Anton Kozlov
 */

#include <drivers/irqctrl.h>

void irqctrl_enable(unsigned int irq) {
}

void irqctrl_disable(unsigned int irq) {
}

void irqctrl_clear(unsigned int irq) {
}

void irqctrl_force(unsigned int irq) {
}

int irqctrl_pending(unsigned int irq) {
	return 0;
}

void irqctrl_eoi(unsigned int irq) {
}

void irqctrl_set_prio(unsigned int interrupt_nr, unsigned int prio) {
}

unsigned int irqctrl_get_prio(unsigned int interrupt_nr) {
	return 0;
}

unsigned int irqctrl_get_intid(void) {
	return 0;
}

static int no_interrupt_init(void) {
	return 0;
}

IRQCTRL_DEF(no_interrupt, no_interrupt_init);
