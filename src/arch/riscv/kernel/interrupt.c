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

extern int irqctrl_get_num(void);

void riscv_interrupt_handler(pt_regs_t *regs) {
	assert(!critical_inside(CRITICAL_IRQ_LOCK));

	critical_enter(CRITICAL_IRQ_HANDLER);
	{
		uint32_t pending;
		uint32_t interrupt_id;

		pending = (read_csr(mcause)) & CLEAN_IRQ_BIT;
		interrupt_id = pending;

		if (pending == MACHINE_TIMER_INTERRUPT) {
			disable_timer_interrupts();
			//ipl_enable();               /* enable mstatus.MIE */
			if (__riscv_timer_handler) {
				__riscv_timer_handler(0, __riscv_timer_data);
			}
			//ipl_disable();              /* disable mstatus.MIE */
			enable_timer_interrupts();
		} else if (pending == MACHINE_EXTERNAL_INTERRUPT) {
			/* the ID of the highest-priority pending interrupt */
			interrupt_id = irqctrl_get_num();
			if (interrupt_id == 0) {
				critical_leave(CRITICAL_IRQ_HANDLER);
				return;
			}

			irqctrl_eoi(interrupt_id);
			irqctrl_disable(interrupt_id);
			ipl_enable();
			irq_dispatch(interrupt_id);
			ipl_disable();
			irqctrl_enable(interrupt_id);
		}
	}
	critical_leave(CRITICAL_IRQ_HANDLER);
	critical_dispatch_pending();
}

static int riscv_interrupt_init(void) {
	enable_interrupts();
	return 0;
}
