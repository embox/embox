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
#include <riscv/exception.h>
#include <util/log.h>

void riscv_exception_handler(unsigned long cause, struct excpt_context *ctx) {
	if (riscv_excpt_table && riscv_excpt_table[cause]) {
		riscv_excpt_table[cause](ctx);
		return;
	}

	log_raw(LOG_EMERG, "\n\n");
	log_emerg("Exception: cause(%#lx) epc(%#lx) ra(%#lx) sp(%#lx)", cause,
	    ctx->epc, ctx->ra, ctx->sp);

	log_info("cpu_id = %#lx", cpu_get_id());
	log_info("status = %#lx", ctx->status);

	while (1) {
		arch_cpu_idle();
	}
}
