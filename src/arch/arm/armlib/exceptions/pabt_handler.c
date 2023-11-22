/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 06.11.22
 */

#include <inttypes.h>
#include <stdint.h>

#include <arm/exception.h>
#include <asm/cp15.h>
#include <hal/test/traps_core.h>
#include <util/log.h>

fault_handler_t arm_inst_fault_table[0x10];

void arm_pabt_handler(excpt_context_t *ctx) {
	uint32_t fault_status;

	fault_status = cp15_get_instruction_fault_status() & 0xf;

	if (arm_inst_fault_table[fault_status]) {
		arm_inst_fault_table[fault_status](fault_status, ctx);
	}
	else {
		log_raw(LOG_EMERG, "\nUnresolvable prefetch abort exception!\n");
		log_raw(LOG_EMERG, "Fault status = %" PRIu32 "\n", fault_status);
		arm_show_excpt_context(ctx);
		while (1) {};
	}
}
