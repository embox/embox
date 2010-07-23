/**
 * @file
 * @brief TODO
 *
 * @date
 * @author
 */
#include <types.h>
#include <hal/context.h>


void context_init(struct context *ctx, bool privileged) {
	int i;
	for (i = 0; i < 32; i++) {
		ctx->buf[0].regs[i] = 0;
	}
}

void context_set_stack(struct context *ctx, void *sp) {
	ctx->buf[0].regs[1] = (reg_t)sp;
}

void context_set_entry(struct context *ctx, void (*pc)(int), int arg) {
	ctx->buf[0].pc = (reg_t)pc;
}

void context_switch(struct context *prev, struct context *next) {
	switch (setjmp(prev->buf)) {
	case 0:
		TRACE("IN %u %u\n", prev, next);
		longjmp(next->buf, 1);
		break;
	case 1:
		TRACE("OUT %u %u\n", prev, next);
		return;
	default:
		TRACE("FUCK %u %u\n", prev, next);
		return;
	}
}
