/**
 * @file
 *
 * @date 25.03.2010
 * @author Alexander Batyukov
 */

#include <asm/ptrace.h>
#include <hal/test/testtraps_core.h>

trap_handler_t test_handler[CONFIG_TRAP_TABLE_SIZE];

void test_trap_handler(uint8_t tt, uint32_t *data, struct pt_regs *regs) {
	if (NULL == test_handler) {
		return;
	}
	test_handler[tt](tt, (void *)data);
//	if (0 != test_handler[tt](tt, (void *)data)) {
//		regs->pc = regs->npc;
//		regs->npc = regs->npc + 4;
//	}
}
