/**
 * @file
 *
 * @brief
 *
 * @date 05.12.2019
 * @author Nastya Nizharadze
 */

#include <assert.h>

#include <asm/csr.h>
#include <asm/entry.h>
#include <asm/interrupts.h>
#include <asm/ptrace.h>
#include <hal/cpu.h>
#include <kernel/irq.h>
#include <kernel/panic.h>
#ifdef SMP
#include <riscv/smp_ipi.h>
#endif

#define CLEAN_IRQ_BIT (~(1u << 31))

extern void riscv_mtimer_irq_handler(void) __attribute__((weak));

void riscv_interrupt_handler(unsigned long cause) {
	int irq;

	assert(!critical_inside(CRITICAL_IRQ_LOCK));

	critical_enter(CRITICAL_IRQ_HANDLER);
	{
		if (cause == RISCV_IRQ_TIMER) {
			disable_timer_interrupts();
			if (riscv_mtimer_irq_handler) {
				riscv_mtimer_irq_handler();
			}
			enable_timer_interrupts();
		}
		else if (cause == RISCV_IRQ_EXT) {
			/* the ID of the highest-priority pending interrupt */
			irq = irqctrl_get_intid();
			if (irq >= 0) {
				irqctrl_eoi(irq);
				irqctrl_disable(irq);
				ipl_enable();
				irq_dispatch(irq);
				ipl_disable();
				irqctrl_enable(irq);
			}
		}
		else if (cause == RISCV_IRQ_SOFT) {
			disable_software_interrupts();
#ifdef SMP
			switch (smp_get_ipi_message()) {
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
#endif
			enable_software_interrupts();
		}
	}
	critical_leave(CRITICAL_IRQ_HANDLER);
	critical_dispatch_pending();
}
