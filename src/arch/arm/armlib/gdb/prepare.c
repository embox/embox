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
#include <hal/mem_barriers.h>

#define ICACHE_INVAL()                                                       \
	__asm__ __volatile__("mcr p15, 0, %0, c7, c5, 0" : : "r"(0) : "memory"); \
	isb()

extern void gdb_stub(void *regs);

static int handle_debug_fault(uint32_t nr, void *data) {
	gdb_stub(&(((excpt_context_t *)data)->ptregs));
	// ICACHE_INVAL();
	return 0;
}

void gdb_prepare_arch(void) {
	arm_inst_fault_table[DEBUG_FAULT] = handle_debug_fault;
	// ICACHE_INVAL();
}

void gdb_cleanup_arch(void) {
	arm_inst_fault_table[DEBUG_FAULT] = NULL;
}
