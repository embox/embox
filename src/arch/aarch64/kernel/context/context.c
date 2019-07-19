/**
 * @file context.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 19.07.2019
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>
#include <hal/context.h>

void context_init(struct context *ctx, unsigned int flags,
		void (*routine_fn)(void), void *sp) {
	memset(ctx, 0, sizeof(*ctx));
}
