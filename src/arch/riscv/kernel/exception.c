/**
 * @file
 *
 * @brief
 *
 * @date 05.12.2019
 * @author Nastya Nizharadze
 */
#include <asm/ptrace.h>
#include <kernel/printk.h>
#include <riscv/exception.h>
#include <hal/test/traps_core.h>

trap_handler_t riscv_excpt_table[0x10];

void riscv_exception_handler(struct excpt_context *ctx, unsigned long mcause) {
	if (riscv_excpt_table[mcause]) {
		riscv_excpt_table[mcause](mcause, ctx);
	}
	else {
		printk("\nUnresolvable exception!\n");
		printk("mcause = %lu\n", mcause);
		PRINT_PTREGS(&ctx->ptregs);
		while (1)
			;
	}
}
