#include <mem/vmem.h>
#include <kernel/thread.h>

extern void stack_protect(struct thread *t, size_t size);
extern void stack_protect_release(struct thread *t);

extern void stack_protect_enable();
extern void stack_protect_disable();

extern int stack_protect_enabled();
