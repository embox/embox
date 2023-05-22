/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 13.05.23
 */
#include <stddef.h>
#include <stdint.h>

#include <arm/exception.h>

extern void icache_inval(void);
extern void gdb_stub(void *regs);

static int handle_debug_fault(uint32_t nr, void *data) {
	gdb_stub(&(((excpt_context_t *)data)->ptregs));
	icache_inval();
	return 0;
}

void gdb_prepare_arch(void) {
	arm_inst_fault_table[DEBUG_FAULT] = handle_debug_fault;
}

void gdb_cleanup_arch(void) {
	arm_inst_fault_table[DEBUG_FAULT] = NULL;
}
