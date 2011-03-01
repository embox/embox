/**
 * @file
 * @brief TODO
 *
 * @date 13.04.10
 * @author Eldar Abusalimov
 */
#include <types.h>
#include <hal/context.h>

void context_init(struct context *ctx, bool privileged) {
}

void context_set_stack(struct context *ctx, void *sp) {
	ctx->buf[0].sp = sp;
}

void context_set_entry(struct context *ctx, void (*pc)(int), int arg) {
	ctx->buf[0].lp = pc;
}

void context_switch(struct context *prev, struct context *next) {
	switch (setjmp(prev->buf)) {
	case 0:
		longjmp(next->buf, 1);
		break;
	case 1:
		return;
	default:
		return;
	}
}
