/**
 * @file
 *
 * @date 15.03.2010
 * @author Anton Bondarev
 */

#ifndef MICROBLAZE_TRAPS_CORE_H_
#define MICROBLAZE_TRAPS_CORE_H_

#include <types.h>

/*FIXME move MMU_TABLE_SIZE to arch dependent config*/
#define TRAP_TABLE_SIZE      (5*8)
#define MAX_HWTRAP_NUMBER    0x8
#define MAX_SOFTTRAP_NUMBER  0x10
#define MAX_INTERRUPT_NUMBER 0x10
typedef void (*traps_handler_t)(uint32_t trap_nr, void *data);
typedef struct __traps_env {
	uint32_t status;
	uint32_t base_addr;
	traps_handler_t hw_traps[MAX_HWTRAP_NUMBER];
	traps_handler_t soft_traps[MAX_SOFTTRAP_NUMBER];
	traps_handler_t interupts[MAX_INTERRUPT_NUMBER];
}__traps_env_t;

#endif /* MICROBLAZE_TRAPS_CORE_H_ */
