/**
 * @file
 *
 * @brief
 *
 * @date 05.12.2019
 * @author Nastya Nizharadze
 */

#include <assert.h>
#include <asm/regs.h>
#include <kernel/irq.h>
#include <asm/ptrace.h>
#include <asm/interrupts.h>
#include <asm/entry.h>

#include <kernel/printk.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(riscv_interrupt_init);

#define CLEAN_IRQ_BIT (~(1 << 31))

void riscv_interrupt_handler(pt_regs_t *regs) {
	assert(!critical_inside(CRITICAL_IRQ_LOCK));

	critical_enter(CRITICAL_IRQ_HANDLER);
	{
		unsigned int pending;
		pending = (read_csr(mcause)) & CLEAN_IRQ_BIT;

		irqctrl_disable(pending);
		ipl_enable();
		irq_dispatch(pending);
		ipl_disable();
		irqctrl_enable(pending);

	}
	critical_leave(CRITICAL_IRQ_HANDLER);
	critical_dispatch_pending();
}

static int riscv_interrupt_init(void) {
	enable_interrupts();
	return 0;
}
