/**
 * @file
 * @brief Defines specific features for working with
 *        trap handlers in x86 architecture
 *
 * @date 21.09.11
 * @author Anton Bondarev
 */

#ifndef X86_TRAPS_CORE_H_
#define X86_TRAPS_CORE_H_

#include <asm/traps.h>
#include <stdint.h>

#define MAX_HWTRAP_NUMBER    0x14
#define MAX_SOFTTRAP_NUMBER  0x10
#define MAX_INTERRUPT_NUMBER 0x10

#define DIVZERO_FAULT        0x0
#define MMU_DATA_SECUR_FAULT X86_T_PAGE_FAULT //X86_T_GENERAL_PROTECTION
#define MMU_DATA_MISS_FAULT  X86_T_PAGE_FAULT

/** Defines handler for traps_dispatcher in microblaze archecture */
typedef int (*__trap_handler)(uint32_t nr, void *data);

/** Defines traps environment for sparc structure */
typedef struct __traps_env {
	uint32_t base_addr;
	__trap_handler hw_traps[MAX_HWTRAP_NUMBER];
	__trap_handler soft_traps[MAX_SOFTTRAP_NUMBER];
	__trap_handler interrupts[MAX_INTERRUPT_NUMBER];
}__traps_env_t;

#endif /* X86_TRAPS_CORE_H_ */
