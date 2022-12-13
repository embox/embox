/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 06.11.22
 */

#include <kernel/printk.h>

#include "exceptions.h"

void _NORETURN arm_fiq_handler(excpt_context_t *ctx) {
	printk("\nUnresolvable fiq exception!\n");
	PRINT_PTREGS(&ctx->ptregs);
	while (1)
		;
}
