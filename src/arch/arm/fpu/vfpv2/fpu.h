/**
 * @file
 *
 * @date May 24, 2018
 * @author Anton Bondarev
 */

#ifndef ARM_FPU_H_
#define ARM_FPU_H_

#define ARM_FPU_VFP

/* One word for coprocessor state and 32 words for VPF registers */
#define FPU_DATA_LEN (32 + 1)

#define VFP_FPEXC_EN (1 << 30)

#ifndef __ASSEMBLER__
#include <stdint.h>
#include <string.h>

typedef struct fpu_context {
	uint32_t fpexc;
	float vfp_regs[32];
	// double vfp_regs[16];
} fpu_context_t;

static inline void arm_fpu_context_init(void *opaque) {
	memset(opaque, 0, sizeof(uint32_t) * FPU_DATA_LEN);
}

extern int try_vfp_instructions(fpu_context_t *vfp);
#else
#include <arm/fpu_macro.s>
#endif /* __ASSEMBLER__ */

#endif /* ARM_FPU_H_ */
