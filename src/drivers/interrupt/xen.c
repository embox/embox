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
#include <xen/event.h>

extern shared_info_t xen_shared_info;
extern void do_divide_error(void);

EMBOX_UNIT_INIT(xen_int_init);

static int xen_int_init(void) {
	return 0;
}

void irqctrl_enable(unsigned int irq) {
}

void irqctrl_disable(unsigned int irq) {
	printk("in disable\n");
}

void irqctrl_force(unsigned int irq) {
	printk("xen force interrupt %d\n", irq);
	int x = 0;
	int y = 100 / x; /* make div-by-zero execption */
	printk("%d\n", y); /* avoid optimizing out by compiler */
}

void interrupt_handle(void) {
}
