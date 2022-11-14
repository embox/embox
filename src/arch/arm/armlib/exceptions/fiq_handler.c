/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 06.11.22
 */

#include "exception_priv.h"

#include <kernel/printk.h>

void arm_fiq_handler(struct pt_regs *pt_regs) {
	printk("\nUnresolvable fiq exception!\n");
	PRINT_PTREGS(pt_regs);

#if KEEP_GOING
	while (1)
		;
#endif
}
