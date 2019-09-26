#include <kernel/panic.h>
#include <hal/context.h>

void context_init(struct context *ctx, unsigned int flags,
		void (*routine_fn)(void), void *sp) {

}

void context_switch(struct context *prev, struct context *next) {
	panic("%s", __func__);
}
