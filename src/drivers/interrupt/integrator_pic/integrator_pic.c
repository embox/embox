/**
 * @file
 *
 * @data 05 aug 2015
 * @author: Anton Bondarev
 */
#include <assert.h>
#include <stdint.h>
#include <sys/mman.h>

#include <drivers/common/memory.h>
#include <drivers/irqctrl.h>
#include <hal/ipl.h>
#include <hal/reg.h>
#include <kernel/critical.h>
#include <kernel/irq.h>
#include <kernel/printk.h>

#define ICU_BASE     0x14000000
/* Registers for interrupt control unit - enable/flag/master */
#define ICU_IRQSTS   (ICU_BASE + 0x00)
#define ICU_IRQENSET (ICU_BASE + 0x08)
#define ICU_IRQENCLR (ICU_BASE + 0x0C)

/**
 * Initialize the PIC
 */
static int integrator_pic_init(void) {
	REG32_STORE(ICU_IRQENCLR, ((1 << IRQCTRL_IRQS_TOTAL) - 1));
	return 0;
}

void irqctrl_enable(unsigned int irq) {
	REG32_ORIN(ICU_IRQENSET, 1 << irq);
}

void irqctrl_disable(unsigned int irq) {
	REG32_STORE(ICU_IRQENCLR, 1 << irq);
}

void irqctrl_force(unsigned int irq) {
}

int irqctrl_pending(unsigned int irq) {
	return 0;
}

/* Sends an EOI (end of interrupt) signal to the PICs. */
void irqctrl_eoi(unsigned int irq) {
}

void interrupt_handle(void) {
	unsigned int stat;
	unsigned int irq;

	stat = REG32_LOAD(ICU_IRQSTS);

	assert(!critical_inside(CRITICAL_IRQ_LOCK));
	for (irq = 0; irq < IRQCTRL_IRQS_TOTAL; irq++) {
		if (stat & (uint32_t)(1 << irq))
			break;
	}

	irqctrl_disable(irq);

	critical_enter(CRITICAL_IRQ_HANDLER);
	{
		ipl_enable();

		irq_dispatch(irq);

		ipl_disable();
	}
	irqctrl_enable(irq);
	critical_leave(CRITICAL_IRQ_HANDLER);
	critical_dispatch_pending();
}

void swi_handle(void) {
	printk("swi!\n");
}

IRQCTRL_DEF(integrator_pic, integrator_pic_init);

PERIPH_MEMORY_DEFINE(icu, ICU_BASE, 0x10);
