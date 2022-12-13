/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 06.11.22
 */

#include <inttypes.h>

#include <kernel/printk.h>

#include "exceptions.h"

extern uint32_t _get_mmu_instruction_fault_status(void);

void _NORETURN arm_pabt_handler(excpt_context_t *ctx) {
	uint32_t fault_status;

	fault_status = _get_mmu_instruction_fault_status();
	printk("\nUnresolvable prefetch abort exception!\n");
	printk("Fault status = %" PRIu32 "\n", fault_status);
	PRINT_PTREGS(&ctx->ptregs);
	while (1)
		;
}
