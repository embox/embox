/**
 * @file
 * @brief
 *
 * @date 08.02.12
 * @author Anton Bulychev
 * @author Ilia Vaprol
 * @author Eldar Abusalimov
 */

#ifndef KERNEL_SPINLOCK_H_
#define KERNEL_SPINLOCK_H_

#include <sys/types.h>

#include <framework/mod/options.h>
#include <hal/ipl.h>
#include <util/lang.h>
#include <util/macro.h>

#include <config/embox/kernel/spinlock.h> /* for SPIN_CONTENTION_LIMIT */

#define SPIN_CONTENTION_LIMIT \
	OPTION_MODULE_GET(embox__kernel__spinlock, NUMBER, spin_contention_limit)

#if SPIN_CONTENTION_LIMIT
#define SPIN_INIT(state) \
	{ -1u, state, SPIN_CONTENTION_LIMIT }
#else
#define SPIN_INIT(state) \
	{ -1u, state }
#endif

#define __SPIN_UNLOCKED 0
#define __SPIN_LOCKED   1

#define SPIN_STATIC_UNLOCKED SPIN_INIT(__SPIN_UNLOCKED)
#define SPIN_STATIC_LOCKED   SPIN_INIT(__SPIN_LOCKED)

#define SPIN_UNLOCKED (spinlock_t) SPIN_STATIC_UNLOCKED
#define SPIN_LOCKED   (spinlock_t) SPIN_STATIC_LOCKED

typedef struct {
	unsigned int owner;
	unsigned long l;
#if SPIN_CONTENTION_LIMIT
	unsigned long contention_count;
#endif
} spinlock_t;

extern int __spin_trylock(spinlock_t *lock);
extern void __spin_lock(spinlock_t *lock);
extern void __spin_unlock(spinlock_t *lock);
extern void __spin_preempt_disable(void);
extern void __spin_preempt_enable(void);

extern void spin_init(spinlock_t *lock, unsigned int state);
extern int spin_trylock(spinlock_t *lock);
extern void spin_lock(spinlock_t *lock);
extern void spin_unlock(spinlock_t *lock);
extern ipl_t spin_lock_ipl(spinlock_t *lock);
extern void spin_unlock_ipl(spinlock_t *lock, ipl_t ipl);
extern void spin_lock_ipl_disable(spinlock_t *lock);
extern void spin_unlock_ipl_enable(spinlock_t *lock);

/**
 * Spin until either @a lock is acquired or @a cond becomes @c false.
 * @return @a cond value. In case of a non-zero value the spin is locked.
 * Unlocking it in the latter case is up to the client.
 */
#define SPIN_LOCK_COND(lock, cond)       \
	({                                   \
		spinlock_t *__lock = (lock);     \
		typeof(cond) __cond;             \
                                         \
		do {                             \
			__cond = (cond);             \
			if (!__cond)                 \
				break;                   \
		} while (!spin_trylock(__lock)); \
                                         \
		if (__cond) {                    \
			/* just been locked */       \
			__cond = (cond);             \
			if (!__cond)                 \
				spin_unlock(__lock);     \
		}                                \
                                         \
		__cond;                          \
	})

/**
 * 'if' statement based on #SPIN_LOCK_COND(). True branch gets executed with
 * a non-zero value of @a cond and spin locked. False branch gets no locks.
 * No explicit unlocking is required.
 * Making 'return' stmt inside the block will leave the spin locked.
 * Also 'break'/'continue' will not work as expected.
 */
#define spin_protected_if(lock, cond)                                         \
	__spin_protected_if(lock, cond, MACRO_GUARD(__done), MACRO_GUARD(__lock), \
	    MACRO_GUARD(__cond))

#define __spin_protected_if(lock, cond, __done, __lock, __cond)                      \
	for (int __done = 0; !__done;)                                                   \
		for (spinlock_t *__lock = (lock); !__done;)                                  \
			for (int __cond = !!SPIN_LOCK_COND(__lock, cond); !__done; ({            \
				     if (__cond)                                                     \
					     spin_unlock(__lock);                                        \
			     }))                                                                 \
				while (!__done && (++__done)) /* break/continue control this loop */ \
					if (__cond)

#define SPIN_PROTECTED_DO(lock, expr)                  \
	__lang_surround(expr, spinlock_t *__lock = (lock); \
	                spin_lock(__lock), spin_unlock(__lock))

#define SPIN_IPL_PROTECTED_DO(lock, expr)                \
	__lang_surround(expr, spinlock_t *__lock = (lock);   \
	                ipl_t __ipl = spin_lock_ipl(__lock), \
	                spin_unlock_ipl(__lock, __ipl))

#endif /* KERNEL_SPINLOCK_H_ */
