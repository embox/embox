/**
 * @file
 * @brief
 *
 * @date 06.01.11
 * @author Anton Bondarev
 */

#include <assert.h>
#include <types.h>

#include <asm/io.h>
#include <asm/regs.h>
#include <asm/traps.h>
#include <drivers/irqctrl.h>
#include <drivers/i8259.h>
#include <hal/reg.h>
#include <kernel/irq.h>

fastcall void irq_handler(pt_regs_t *regs) {
	assert(!critical_inside(CRITICAL_IRQ_LOCK));

	critical_enter(CRITICAL_IRQ_HANDLER);
	{
		int irq = regs->trapno - 0x20;

		irqctrl_disable(irq);
		ipl_enable();

		i8259_send_eoi(irq);

		irq_dispatch(irq);

		ipl_disable();
		irqctrl_enable(irq);
	}
	critical_leave(CRITICAL_IRQ_HANDLER);
	critical_dispatch_pending();
}
