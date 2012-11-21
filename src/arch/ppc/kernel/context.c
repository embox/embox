/**
 * @file
 *
 * @date 16.11.12
 * @author Ilia Vaprol
 */

#include <hal/context.h>

//#include <prom/prom_printf.h>
void context_init(struct context *ctx, bool privileged) {
//	prom_printf("\nCONTEXT_INIT ctx[%p]\n", ctx);
}

void context_set_stack(struct context *ctx, void *sp) {
//	prom_printf("\nCONTEXT_SET_STACK ctx[%p] sp[%p]\n", ctx, (char*)sp - 8);
	ctx->sp = (uint32_t)sp - 8;
}

void context_set_entry(struct context *ctx, void (*pc)(void)) {
//	prom_printf("\nCONTEXT_SET_ENTRY ctx[%p] pc[%p]\n", ctx, pc);
	ctx->lr = (uint32_t)pc;
}
