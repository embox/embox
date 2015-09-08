/**
 * @file mmu_test.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-08-18
 */

#include <string.h>

#include <asm/modes.h>
#include <hal/test/traps_core.h>

extern void *data_abt_handler_addr;
extern void *data_abt_handler;

void set_fault_handler(enum fault_type type, fault_handler_t handler) {
	switch (type) {
		case MMU_DATA_MISS:
			if (handler == NULL)
				handler = data_abt_handler;
			memcpy(&data_abt_handler_addr, &handler, sizeof(uintptr_t));
		default:
			return;
	}
}

inline void mmu_drop_privileges(void) {
	__asm__ __volatile__ (
		"mrs r0, CPSR\n\t"
		"bic r0, #0xff\n\t"
		"orr r0, %[mode]\n\t"
		"msr cpsr, r0\n\t"
		"mov r0, r0\n\t": : [mode] "J" (ARM_MODE_USR)
	);
}

inline void mmu_sys_privileges(void) {
	__asm__ __volatile__ (
		"mrs r0, CPSR\n\t"
		"bic r0, #0xff\n\t"
		"orr r0, %[mode]\n\t"
		"msr cpsr, r0\n\t"
		"mov r0, r0\n\t": : [mode] "J" (ARM_MODE_SYS)
	);
}
