/**
 * @file
 * @brief
 *
 * @date 06.11.22
 * @author Aleksey Zhmulin
 */

#include <inttypes.h>

#include <arm/fpu.h>
#include <asm/ptrace.h>
#include <kernel/printk.h>

typedef struct excpt_context {
	fpu_context_t fpu_context;
	pt_regs_t ptregs;
} excpt_context_t;

#define PRINT_PTREGS(ptregs)                                                 \
	printk("r0   = %#" PRIx32 "\n"                                           \
	       "r1   = %#" PRIx32 "\n"                                           \
	       "r2   = %#" PRIx32 "\n"                                           \
	       "r3   = %#" PRIx32 "\n"                                           \
	       "r4   = %#" PRIx32 "\n"                                           \
	       "r5   = %#" PRIx32 "\n"                                           \
	       "r6   = %#" PRIx32 "\n"                                           \
	       "r7   = %#" PRIx32 "\n"                                           \
	       "r8   = %#" PRIx32 "\n"                                           \
	       "r9   = %#" PRIx32 "\n"                                           \
	       "r10  = %#" PRIx32 "\n"                                           \
	       "r11  = %#" PRIx32 "\n"                                           \
	       "r12  = %#" PRIx32 "\n"                                           \
	       "sp   = %#" PRIx32 "\n"                                           \
	       "lr   = %#" PRIx32 "\n"                                           \
	       "pc   = %#" PRIx32 "\n"                                           \
	       "cpsr = %#" PRIx32 "\n",                                          \
	       (ptregs)->r[0], (ptregs)->r[1], (ptregs)->r[2], (ptregs)->r[3],   \
	       (ptregs)->r[4], (ptregs)->r[5], (ptregs)->r[6], (ptregs)->r[7],   \
	       (ptregs)->r[8], (ptregs)->r[9], (ptregs)->r[10], (ptregs)->r[11], \
	       (ptregs)->r[12], (ptregs)->sp, (ptregs)->lr, (ptregs)->pc,        \
	       (ptregs)->cpsr)
