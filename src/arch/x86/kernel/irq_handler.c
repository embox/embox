/**
 * @file
 * @brief
 *
 * @date 06.01.11
 * @author Anton Bondarev
 */

#include <assert.h>

#include <asm/traps.h>
#include <asm/linkage.h>
#include <drivers/irqctrl.h>
#include <kernel/irq.h>

fastcall void irq_handler(pt_regs_t *regs) {
	int irq = regs->trapno - 0x20;
	assert(!critical_inside(CRITICAL_IRQ_LOCK));

	critical_enter(CRITICAL_IRQ_HANDLER);
	{
		irqctrl_disable(irq);

		ipl_enable();

		irq_dispatch(irq);

		/* next lines ordered this way thats why.
 		 * On eoi current irq unmasked and may occur again right there,
		 * on irq stack. It may repeat till stack exhaustion.
		 * Disabling ipl first prevents irq handling of same or lower
		 * level till switched to lower critical level.
		 */

		ipl_disable();

		irqctrl_eoi(irq);

		irqctrl_enable(irq);
	}
	critical_leave(CRITICAL_IRQ_HANDLER);
	critical_dispatch_pending();
}
