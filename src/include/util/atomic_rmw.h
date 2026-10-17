/**
 * @file
 * @brief Atomic RMW, with a fallback for targets that lack lock-free RMW.
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

#define atomic_rmw_add_fetch(ptr, val, order) __atomic_add_fetch(ptr, val, order)
#define atomic_rmw_sub_fetch(ptr, val, order) __atomic_sub_fetch(ptr, val, order)
#define atomic_rmw_or_fetch(ptr, val, order)  __atomic_or_fetch(ptr, val, order)
#define atomic_rmw_and_fetch(ptr, val, order) __atomic_and_fetch(ptr, val, order)
#define atomic_rmw_exchange(ptr, val, order)  __atomic_exchange_n(ptr, val, order)
#define atomic_rmw_load(ptr, order)           __atomic_load_n(ptr, order)
#define atomic_rmw_store(ptr, val, order)     __atomic_store_n(ptr, val, order)

/** Acquire on success, relaxed on failure. */
#define atomic_rmw_try_lock(ptr, unlocked, locked)                                \
	({                                                                        \
		__typeof__(*(ptr)) __expected = (unlocked);                           \
		__atomic_compare_exchange_n(ptr, &__expected, locked, 0,              \
		    __ATOMIC_ACQUIRE, __ATOMIC_RELAXED);                              \
	})

#else /* !ATOMIC_RMW_LOCK_FREE */

/* No libatomic in Embox: plain RMW between compiler barriers, correct on a
 * single core only. */
#define atomic_rmw_add_fetch(ptr, val, order) \
	({ __barrier(); *(ptr) += (val); __barrier(); *(ptr); })
#define atomic_rmw_sub_fetch(ptr, val, order) \
	({ __barrier(); *(ptr) -= (val); __barrier(); *(ptr); })
#define atomic_rmw_or_fetch(ptr, val, order) \
	({ __barrier(); *(ptr) |= (val); __barrier(); *(ptr); })
#define atomic_rmw_and_fetch(ptr, val, order) \
	({ __barrier(); *(ptr) &= (val); __barrier(); *(ptr); })
#define atomic_rmw_exchange(ptr, val, order)                                      \
	({                                                                        \
		__typeof__(*(ptr)) __old;                                             \
		__barrier();                                                          \
		__old = *(ptr);                                                       \
		*(ptr) = (val);                                                       \
		__barrier();                                                          \
		__old;                                                                \
	})

/* One barrier, on the side acquire (load) or release (store) needs. */
#define atomic_rmw_load(ptr, order)                                               \
	({                                                                        \
		__typeof__(*(ptr)) __v = *(volatile __typeof__(*(ptr)) *)(ptr);        \
		__barrier();                                                          \
		__v;                                                                  \
	})
#define atomic_rmw_store(ptr, val, order)                                         \
	do {                                                                      \
		__barrier();                                                          \
		*(volatile __typeof__(*(ptr)) *)(ptr) = (val);                        \
	} while (0)

#define atomic_rmw_try_lock(ptr, unlocked, locked)                                \
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
