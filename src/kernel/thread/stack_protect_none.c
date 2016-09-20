#include <kernel/thread/stack_protect.h>

#include <mem/vmem/vmem_alloc.h>

static int enabled = 0;

void stack_protect(struct thread *t, size_t size) {}

void stack_protect_release(struct thread *t) {}

void stack_protect_enable() {
    enabled = 1;
}

void stack_protect_disable() {
    enabled = 0;
}

int stack_protect_enabled()
{
    return enabled;
}
