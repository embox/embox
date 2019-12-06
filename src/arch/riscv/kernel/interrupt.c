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

#include <embox/unit.h>

EMBOX_UNIT_INIT(riscv_interrupt_init);

void riscv_interrupt_handler(pt_regs_t *regs) {
	assert(!critical_inside(CRITICAL_IRQ_LOCK));

	critical_enter(CRITICAL_IRQ_HANDLER);
	{
		unsigned int pending;
		unsigned int irq;
		pending = read_csr(mcause);

		for (irq = 11; irq > 1; irq -= 2) {
			if (pending & (1 << irq)) {
				ipl_enable();
				irq_dispatch((irq - 1) / 2);
				ipl_disable();
			}
		}

	}
	critical_leave(CRITICAL_IRQ_HANDLER);
	critical_dispatch_pending();
}

static int riscv_interrupt_init(void) {
	write_csr(mcause, 0);
	write_csr(mip, 0);
/*
	enable_timer_interrupts();
	enable_external_interrupts();
	enable_software_interrupts();
*/
	enable_interrupts();

	return 0;
}
