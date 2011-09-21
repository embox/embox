/**
 * @file
 *
 * @brief
 *
 * @date 21.09.2011
 * @author Anton Bondarev
 */

#ifndef X86_TRAPS_CORE_H_
#define X86_TRAPS_CORE_H_

#include <types.h>

#define MAX_HWTRAP_NUMBER    0x14
#define MAX_SOFTTRAP_NUMBER  0x10
#define MAX_INTERRUPT_NUMBER 0x10

#define DIVZERO_FAULT        0x0

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
