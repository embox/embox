/**
 * @file
 *
 * @date 25.03.2010
 * @author Alexander Batyukov
 */

#include <asm/ptrace.h>
#include <hal/test/traps_core.h>

trap_handler_t test_handlers[CONFIG_TRAP_TABLE_SIZE];

/**
 * Calls handler to trap process. Handler should return 0,
 * otherwise last instruction will be reexecuted.
 * @param tt trap number in table
 * @param data pointer to same data
 * @param regs pointer to kernel regs struct
 */

void test_trap_handler(uint8_t tt, uint32_t *data, struct pt_regs *regs) {
	if (NULL == test_handlers[tt]) {
		printf("asdf");
		return;
	}
	if (0 == test_handlers[tt](tt, (void *)data)) {
		regs->pc = regs->npc;
		regs->npc = regs->npc + 4;
	}
}
