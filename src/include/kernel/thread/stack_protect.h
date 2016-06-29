#include <mem/vmem.h>
#include <kernel/thread.h>

void stack_protect(struct thread *t, size_t size);
void stack_protect_release(struct thread *t);

void stack_protect_enable();
void stack_protect_disable();

int stack_protect_enabled();
