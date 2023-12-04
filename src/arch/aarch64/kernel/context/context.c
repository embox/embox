/**
 * @file context.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 19.07.2019
 */
#include <stdint.h>
#include <string.h>

#include <hal/context.h>
#include <hal/reg.h>

void context_init(struct context *ctx, unsigned int flags,
    void (*routine_fn)(void), void *sp) {
	memset(ctx, 0, sizeof(*ctx));

	ctx->lr = (uint64_t)routine_fn;
	ctx->sp = (uint64_t)sp;

	if (!(flags & CONTEXT_IRQDISABLE)) {
		ctx->daif |= DAIF_I | DAIF_F;
	}
}
