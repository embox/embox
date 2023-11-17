/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 06.11.22
 */

#include <arm/exception.h>
#include <kernel/printk.h>

void _NORETURN arm_fiq_handler(excpt_context_t *ctx) {
	printk("\nUnresolvable fiq exception!\n");
	arm_show_excpt_context(ctx);
	while (1) {};
}
