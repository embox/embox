/**
 * @file
 *
 * @brief
 *
 * @date 05.12.2019
 * @author Nastya Nizharadze
 */

#include <assert.h>

#include <asm/entry.h>
#include <asm/interrupts.h>
#include <asm/ptrace.h>
#include <asm/regs.h>
#include <embox/unit.h>
#include <kernel/irq.h>
#include <kernel/printk.h>

EMBOX_UNIT_INIT(riscv_interrupt_init);

#define CLEAN_IRQ_BIT (~(1u << 31))

void riscv_interrupt_handler(void) {
	assert(!critical_inside(CRITICAL_IRQ_LOCK));

	critical_enter(CRITICAL_IRQ_HANDLER);
	{

	}
	critical_leave(CRITICAL_IRQ_HANDLER);
	critical_dispatch_pending();
}

static int riscv_interrupt_init(void) {
	enable_interrupts();

	return 0;
}
