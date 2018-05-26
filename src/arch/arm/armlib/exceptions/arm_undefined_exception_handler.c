/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    23.10.2013
 */
#include <util/log.h>

#include <kernel/thread.h>

#include <arm/fpu.h>

void arm_undefined_exception(unsigned int *regs) {
	if (try_vfp_instructions()) {
		return;
	}

	log_debug("regs:\n"
			"%08x %08x %08x %08x\n"
			"%08x %08x %08x %08x\n"
			"%08x %08x %08x %08x\n"
			"%08x %08x %08x %08x\n",
			"%08x %08x %08x %08x\n",
			regs[0], regs[1], regs[2], regs[3],
			regs[4], regs[5], regs[6], regs[7],
			regs[8], regs[9], regs[10], regs[11],
			regs[12], regs[13], regs[14], regs[15],
			regs[16], regs[17], regs[18], regs[19]);
}
