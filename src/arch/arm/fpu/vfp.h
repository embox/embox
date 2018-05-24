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
	beq       fpu_out_save_inc; \
	vstmia stack!, {d0-d15}; \
	vstmia stack!, {d16-d31}; \
fpu_out_save_inc:

#define ARM_FPU_CONTEXT_SAVE_DEC(tmp, stack) \
	mrc       p15, 0, tmp, c1, c0, 2; \
	tst       tmp, #0xF00000; \
	beq       fpu_out_save_dec; \
	fstmfdd   stack!, {d0-d15}; \
	fstmfdd   stack!, {d16-d31}; \
fpu_out_save_dec: \
	stmfd     stack!, {tmp};

#define ARM_FPU_CONTEXT_LOAD_INC(tmp, stack) \
	ldmia     stack!, {tmp}; \
	mcr       p15, 0, tmp, c1, c0, 2; \
	tst       tmp, #0xF00000; \
	beq       fpu_out_load_inc; \
	vldmia    stack!, {d0-d15}; \
	vldmia    stack!, {d16-d31}; \
fpu_out_load_inc:

#define ARM_FPU_CONTEXT_LOAD_DEC(tmp, stack) \
	ldmfd     stack!, {tmp}; \
	mcr       p15, 0, tmp, c1, c0, 2; \
	tst       tmp, #0xF00000; \
	beq       fpu_out_load_dec; \
	fldmfdd   stack!, {d16-d31}; \
	fldmfdd   stack!, {d0-d15}; \
fpu_out_load_dec:

#endif /* __ASSEMBLER__ */

#endif /* ARM_FPU_VFP_H_ */
