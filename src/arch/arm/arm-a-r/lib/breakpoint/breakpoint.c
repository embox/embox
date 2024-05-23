/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 21.08.23
 */
#include <assert.h>
#include <stdint.h>

#include <arm/exception.h>
#include <debug/breakpoint.h>

static bpt_handler_t __bpt_handler;

static int arm_debug_fault_handler(uint32_t nr, void *data) {
	assert(__bpt_handler);

	__bpt_handler((void *)&((excpt_context_t *)data)->ptregs);

	return 0;
}

void bpt_handler_set(bpt_handler_t handler) {
	__bpt_handler = handler;

	if (handler) {
		arm_inst_fault_table[DEBUG_FAULT] = arm_debug_fault_handler;
	}
	else {
		arm_inst_fault_table[DEBUG_FAULT] = NULL;
	}
}
