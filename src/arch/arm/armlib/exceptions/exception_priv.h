/**
 * @file
 * @brief
 *
 * @date 06.11.22
 * @author Aleksey Zhmulin
 */

#include <framework/mod/options.h>

#define KEEP_GOING OPTION_GET(BOOLEAN, keep_going)

#ifndef __ASSEMBLER__
#include <inttypes.h>

#include <asm/ptrace.h>
#include <kernel/printk.h>

#define PRINT_PTREGS(pt_regs)                                         \
	printk("r0   = %#08" PRIx32 "\n"                                  \
	       "r1   = %#08" PRIx32 "\n"                                  \
	       "r2   = %#08" PRIx32 "\n"                                  \
	       "r3   = %#08" PRIx32 "\n"                                  \
	       "r4   = %#08" PRIx32 "\n"                                  \
	       "r5   = %#08" PRIx32 "\n"                                  \
	       "r6   = %#08" PRIx32 "\n"                                  \
	       "r7   = %#08" PRIx32 "\n"                                  \
	       "r8   = %#08" PRIx32 "\n"                                  \
	       "r9   = %#08" PRIx32 "\n"                                  \
	       "r10  = %#08" PRIx32 "\n"                                  \
	       "r11  = %#08" PRIx32 "\n"                                  \
	       "r12  = %#08" PRIx32 "\n"                                  \
	       "sp   = %#08" PRIx32 "\n"                                  \
	       "lr   = %#08" PRIx32 "\n"                                  \
	       "pc   = %#08" PRIx32 "\n"                                  \
	       "cpsr = %#08" PRIx32 "\n",                                 \
	    pt_regs->r[0], pt_regs->r[1], pt_regs->r[2], pt_regs->r[3],   \
	    pt_regs->r[4], pt_regs->r[5], pt_regs->r[6], pt_regs->r[7],   \
	    pt_regs->r[8], pt_regs->r[9], pt_regs->r[10], pt_regs->r[11], \
	    pt_regs->r[12], pt_regs->sp, pt_regs->lr, pt_regs->pc, pt_regs->cpsr)

#else
#endif /* __ASSEMBLER__ */
