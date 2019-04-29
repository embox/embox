/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    23.10.2013
 */

#include <stdint.h>
#include <inttypes.h>

#include <arm/fpu.h>
#include <framework/mod/options.h>
#include <kernel/printk.h>
#include <util/log.h>

#define KEEP_GOING OPTION_GET(BOOLEAN,keep_going)
struct pt_regs_exception {
#ifdef ARM_FPU_VFP
	struct pt_regs_fpu vfp;
#endif

	uint32_t lr;
	uint32_t sp;
	uint32_t regs[13]; /* r0-12 */
	uint32_t prev_lr; /* previous mode lr */
	uint32_t spsr;
};

void arm_undefined_exception(struct pt_regs_exception *pt_regs) {
#ifdef ARM_FPU_VFP
	if (try_vfp_instructions(/* &pt_regs ->vfp */)) {
		return;
	}
#endif

	printk("Undefined exception: ret pc = 0x%08" PRIx32 " spsr = 0x%08" PRIx32 "\n",
			pt_regs->prev_lr, pt_regs->spsr);

	printk("regs:\n"
			"%08" PRIx32 " %08" PRIx32 " %08" PRIx32 " %08" PRIx32 "\n"
			"%08" PRIx32 " %08" PRIx32 " %08" PRIx32 " %08" PRIx32 "\n"
			"%08" PRIx32 " %08" PRIx32 " %08" PRIx32 " %08" PRIx32 "\n"
			"%08" PRIx32 " sp(r13) %08" PRIx32 " lr (r14) %08" PRIx32 "\n",
			pt_regs->regs[0], pt_regs->regs[1], pt_regs->regs[2],
			pt_regs->regs[3], pt_regs->regs[4], pt_regs->regs[5],
			pt_regs->regs[6], pt_regs->regs[7],	pt_regs->regs[8],
			pt_regs->regs[9], pt_regs->regs[10], pt_regs->regs[11],
			pt_regs->regs[12], pt_regs->prev_lr, pt_regs->spsr);
#if !KEEP_GOING
	while(1);
#endif
}
