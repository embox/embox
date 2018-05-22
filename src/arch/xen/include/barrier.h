/*
 * Definitions of memory barriers used by Xen.
 */

#if defined(__i386__)
#define mb()  __asm__ __volatile__ ("lock; addl $0,0(%%esp)" : : : "memory")
#define rmb() __asm__ __volatile__ ("lock; addl $0,0(%%esp)" : : : "memory")
#define wmb() __asm__ __volatile__ ("" : : : "memory")
#elif defined(__x86_64__)
#define mb()  __asm__ __volatile__ ("mfence" : : : "memory")
#define rmb() __asm__ __volatile__ ("lfence" : : : "memory")
#define wmb() __asm__ __volatile__ ("" : : : "memory")
#elif defined(__ia64__)
#define mb()   __asm__ __volatile__ ("mf" ::: "memory")
#define rmb()  __asm__ __volatile__ ("mf" ::: "memory")
#define wmb()  __asm__ __volatile__ ("mf" ::: "memory")
#elif defined(__powerpc__)
/* XXX loosen these up later */
#define mb()   __asm__ __volatile__ ("sync" : : : "memory")
#define rmb()  __asm__ __volatile__ ("sync" : : : "memory") /* lwsync? */
#define wmb()  __asm__ __volatile__ ("sync" : : : "memory") /* eieio? */
#else
#error "Define barriers"
#endif
