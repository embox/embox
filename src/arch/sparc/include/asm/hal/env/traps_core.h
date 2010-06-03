/**
 * @file
 * @brief Defines specific features for working with
 * trap handlers in SPARC architecture.
 *
 * @date 15.03.2010
 * @author Anton Bondarev
 * @author Alexander Batyukov
 */

#ifndef SPARC_TRAPS_CORE_H_
#define SPARC_TRAPS_CORE_H_

#include <types.h>

/*FIXME move MMU_TABLE_SIZE to arch dependent config*/
#define TRAP_TABLE_SIZE      (256)

#define DIVZERO_FAULT        0x2A
#define MMU_DFAULT           0x09
#define MMU_IFAULT           0x01

/** Defines handler for traps_dispatcher in microblaze archecture */
typedef int (*__trap_handler)(uint32_t nr, void *data);

/** Defines traps environment for sparc structure */
typedef struct __traps_env {
	uint32_t base_addr;
	__trap_handler hw_traps[CONFIG_MAX_HWTRAP_NUMBER];
	__trap_handler soft_traps[CONFIG_MAX_SOFTTRAP_NUMBER];
	__trap_handler interrupts[CONFIG_MAX_INTERRUPT_NUMBER];
}__traps_env_t;

#endif /* SPARC_TRAPS_CORE_H_ */
