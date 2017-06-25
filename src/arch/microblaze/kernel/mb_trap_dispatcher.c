/**
 * @file
 *
 * @date 17.03.10
 * @author Anton Bondarev
 */

#include <stdint.h>
#include <stddef.h>
#include <asm/ptrace.h>
#include <hal/test/traps_core.h>

trap_handler_t sotftrap_handler[MAX_SOFTTRAP_NUMBER];
trap_handler_t hwtrap_handler[MAX_HWTRAP_NUMBER];

int mb_exception_dispatcher(uint32_t number, void *data, struct pt_regs *regs) {
	if (NULL == sotftrap_handler[number]) {
		return -1;
	}
	return sotftrap_handler[number](number, data);
}

void mb_hwtrap_dispatcher(uint32_t number, void *data, struct pt_regs  *regs) {
	if (NULL == hwtrap_handler[number])
		return;
	if (0 == hwtrap_handler[number](number, data)) {
		regs->regs[17] += 4;
	}
}
