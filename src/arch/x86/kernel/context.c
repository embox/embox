/**
 * @file
 *
 * @date 1.12.10
 * @author Nikolay Korotky
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <asm/cr_regs.h>
#include <asm/flags.h>
#include <hal/context.h>
#include <util/binalign.h>

void context_init(struct context *ctx, unsigned int flags,
    void (*routine_fn)(void), void *sp) {
	memset(ctx, 0, sizeof(*ctx));

	/* Set kernel mode stack pointer */
	ctx->esp = (uint32_t)sp - 0x40 - sizeof(uint32_t);
	// TODO seems that ebp must be set as well

	ctx->eip = (uint32_t)routine_fn;

#ifdef __SSE__
	/* disable SSE extension */
	ctx->cr4 = get_cr4();
	ctx->cr4 &= ~X86_CR4_OSFXSR;
	set_cr4(ctx->cr4);
#endif
}

#ifdef __SSE__
static inline void save_sse_regs(struct context *ctx) {
	ctx->cr4 = get_cr4();
	if (ctx->cr4 & X86_CR4_OSFXSR) {
		/* fxsave_region must be 16-byte aligned */
		if (binalign_check_bound((uintptr_t)&ctx->fxsave_region[0], 16)) {
			__asm__ __volatile__("fxsave %0" ::"m"(ctx->fxsave_region[0]));
		}
		else {
			__asm__ __volatile__("fxsave %0" ::"m"(ctx->fxsave_region[1]));
		}
	}
}

static inline void load_sse_regs(struct context *ctx) {
	set_cr4(ctx->cr4);
	if (ctx->cr4 & X86_CR4_OSFXSR) {
		/* fxsave_region must be 16-byte aligned */
		if (binalign_check_bound((uintptr_t)&ctx->fxsave_region[0], 16)) {
			__asm__ __volatile__("fxrstor %0" ::"m"(ctx->fxsave_region[0]));
		}
		else {
			__asm__ __volatile__("fxrstor %0" ::"m"(ctx->fxsave_region[1]));
		}
	}
}
#else
#define save_sse_regs(ctx)
#define load_sse_regs(ctx)
#endif

extern void __context_switch(struct context *prev, struct context *next);

void context_switch(struct context *prev, struct context *next) {
	save_sse_regs(prev);
	__context_switch(prev, next);
	load_sse_regs(next);
}
