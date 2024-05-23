/**
 * @file testtraps_core.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-08-18
 */

#include <string.h>

#include <arm/exception.h>
#include <hal/reg.h>
#include <hal/test/traps_core.h>

void set_fault_handler(enum fault_type type, fault_handler_t handler) {
	switch (type) {
	case MMU_DATA_MISS:
		arm_data_fault_table[SECT_TRANSLATION_FAULT] = handler;
		arm_data_fault_table[PAGE_TRANSLATION_FAULT] = handler;
		arm_inst_fault_table[SECT_TRANSLATION_FAULT] = handler;
		arm_inst_fault_table[PAGE_TRANSLATION_FAULT] = handler;
		break;
	case MMU_DATA_SECUR:
		arm_data_fault_table[SECT_PERMISSION_FAULT] = handler;
		arm_data_fault_table[PAGE_PERMISSION_FAULT] = handler;
		arm_inst_fault_table[SECT_PERMISSION_FAULT] = handler;
		arm_inst_fault_table[PAGE_PERMISSION_FAULT] = handler;
		break;
	case BREAKPOINT:
		arm_inst_fault_table[DEBUG_FAULT] = handler;
		break;
	default:
		break;
	}
}

inline void mmu_drop_privileges(void) {
	__asm__ __volatile__("mrs r0, CPSR\n\t"
	                     "bic r0, #0xff\n\t"
	                     "orr r0, %[mode]\n\t"
	                     "msr cpsr, r0\n\t"
	                     "mov r0, r0\n\t"
	                     :
	                     : [mode] "J"(PSR_M_USR));
}

inline void mmu_sys_privileges(void) {
	__asm__ __volatile__("mrs r0, CPSR\n\t"
	                     "bic r0, #0xff\n\t"
	                     "orr r0, %[mode]\n\t"
	                     "msr cpsr, r0\n\t"
	                     "mov r0, r0\n\t"
	                     :
	                     : [mode] "J"(PSR_M_SYS));
}
