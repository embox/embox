/**
 * @file context.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 19.07.2019
 */
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <asm/hal/reg.h>
#include <asm/modes.h>
#include <hal/context.h>

void context_init(struct context *ctx, unsigned int flags,
    void (*routine_fn)(void), void *sp) {
	memset(ctx, 0, sizeof(*ctx));

	ctx->lr = (uint64_t)routine_fn;
	ctx->sp = (uint64_t)sp;

	if (!(flags & CONTEXT_IRQDISABLE)) {
		ctx->daif |= DAIF_I_BIT | DAIF_F_BIT;
	}
}
