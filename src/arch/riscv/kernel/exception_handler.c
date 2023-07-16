/**
 * @file
 *
 * @brief
 *
 * @date 05.12.2019
 * @author Nastya Nizharadze
 */
#include <stdint.h>

#include <asm/regs.h>
#include <asm/ptrace.h>
#include <kernel/printk.h>

int (*riscv_excpt_table[16])(uint32_t nr, void *data);

void riscv_exception_handler(pt_regs_t *regs, unsigned long mcause) {
	if (riscv_excpt_table[mcause]) {
		riscv_excpt_table[mcause](mcause, regs);
	}
	else {
		printk("\nUnresolvable exception!\n");
		printk("mcause = %lu\n", mcause);
		while (1)
			;
	}
}
