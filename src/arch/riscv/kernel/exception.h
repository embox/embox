/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 28.07.23
 */
#include <asm/ptrace.h>
#include <asm/regs.h>
#include <kernel/printk.h>
#include <hal/test/traps_core.h>

typedef struct excpt_context {
	struct pt_regs ptregs;
} excpt_context_t;

extern trap_handler_t riscv_excpt_table[0x10];

#define PRINT_PTREGS(ptregs)                                    \
	printk(MACRO_STRING(STATUS_REG) " = %#lx\n"                 \
	       "ra      = %#lx\n"                                   \
	       "sp      = %#lx\n"                                   \
	       "gp      = %#lx\n"                                   \
	       "tp      = %#lx\n"                                   \
	       "pc      = %#lx\n",                                  \
	    (ptregs)->mstatus, (ptregs)->ra, (ptregs)->sp,          \
	    (ptregs)->gp, (ptregs)->tp, (ptregs)->pc)
