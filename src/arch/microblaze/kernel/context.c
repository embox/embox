/**
 * @file
 * @brief
 *
 * @date 29.07.2011
 * @author Anton Bondarev
 */

#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

#include <hal/context.h>
#include <asm/msr.h>

void context_init(struct context *ctx, unsigned int flags,
		void (*routine_fn)(void), void *sp) {
	ctx->msr = msr_get_value();
	ctx->r1 = (uint32_t) sp;
	ctx->r15 = (uint32_t) routine_fn - 8;
}
