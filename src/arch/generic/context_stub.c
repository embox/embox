/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    16.01.2014
 */

#include <hal/context.h>
#include <kernel/panic.h>

void context_init(struct context *ctx, unsigned int flags,
    void (*routine_fn)(void), void *sp) {
}

void context_switch(struct context *prev, struct context *next) {
	panic("%s", __func__);
}
