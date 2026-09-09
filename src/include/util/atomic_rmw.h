/**
 * @file
 * @brief RMW atomics with fallback for targets lacking lock-free support.
 *
 * __atomic_* RMW operations become libatomic calls on targets without
 * hardware support (e.g., 80386). Embox doesn't link libatomic, so we
 * provide fallbacks: plain RMW with barriers for non-lock-free targets.
 * Correct on single-core; multi-core targets without lock-free RMW are
 * not supported by this kernel.
 */

#ifndef UTIL_ATOMIC_RMW_H_
#define UTIL_ATOMIC_RMW_H_

#include <linux/compiler.h>

#if defined(__GCC_ATOMIC_INT_LOCK_FREE) && defined(__GCC_ATOMIC_LONG_LOCK_FREE) \
    && (__GCC_ATOMIC_INT_LOCK_FREE == 2) && (__GCC_ATOMIC_LONG_LOCK_FREE == 2)
#define ATOMIC_RMW_LOCK_FREE 1
#else
#define ATOMIC_RMW_LOCK_FREE 0
#endif

#if ATOMIC_RMW_LOCK_FREE

#define atomic_add_fetch(ptr, val, order) __atomic_add_fetch(ptr, val, order)
#define atomic_sub_fetch(ptr, val, order) __atomic_sub_fetch(ptr, val, order)
#define atomic_or_fetch(ptr, val, order)  __atomic_or_fetch(ptr, val, order)
#define atomic_and_fetch(ptr, val, order) __atomic_and_fetch(ptr, val, order)
#define atomic_exchange(ptr, val, order)  __atomic_exchange_n(ptr, val, order)
#define atomic_load(ptr, order)           __atomic_load_n(ptr, order)
#define atomic_store(ptr, val, order)     __atomic_store_n(ptr, val, order)

/** Acquire on success, relaxed on failure. */
#define atomic_try_lock(ptr, unlocked, locked)                                \
	({                                                                        \
		__typeof__(*(ptr)) __expected = (unlocked);                           \
		__atomic_compare_exchange_n(ptr, &__expected, locked, 0,              \
		    __ATOMIC_ACQUIRE, __ATOMIC_RELAXED);                              \
	})

#else /* !ATOMIC_RMW_LOCK_FREE */

/* The barrier is what is left of the memory order: the compiler is stopped
 * from moving the access, and a machine that cannot run two of these at once
 * has nothing else to reorder against. */
#define atomic_add_fetch(ptr, val, order) \
	({ __barrier(); *(ptr) += (val); __barrier(); *(ptr); })
#define atomic_sub_fetch(ptr, val, order) \
	({ __barrier(); *(ptr) -= (val); __barrier(); *(ptr); })
#define atomic_or_fetch(ptr, val, order) \
	({ __barrier(); *(ptr) |= (val); __barrier(); *(ptr); })
#define atomic_and_fetch(ptr, val, order) \
	({ __barrier(); *(ptr) &= (val); __barrier(); *(ptr); })
#define atomic_exchange(ptr, val, order)                                      \
	({                                                                        \
		__typeof__(*(ptr)) __old;                                             \
		__barrier();                                                          \
		__old = *(ptr);                                                       \
		*(ptr) = (val);                                                       \
		__barrier();                                                          \
		__old;                                                                \
	})

/* Acquire on a load is "nothing after this may be hoisted above it", release
 * on a store is "nothing before it may sink below": one barrier each, on the
 * side the order names. */
#define atomic_load(ptr, order)                                               \
	({                                                                        \
		__typeof__(*(ptr)) __v = *(volatile __typeof__(*(ptr)) *)(ptr);        \
		__barrier();                                                          \
		__v;                                                                  \
	})
#define atomic_store(ptr, val, order)                                         \
	do {                                                                      \
		__barrier();                                                          \
		*(volatile __typeof__(*(ptr)) *)(ptr) = (val);                        \
	} while (0)

#define atomic_try_lock(ptr, unlocked, locked)                                \
	({                                                                        \
		int __got;                                                            \
		__barrier();                                                          \
		__got = (*(ptr) == (unlocked));                                       \
		if (__got) {                                                          \
			*(ptr) = (locked);                                                \
		}                                                                     \
		__barrier();                                                          \
		__got;                                                                \
	})

#endif /* ATOMIC_RMW_LOCK_FREE */

#endif /* UTIL_ATOMIC_RMW_H_ */
