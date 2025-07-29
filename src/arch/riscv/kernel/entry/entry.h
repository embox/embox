/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 28.07.23
 */

#ifndef RICSV_KERNEL_ENTRY_H_
#define RICSV_KERNEL_ENTRY_H_

#include <asm/ptrace.h>
#include <hal/test/traps_core.h>

typedef struct excpt_context {
	struct pt_regs ptregs;
} excpt_context_t;

extern trap_handler_t riscv_excpt_table[0x10];

#endif /* RICSV_KERNEL_ENTRY_H_ */
