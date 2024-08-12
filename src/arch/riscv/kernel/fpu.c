/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 21.08.23
 */

#include <asm/regs.h>
#include <embox/unit.h>

EMBOX_UNIT_INIT(riscv_enable_fpu);

static int riscv_enable_fpu(void) {
	set_csr_bit(mstatus, MSTATUS_FS);
	return 0;
}
