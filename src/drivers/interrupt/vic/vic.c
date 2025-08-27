/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 29.08.23
 */
#include <assert.h>

#include <drivers/irqctrl.h>
#include <kernel/critical.h>
#include <kernel/irq.h>
#include <kernel/printk.h>

#include <module/embox/driver/interrupt/vic_impl.h>

#include "vic.h"
#include "vic_util.h"

static int vic_irqctrl_init(void) {
	__irqctrl_init();

	return 0;
}

IRQCTRL_DEF(vic, vic_irqctrl_init);

void irqctrl_enable(unsigned int irq) {
	assert(irq_nr_valid(irq));

	__irqctrl_enable(irq);
}

void irqctrl_disable(unsigned int irq) {
	assert(irq_nr_valid(irq));

	/* Disable software interrupt. If the current interrupt is hardware, it has
	no effect. */
	VIC_REG_STORE(irq / VIC_IRQ_COUNT, VIC_SOFT_DISABLE,
	    1U << (irq % VIC_IRQ_COUNT));

	__irqctrl_disable(irq);
}

void irqctrl_force(unsigned int irq) {
	assert(irq_nr_valid(irq));

	/* Enable software interrupt. */
	VIC_REG_STORE(irq / VIC_IRQ_COUNT, VIC_SOFT_ENABLE,
	    1U << (irq % VIC_IRQ_COUNT));
}

void irqctrl_eoi(unsigned int irq) {
	int i;

	assert(irq_nr_valid(irq));

	/* Writing VIC_ADDR register clears the respective interrupt in the internal
	interrupt priority hardware. */
	for (i = 0; i < VIC_DEVICE_COUNT; i++) {
		VIC_REG_STORE(i, VIC_ADDR, irq);
	}
}

int irqctrl_get_intid(void) {
	/*  Reading VIC_ADDR register updates the hardware priority register of the
	interrupt controller. */
	return VIC_REG_LOAD(0, VIC_ADDR);
}
