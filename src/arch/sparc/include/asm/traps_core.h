/**
 * @file
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
#define MAX_HWTRAP_NUMBER    0x40
#define MAX_SOFTTRAP_NUMBER  0x80
#define MAX_INTERRUPT_NUMBER 0x40

#define MIN_SOFT_TRAP_NUMBER 0x80

/** Defines handler for traps_dispatcher in microblaze archecture */
typedef void (*__trap_handler)(uint32_t nr, void *data);

/** Defines traps environment for microblaze structure */
typedef struct __traps_env {
	uint32_t base_addr;
	__trap_handler hw_traps[MAX_HWTRAP_NUMBER];
	__trap_handler soft_traps[MAX_SOFTTRAP_NUMBER];
	__trap_handler interupts[MAX_INTERRUPT_NUMBER];
}__traps_env_t;

#endif /* SPARC_TRAPS_CORE_H_ */


