/**
 * @file
 * @brief
 *
 * @date 12.10.2012
 * @author Anton Bulychev
 */

#ifndef ARCH_X86_USERMODE_H_
#define ARCH_x86_USERMODE_H_

#include <types.h>

static inline void *call_in_usermode_if(int cond, void *(*func)(void *), void *arg) {
	extern void *usermode_enter(void *(*func)(void *), void *arg);

	if (cond) {
		/* There is no return from usermode. */
		return usermode_enter(func, arg);
	} else {
		return func(arg);
	}
}

#endif /* ARCH_x86_USERMODE_H_ */
