/**
 * @file
 *
 * @date 15.03.2010
 * @author Anton Bondarev
 * @author Alexander Batyukov
 */

#ifndef TRAPS_CORE_H_
#define TRAPS_CORE_H_

#include <types.h>

/*FIXME move MMU_TABLE_SIZE to arch dependent config*/
#define TRAP_TABLE_SIZE (5*8)

extern void traps_save_table(uint32_t *table);

extern void traps_restore_table(uint32_t *table);

extern void traps_enable(void);

extern void traps_disable(void);

extern void traps_status_save(uint32_t *status);

extern void traps_status_restore(uint32_t *status);

#endif /* TRAPS_CORE_H_ */
