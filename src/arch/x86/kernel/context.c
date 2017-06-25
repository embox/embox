/**
 * @file
 *
 * @date 1.12.10
 * @author Nikolay Korotky
 */

#include <hal/context.h>
#include <stdint.h>
#include <stdbool.h>

#include <string.h>

void context_init(struct context *ctx, unsigned int flags,
		void (*routine_fn)(void), void *sp) {
	memset(ctx, 0, sizeof(*ctx));

	/* Set kernel mode stack pointer */
	ctx->esp = (uint32_t) sp - 0x40 - sizeof(uint32_t);
	// TODO seems that ebp must be set as well

	ctx->eip = (uint32_t) routine_fn;
}
