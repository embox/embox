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

EMBOX_UNIT_INIT(riscv64_interrupt_init);

#define CLEAN_IRQ_BIT (~(1L << 63))

void riscv64_interrupt_handler(pt_regs_t *regs) {
	assert(!critical_inside(CRITICAL_IRQ_LOCK));

	critical_enter(CRITICAL_IRQ_HANDLER);
	{
		unsigned int pending;
		pending = (read_csr(mcause)) & CLEAN_IRQ_BIT;

		if (pending == MACHINE_TIMER_INTERRUPT) {
			irqctrl_disable(pending);   //disable mie
			ipl_enable();               //enable mstatus.MIE
			if (__riscv_timer_handler) {
				__riscv_timer_handler(0, __riscv_timer_data);
			}
			ipl_disable();              //disable mstatus.MIE
			irqctrl_enable(pending);    //enable mie
		}
	}
	critical_leave(CRITICAL_IRQ_HANDLER);
	critical_dispatch_pending();
}

static int riscv64_interrupt_init(void) {
	enable_interrupts();
	return 0;
}
