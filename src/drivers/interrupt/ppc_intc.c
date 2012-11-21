/**
 * @file
 * @brief PowerPC Microprocessor Family interrupt controller
 *
 * @date 06.11.12
 * @author Ilia Vaprol
 */

#include <asm/psr.h>
#include <asm/regs.h>
#include <assert.h>
#include <drivers/irqctrl.h>
#include <kernel/critical.h>
#include <kernel/irq.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(ppc_intc_init);

static int ppc_intc_init(void) {
	return 0;
}

void irqctrl_enable(unsigned int interrupt_nr) {
}

void irqctrl_disable(unsigned int interrupt_nr) {
}

void irqctrl_clear(unsigned int interrupt_nr) {
}

void irqctrl_force(unsigned int interrupt_nr) {
}

#include <prom/prom_printf.h>
void interrupt_handle(void) {
	unsigned int irq;
	unsigned int sprg2;

#if 1 /* interrupt nest count test (same as to !critical_inside) */
	__asm__ __volatile__ ("mfsprg2 %0" : "=r"(sprg2));
	if(sprg2!=1)prom_printf("sprg2 %u\n", sprg2);
	assert(sprg2==1);
#endif

	irq = 10; /* LOL */

	assert(!critical_inside(CRITICAL_IRQ_LOCK));

	irqctrl_disable(irq);

	irqctrl_clear(irq);

	critical_enter(CRITICAL_IRQ_HANDLER);
	{
//		ipl_enable();

		irq_dispatch(irq);

//		ipl_disable();
	}
	irqctrl_enable(irq);
	critical_leave(CRITICAL_IRQ_HANDLER);
	critical_dispatch_pending();
}

