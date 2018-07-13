/**
 * @file
 *
 * @date May 24, 2018
 * @author Anton Bondarev
 */

#ifndef SRC_ARCH_ARM_FPU_VFP9_S_H_
#define SRC_ARCH_ARM_FPU_VFP9_S_H_

#define ARM_FPU_VFP 1

#define FPU_DATA_LEN (32 + 1) /* One word for coprocessor state
			   and 32 words for VPF registers */

#ifndef __ASSEMBLER__
#include <string.h>
#include <stdint.h>

struct pt_regs_fpu {
	uint32_t   fpexc;
	union  {
		float  s[32];
		double d[16];
	} vfp_regs;
};

static inline void arm_fpu_context_init(void *opaque) {
	memset(opaque, 0, sizeof(uint32_t) * FPU_DATA_LEN);
}

extern int try_vfp_instructions(void /*struct pt_regs_fpu *vfp */);

#else

/* Note: this functions do not change stack value */

#define ARM_FPU_CONTEXT_SAVE_INC(tmp, stack) \
	vmrs      tmp, FPEXC ; \
	stmia     stack!, {tmp}; \
	ands      tmp, tmp, #1<<30; \
	beq       fpu_out_save_inc; \
	vstmia    stack!, {d0-d15}; \
fpu_out_save_inc:

#define ARM_FPU_CONTEXT_SAVE_DEC(tmp, stack) \
	vmrs      tmp, FPEXC ;             \
	sub       stack, stack, #(4 * 32); \
	tst       tmp, #1<<30;             \
	beq       fpu_skip_save_dec;       \
	vstmia    stack, {d0-d15};         \
fpu_skip_save_dec:                     \
	stmfd     stack!, {tmp};


#define ARM_FPU_CONTEXT_LOAD_INC(tmp, stack) \
	ldmia     stack!, {tmp}; \
	vmsr      FPEXC, tmp; \
	ands      tmp, tmp, #1<<30; \
	beq       fpu_out_load_inc; \
	vldmia    stack!, {d0-d15}; \
fpu_out_load_inc:

#define ARM_FPU_CONTEXT_LOAD_DEC(tmp, stack) \
	ldmfd     stack!, {tmp}; \
	/* vmsr      FPEXC, tmp; */ \
	ands      tmp, tmp, #1<<30; \
	beq       fpu_skip_load_dec; \
	vldmia    stack, {d0-d15}; \
fpu_skip_load_dec: \
	add       stack, stack, #4 * 32;

#endif /* __ASSEMBLER__ */

#endif /* SRC_ARCH_ARM_FPU_VFP9_S_H_ */
