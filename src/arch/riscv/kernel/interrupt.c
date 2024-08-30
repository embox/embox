/**
 * @file
 *
 * @brief
 *
 * @date 05.12.2019
 * @author Nastya Nizharadze
 */

#include <assert.h>

#include <hal/cpu.h>
#include <asm/entry.h>
#include <asm/interrupts.h>
#include <asm/ptrace.h>
#include <asm/regs.h>
#include <embox/unit.h>
#include <kernel/irq.h>
#include <kernel/panic.h>
#include <riscv/ipi.h>

EMBOX_UNIT_INIT(riscv_interrupt_init);

#define CLEAN_IRQ_BIT (~(1u << 31))

void riscv_interrupt_handler(void) {
	assert(!critical_inside(CRITICAL_IRQ_LOCK));

	critical_enter(CRITICAL_IRQ_HANDLER);
	{
		long pending;
		long interrupt_id;

		pending = (read_csr(CAUSE_REG)) & CLEAN_IRQ_BIT;
		interrupt_id = pending;

		if (pending == IRQ_TIMER) {
			disable_timer_interrupts();
			//ipl_enable();               /* enable mstatus.MIE */
			if (__riscv_timer_handler) {
				if(cpu_get_id() == 0) {
					__riscv_timer_handler(0, __riscv_timer_data);
				}else {
#ifdef SMP
					extern void __riscv_ap_timer_handler(void* dev);
					__riscv_ap_timer_handler(__riscv_timer_data);
#endif
				}
			}
			//ipl_disable();              /* disable mstatus.MIE */
			enable_timer_interrupts();
		}
		else if (pending == IRQ_EXTERNAL) {
			/* the ID of the highest-priority pending interrupt */
			interrupt_id = irqctrl_get_intid();
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
		}else if (pending == IRQ_SOFTWARE) {
			disable_software_interrupts();
			switch(smp_get_ipi_message()) {
				case NONE:
					smp_ack_ipi();
					break;
				case RESCHED:
					smp_ack_ipi();
					resched();
					break;
				default:
					panic("unknown software interrupt\n");
					break;
			}
			enable_software_interrupts();
		}
	}
	critical_leave(CRITICAL_IRQ_HANDLER);
	critical_dispatch_pending();
}

static int riscv_interrupt_init(void) {
	enable_interrupts();

	return 0;
}
