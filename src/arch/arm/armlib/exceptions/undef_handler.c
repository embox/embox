/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 06.11.22
 */

#include "exception_priv.h"

#include <arm/fpu.h>
#include <kernel/printk.h>

void arm_undef_handler(struct pt_regs *pt_regs) {
	if (try_vfp_instructions()) {
		return;
	}

	printk("\nUnresolvable undefined exception!\n");
	PRINT_PTREGS(pt_regs);

#if KEEP_GOING
	while (1)
		;
#endif
}
