/**
 * @file
 *
 * @author Anton Bondarev
 *
 * @date 14.12.2022
 */
 
#ifndef E2K_ATOMIC_API_H_
#define E2K_ATOMIC_API_H_

#define NATIVE_HWBUG_AFTER_LD_ACQ() do { } while (0)

#define	VIRT_HWBUG_AFTER_LD_ACQ()   NATIVE_HWBUG_AFTER_LD_ACQ()

#define VIRT_HWBUG_AFTER_LD_ACQ_STRONG_MB     VIRT_HWBUG_AFTER_LD_ACQ
#define VIRT_HWBUG_AFTER_LD_ACQ_LOCK_MB       VIRT_HWBUG_AFTER_LD_ACQ
#define VIRT_HWBUG_AFTER_LD_ACQ_ACQUIRE_MB    VIRT_HWBUG_AFTER_LD_ACQ
#define VIRT_HWBUG_AFTER_LD_ACQ_RELEASE_MB()
#define VIRT_HWBUG_AFTER_LD_ACQ_RELAXED_MB()

#define virt_api_atomic_op(val, addr, size_letter, op, mem_model) \
({ \
	typeof(val) rval; \
	NATIVE_ATOMIC_OP(val, addr, rval, size_letter, op, mem_model); \
	VIRT_HWBUG_AFTER_LD_ACQ_##mem_model(); \
	rval; \
})

#define virt_api_atomic_fetch_op(val, addr, size_letter, op, mem_model) \
({ \
	typeof(val) rval, stored_val; \
	NATIVE_ATOMIC_FETCH_OP(val, addr, rval, stored_val, \
				size_letter, op, mem_model); \
	VIRT_HWBUG_AFTER_LD_ACQ_##mem_model(); \
	rval; \
})


/* Atomically add and return the old value */
#define __api_atomic32_add_oldval(val, addr) \
		virt_api_atomic_fetch_op(val, addr, w, "adds", STRONG_MB)

#define __api_atomic32_add_oldval_lock(val, addr) \
		virt_api_atomic_fetch_op(val, addr, w, "adds", LOCK_MB)

#endif /* E2K_ATOMIC_API_H_ */
