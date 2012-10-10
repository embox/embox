/**
 * @file
 * @brief
 *
 * @date 10.10.2012
 * @author Anton Bulychev
 */

#include <assert.h>
#include <types.h>

#include <asm/traps.h>
#include <kernel/critical.h>

#include <prom/prom_printf.h>

fastcall void syscall_handler(pt_regs_t *regs) {
	assert(!critical_inside(CRITICAL_IRQ_LOCK));

	critical_enter(CRITICAL_IRQ_HANDLER);
	{
		prom_printf("0x%x\n", (unsigned int) regs->trapno);
	}
	critical_leave(CRITICAL_IRQ_HANDLER);
	critical_dispatch_pending();
}
