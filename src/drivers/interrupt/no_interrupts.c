/**
 * @file
 * @brief No interrupts.
 *
 * @date 25.03.2016
 * @author Anton Kozlov
 */

#include <drivers/irqctrl.h>

void interrupt_handle(void) {
}

void swi_handle(void) {
}

static int no_interrupt_init(void) {
	return 0;
}

IRQCTRL_DEF(no_interrupt, no_interrupt_init);
