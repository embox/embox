/**
 * @file
 * @brief
 *
 * @date 05.12.2019
 * @author Nastya Nizharadze
 */

#include <asm/csr.h>
#include <hal/cpu.h>
#include <hal/cpu_idle.h>
#include <hal/test/traps_core.h>
#include <kernel/printk.h>
#include <riscv/entry.h>
#include <util/log.h>

trap_handler_t riscv_excpt_table[0x10];

void riscv_exception_handler(struct excpt_context *ctx, unsigned long cause) {
	if (riscv_excpt_table[cause]) {
		riscv_excpt_table[cause](cause, ctx);
		return;
	}

	log_raw(LOG_EMERG, "\nException: cause(%#lx) epc(%#lx)",
	    csr_read(CSR_CAUSE), ctx->ptregs.pc);

	log_info("cpu_id  = %#lx", cpu_get_id());
	log_info("mstatus = %#lx", ctx->ptregs.mstatus);

	while (1) {
		arch_cpu_idle();
	}
}
