/**
 * @file
 * @brief
 *
 * @date
 * @author
 */

#include <kernel/irq.h>
#include <embox/unit.h>
#include <kernel/printk.h>

#include <drivers/irqctrl.h>

EMBOX_UNIT_INIT(xen_int_init);

static int xen_int_init(void) {
	return 0;
}

void irqctrl_enable(unsigned int irq) {
}

void irqctrl_disable(unsigned int irq) {
}

void irqctrl_force(unsigned int irq) {
}

void interrupt_handle(void) {
}
