/**
 * @file abort_exception_handler.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 28.06.2018
 */
#include <stdint.h>
#include <inttypes.h>

#include <kernel/printk.h>

struct pt_regs_exception {
	uint32_t regs[14]; /* r0-12, r14 */
};

void arm_data_abort_exception(struct pt_regs_exception *pt_regs) {
	printk("Data abort exception!\nregs:\n"
			"%08" PRIx32 " %08" PRIx32 " %08" PRIx32 " %08" PRIx32 "\n"
			"%08" PRIx32 " %08" PRIx32 " %08" PRIx32 " %08" PRIx32 "\n"
			"%08" PRIx32 " %08" PRIx32 " %08" PRIx32 " %08" PRIx32 "\n"
			"%08" PRIx32 " %08" PRIx32 "\n",
			pt_regs->regs[0], pt_regs->regs[1], pt_regs->regs[2],
			pt_regs->regs[3], pt_regs->regs[4], pt_regs->regs[5],
			pt_regs->regs[6], pt_regs->regs[7], pt_regs->regs[8],
			pt_regs->regs[9], pt_regs->regs[10], pt_regs->regs[11],
			pt_regs->regs[12], pt_regs->regs[13]);

	while(1);
}
