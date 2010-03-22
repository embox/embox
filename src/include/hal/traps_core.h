/**
 * @file
 *
 * @brief Describes interface for working with trap handlers
 *
 * @date 17.03.2010
 * @author Anton Bondarev
 */

#ifndef TRAPS_CORE_H_
#define TRAPS_CORE_H_

#include <asm/traps_core.h>

#define TRAP_TYPE_HARDTRAP  0
#define TRAP_TYPE_INTERRUPT 1
#define TRAP_TYPE_SOFTTRAP  2

typedef __traps_env_t traps_env_t ;

extern void traps_save_env(traps_env_t *env);

extern void traps_restore_env(traps_env_t *env);

extern void traps_set_env(traps_env_t *env);

/**
 * Allows traps in system
 */
extern void traps_enable(void);

/**
 * Prohibits traps in system
 */
extern void traps_disable(void);

#endif /* TRAPS_CORE_H_ */
