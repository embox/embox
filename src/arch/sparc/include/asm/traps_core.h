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
#define TRAP_TABLE_SIZE (5*8)

/** Defines handler for traps_dispatcher in microblaze archecture */
typedef void (*__trap_handler)(uint32_t nr, void *data);

/** Defines traps environment for microblaze structure */
typedef struct __traps_env {
	//TODO:
}__traps_env_t;

#endif /* SPARC_TRAPS_CORE_H_ */
