/**
 * @file
 *
 * @brief
 *
 * @date 05.12.2019
 * @author Nastya Nizharadze
 */
#include <asm/regs.h>
#include <asm/ptrace.h>
#include <kernel/printk.h>
#include <riscv/exception.h>
#include <hal/test/traps_core.h>

trap_handler_t riscv_excpt_table[0x10];

void riscv_exception_handler(struct excpt_context *ctx, unsigned long cause,
			unsigned long interrupt, unsigned long pending, unsigned long epc,
			unsigned long trap_vector, unsigned long hartid) {
	if (riscv_excpt_table[cause]) {
		riscv_excpt_table[cause](cause, ctx);
	}
	else {
		printk("\nUnresolvable exception!\n");
		printk(MACRO_STRING(HARTID_REG)" = %lu\n", hartid);
		printk(MACRO_STRING(CAUSE_REG)"  = %lu\n", cause);
		printk(MACRO_STRING(EPC_REG)"    = %#lx\n", epc);
		printk(MACRO_STRING(INTERRUPT_REG)"     = %#lx\n", interrupt);
		printk(MACRO_STRING(TRAP_VECTOR_REG)"   = %#lx\n", trap_vector);
		printk(MACRO_STRING(INTPENDING_REG)"     = %#lx\n", pending);
		PRINT_PTREGS(&ctx->ptregs);
		while (1)
			;
	}
}
