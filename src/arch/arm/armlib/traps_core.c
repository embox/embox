/**
 * @file testtraps_core.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-08-18
 */

#include <string.h>

#include <asm/modes.h>
#include <hal/test/traps_core.h>

#define FAULT_DEBUG       2
#define FAULT_TRANSL_SECT 5
#define FAULT_TRANSL_PAGE 7
#define FAULT_PERMIT_SECT 13
#define FAULT_PERMIT_PAGE 15

extern fault_handler_t __fault_table[0x10];
extern fault_handler_t __ifault_table[0x10];

void set_fault_handler(enum fault_type type, fault_handler_t handler) {
	switch (type) {
	case MMU_DATA_MISS:
		__fault_table[FAULT_TRANSL_SECT] = handler;
		__fault_table[FAULT_TRANSL_PAGE] = handler;
		__ifault_table[FAULT_TRANSL_SECT] = handler;
		__ifault_table[FAULT_TRANSL_PAGE] = handler;
		break;

	case MMU_DATA_SECUR:
		__fault_table[FAULT_PERMIT_SECT] = handler;
		__fault_table[FAULT_PERMIT_PAGE] = handler;
		__ifault_table[FAULT_PERMIT_SECT] = handler;
		__ifault_table[FAULT_PERMIT_PAGE] = handler;
		break;

	case BREAKPOINT:
		__fault_table[FAULT_DEBUG] = handler;
		__ifault_table[FAULT_DEBUG] = handler;
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
	                     : [mode] "J"(ARM_MODE_USR));
}

inline void mmu_sys_privileges(void) {
	__asm__ __volatile__("mrs r0, CPSR\n\t"
	                     "bic r0, #0xff\n\t"
	                     "orr r0, %[mode]\n\t"
	                     "msr cpsr, r0\n\t"
	                     "mov r0, r0\n\t"
	                     :
	                     : [mode] "J"(ARM_MODE_SYS));
}
