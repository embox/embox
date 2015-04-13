/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    25.10.2012
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <hal/context.h>

#include <asm/modes.h>

void context_init(struct context *ctx, unsigned int flags,
		void (*routine_fn)(void), void *sp) {
	ctx->lr = (uint32_t) routine_fn;
	ctx->sp = (uint32_t) sp;
	ctx->cpsr = ARM_MODE_SYS;

	if (flags & CONTEXT_IRQDISABLE) {
		ctx->cpsr |= I_BIT | F_BIT;
	}
}

