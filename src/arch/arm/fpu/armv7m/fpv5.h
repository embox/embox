/**
 * @file
 *
 * @date 13.06.2018
 * @author Alexander Kalmuk
 */

#ifndef SRC_ARCH_ARM_FPU_VFP5_S_H_
#define SRC_ARCH_ARM_FPU_VFP5_S_H_

#include <armv7m/fpu_regs.h>

#define ARM_FPU_VFP 1

#define FPU_DATA_LEN 32

/* Common registers for all ARMv7-M */
#define FPU_MVFR0 0xE000EF40
#define FPU_MVFR1 0xE000EF44

#ifndef __ASSEMBLER__
#include <string.h>
#include <stdint.h>

struct pt_regs_fpu {
	union  {
		float  s[32];
		double d[16];
	} vfp_regs;
};

static inline void arm_fpu_context_init(void *opaque) {
	memset(opaque, 0, sizeof(uint32_t) * FPU_DATA_LEN);
}

// extern int try_vfp_instructions(void /*struct pt_regs_fpu *vfp */);

#else

/* TODO write in the more efficient way with UsageFault.
 * ow these function do not store CPACR and just check
 * whether FPU is on or off.  */

#define ARM_FPU_CONTEXT_SAVE_INC(tmp, stack) \
	ldr       tmp, =FPU_CPACR; \
	ldr       tmp, [tmp]; \
	tst       tmp, #0xF00000; \
	beq       1f; \
	vstmia    stack!, {s0-s31}; \
1:

#define ARM_FPU_CONTEXT_LOAD_INC(tmp, stack) \
	ldr       tmp, =FPU_CPACR; \
	ldr       tmp, [tmp]; \
	tst       tmp, #0xF00000; \
	beq       1f; \
	vldmia    stack!, {s0-s31}; \
1:

#endif /* __ASSEMBLER__ */

#endif /* SRC_ARCH_ARM_FPU_VFP5_S_H_ */
