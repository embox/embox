/**
 * @file
 *
 * @date 15.03.10
 * @author Anton Bondarev
 */

#ifndef MICROBLAZE_TRAPS_CORE_H_
#define MICROBLAZE_TRAPS_CORE_H_

#include <types.h>

/*FIXME move UTLB_QUANTITY_RECORDS to arch dependent config*/
#define MAX_HWTRAP_NUMBER    0x14
#define MAX_SOFTTRAP_NUMBER  0x10
#define MAX_INTERRUPT_NUMBER 0x10

#define DIVZERO_FAULT        0x5
#define MMU_DFAULT           0x12
#define MMU_IFAULT           0x13

/** Defines handler for traps_dispatcher in microblaze archecture */
typedef int (*__trap_handler)(uint32_t nr, void *data);

/** Defines traps environment for microblaze structure */
typedef struct __traps_env {
	uint32_t status;
	uint32_t base_addr;
	__trap_handler hw_traps[MAX_HWTRAP_NUMBER];
	__trap_handler soft_traps[MAX_SOFTTRAP_NUMBER];
	__trap_handler interrupts[MAX_INTERRUPT_NUMBER];
}__traps_env_t;

/**
 * Massive for sotf trap handlers. It's used in file
 * @link #exception_handler.c @endlink by mb_exception_dispatcher function
 */
extern __trap_handler sotftrap_handler[MAX_SOFTTRAP_NUMBER];
/**
 * Massive for sotf trap handlers. It's used in file
 * @link #exception_handler.c @endlink by mb_hwtrap_handler function
 */
extern __trap_handler hwtrap_handler[MAX_HWTRAP_NUMBER];

#endif /* MICROBLAZE_TRAPS_CORE_H_ */
