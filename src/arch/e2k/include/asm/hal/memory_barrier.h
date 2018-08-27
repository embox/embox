#ifndef _HAL_MEMORY_BARRIER_H_
#define _HAL_MEMORY_BARRIER_H_

#include <e2k_api.h>

#define memory_barrier() mb()

#define mb()	E2K_WAIT(_st_c | _ld_c)
#define wmb()	E2K_WAIT_ST_C_SAS()
#define rmb()	E2K_WAIT(_ld_c)

#if MAX_CORES > 1
#define smp_mb()	mb()
#define smp_rmb()	rmb()
#define smp_wmb()	wmb()
#else
#define smp_mb()	barrier()
#define smp_rmb()	barrier()
#define smp_wmb()	barrier()
#endif

#ifdef CONFIG_ISET_V5_SUPPORTED
# ifdef CONFIG_ATOMICS_5CH_ACQUIRE
#  define smp_mb__after_atomic_dec()	barrier()
#  define smp_mb__after_atomic_inc()	barrier()
# else
#  define smp_mb__after_atomic_dec()	E2K_WAIT(_st_c)
#  define smp_mb__after_atomic_inc()	E2K_WAIT(_st_c)
# endif
# define smp_mb__before_atomic_dec()	E2K_WAIT_ST_C_SAS_LD_C_SAL()
# define smp_mb__before_atomic_inc()	E2K_WAIT_ST_C_SAS_LD_C_SAL()
# define smp_mb__before_spinlock()	E2K_WAIT_ST_C_SAS_LD_C_SAL()
#else
/* Atomic operations are serializing since e2s */
# if MAX_CORES > 1 && \
		(!defined CONFIG_E2K_MACHINE || \
		 defined CONFIG_E2K_E3S || \
		 defined CONFIG_E2K_ES2_DSP || \
		 defined CONFIG_E2K_ES2_RU)
#  define smp_mb__after_atomic_dec()	E2K_WAIT(_st_c)
#  define smp_mb__after_atomic_inc()	E2K_WAIT(_st_c)
# else
#  define smp_mb__after_atomic_dec()	barrier()
#  define smp_mb__after_atomic_inc()	barrier()
# endif
# define smp_mb__before_atomic_dec()	barrier()
# define smp_mb__before_atomic_inc()	barrier()
# define smp_mb__before_spinlock()	barrier()
#endif


#define smp_store_release(p, v) \
do { \
	compiletime_assert(sizeof(*p) == 1 || sizeof(*p) == 2 || \
			sizeof(*p) == 4 || sizeof(*p) == 8, \
			"Need native word sized stores/loads for atomicity."); \
	E2K_WAIT_ST_C_SAS_LD_C_SAL(); \
	ACCESS_ONCE(*p) = (v); \
} while (0)

#define smp_load_acquire(p) \
({ \
	typeof(*(p)) ___p1 = ACCESS_ONCE(*(p)); \
	compiletime_assert(sizeof(*p) == 1 || sizeof(*p) == 2 || \
			sizeof(*p) == 4 || sizeof(*p) == 8, \
			"Need native word sized stores/loads for atomicity."); \
	E2K_RF_WAIT_LOAD(___p1); \
	___p1; \
})

#endif /* _HAL_MEMORY_BARRIER_H_ */
