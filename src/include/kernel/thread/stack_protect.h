#ifndef STACK_PROTECT_H_
#define STACK_PROTECT_H_

#include <stddef.h>

struct thread;

extern void stack_protect(struct thread *t, size_t size);
extern void stack_protect_release(struct thread *t);

extern void stack_protect_enable(void);
extern void stack_protect_disable(void);

extern int stack_protect_enabled(void);

#endif /* STACK_PROTECT_H_ */
