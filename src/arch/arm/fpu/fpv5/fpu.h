/**
 * @file
 *
 * @date 13.06.2018
 * @author Alexander Kalmuk
 */

#ifndef ARM_FPU_H_
#define ARM_FPU_H_

#include <armv7m/fpu_regs.h>

#define ARM_FPU_VFP

#define FPU_DATA_LEN 32

/* Common registers for all ARMv7-M */
#define FPU_MVFR0 0xE000EF40
#define FPU_MVFR1 0xE000EF44

#ifndef __ASSEMBLER__
#include <string.h>
#include <stdint.h>

typedef struct fpu_context {
	union {
		float s[32];
		double d[16];
	} vfp_regs;
} fpu_context_t;

static inline void arm_fpu_context_init(void *opaque) {
	memset(opaque, 0, sizeof(uint32_t) * FPU_DATA_LEN);
}

// extern int try_vfp_instructions(void /*struct fpu_context *vfp */);
#else
#include <arm/fpu_macro.s>
#endif /* __ASSEMBLER__ */

#endif /* ARM_FPU_H_ */
