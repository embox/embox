/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 06.11.22
 */

#include <arm/exception.h>
#include <arm/fpu.h>
#include <kernel/printk.h>

void arm_undef_handler(excpt_context_t *ctx) {
	if (!try_vfp_instructions(&ctx->fpu_context)) {
		printk("\nUnresolvable undefined exception!\n");
		arm_show_excpt_context(ctx);
		while (1) {};
	}
}
