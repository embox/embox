/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 28.07.23
 */

#ifndef RICSV_KERNEL_ENTRY_H_
#define RICSV_KERNEL_ENTRY_H_

#include <asm/context.h>

struct excpt_context {
	struct caller_saved_regs r;
	unsigned long status;
	unsigned long epc;
	unsigned long ra;
	unsigned long sp;
};

typedef void (*excpt_handler)(struct excpt_context *ctx);

extern excpt_handler riscv_excpt_table[0x10] __attribute__((weak));

#endif /* RICSV_KERNEL_ENTRY_H_ */
