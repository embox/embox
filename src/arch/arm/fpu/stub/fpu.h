/**
 * @file stub.h
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 28.03.2018
 */

#ifndef ARM_FPU_H_
#define ARM_FPU_H_

#define FPU_DATA_LEN 0

#ifndef __ASSEMBLER__

typedef struct fpu_context {

} fpu_context_t;

static inline void arm_fpu_context_init(void *opaque) {
	/* Do nothing */
}

static inline int try_vfp_instructions(fpu_context_t *vfp) {
	/* Do nothing */
	return 0;
}

#else
#include <arm/fpu_macro.s>
#endif /* __ASSEMBLER__ */

#endif /* ARM_FPU_H_ */
