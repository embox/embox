/**
 * @file stub.h
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 28.03.2018
 */

#ifndef ARM_FPU_STUB_H_
#define ARM_FPU_STUB_H_

/* Do nothing on context switch */
#define ARM_FPU_CONTEXT_SAVE_INC(tmp, stack)
#define ARM_FPU_CONTEXT_LOAD_INC(tmp, stack)
#define ARM_FPU_CONTEXT_SAVE_DEC(tmp, stack)
#define ARM_FPU_CONTEXT_LOAD_DEC(tmp, stack)

#define FPU_DATA_LEN 0

#ifndef __ASSEMBLER__
static inline void arm_fpu_context_init(void *opaque) {
	/* Do nothing */
}

static inline int try_vfp_instructions(void) {
	return 0;
}

#endif /* __ASSEMBLER__ */

#endif /* ARM_FPU_STUB_H_ */
