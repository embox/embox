/**
 * @file vfp.h
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 28.03.2018
 */

#ifndef ARM_FPU_H_
#define ARM_FPU_H_

#define ARM_FPU_VFP

/* One word for coprocessor state and 64 words for VFP registers */
#define FPU_DATA_LEN (64 + 1)

#define VFP_FPEXC_EN (1 << 30)

#ifndef __ASSEMBLER__
#include <stdint.h>
#include <string.h>

typedef struct fpu_context {
	uint32_t fpexc; /* cpacr actually */
	union {
		float s[32];
		double d[16];
	} __attribute__((packed)) vfp_regs;
} __attribute__((packed)) fpu_context_t;

static inline void arm_fpu_context_init(void *opaque) {
	memset(opaque, 0, sizeof(uint32_t) * FPU_DATA_LEN);
}

extern int try_vfp_instructions(fpu_context_t *vfp);

#else /* __ASSEMBLER__ */
#include <arm/fpu_macro.s>
#endif /* __ASSEMBLER__ */

#endif /* ARM_FPU_H_ */
