/**
 * @file vfp.h
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 28.03.2018
 */

#ifndef ARM_FPU_VFP_H_
#define ARM_FPU_VFP_H_

#define ARM_FPU_VFP 1

#define FPU_DATA_LEN 65 /* One word for coprocessor state
			   and 64 words for VPF registers */

#ifndef __ASSEMBLER__
#include <string.h>
#include <stdint.h>

struct pt_regs_fpu {
	uint32_t   fpexc;
	union  {
		float  s[64];
		double d[32];
	} vfp_regs;
};

static inline void arm_fpu_context_init(void *opaque) {
	memset(opaque, 0, sizeof(uint32_t) * FPU_DATA_LEN);
}

extern int try_vfp_instructions(void);

#else /* __ASSEMBLER__ */

/* Note: this functions do not change stack value */

#define ARM_FPU_CONTEXT_SAVE_INC(tmp, stack) \
	mrc       p15, 0, tmp, c1, c0, 2; \
	stmia     stack!, {tmp}; \
	tst       tmp, #0xF00000; \
	beq       1f;          \
	vstmia    stack!, {d0-d15}; \
	vstmia    stack!, {d16-d31}; \
1:

#define ARM_FPU_CONTEXT_SAVE_DEC(tmp, stack) \
	mrc       p15, 0, tmp, c1, c0, 2; \
	tst       tmp, #0xF00000; \
	beq       1f; \
	vstmdb    stack!, {d0-d15}; \
	vstmdb    stack!, {d16-d31}; \
1: \
	stmfd     stack!, {tmp};

#define ARM_FPU_CONTEXT_LOAD_INC(tmp, stack) \
	ldmia     stack!, {tmp}; \
	mcr       p15, 0, tmp, c1, c0, 2; \
	tst       tmp, #0xF00000; \
	beq       1f; \
	vldmia    stack!, {d0-d15}; \
	vldmia    stack!, {d16-d31}; \
1:

#define ARM_FPU_CONTEXT_LOAD_DEC(tmp, stack) \
	ldmfd     stack!, {tmp}; \
	mcr       p15, 0, tmp, c1, c0, 2; \
	tst       tmp, #0xF00000; \
	beq       1f; \
	vldmia   stack!, {d16-d31}; \
	vldmia   stack!, {d0-d15}; \
1:

#endif /* __ASSEMBLER__ */

#endif /* ARM_FPU_VFP_H_ */
