/**
 * @file
 * @brief Describes interface for working with trap handlers
 *
 * @date 17.03.10
 * @author Anton Bondarev
 */

#ifndef TRAPS_CORE_H_
#define TRAPS_CORE_H_

#include <asm/hal/env/traps_core.h>

#define TRAP_TYPE_HARDTRAP  0
#define TRAP_TYPE_INTERRUPT 1
#define TRAP_TYPE_SOFTTRAP  2

/**
 * Specifies type for pointer to function which can use in trap_dispatcher
 * function. This handler's signature can be change in different platform.
 */
typedef __trap_handler trap_handler_t;

/**
 * Specifies type for traps environment. It's described in architecture part
 * in  structure @link #__traps_env_t @endlink. Each platform have own implementation for
 * this structure.
 */
typedef __traps_env_t traps_env_t;

/**
 * Setups traps environment in systems such as traps handlers and traps status.
 * Each architecture have own parameters of environment described in
 * @link #traps_env_t @endlink
 *
 * @param env structure with set of environments
 */
extern void traps_set_env(traps_env_t *env);

/**
 * Save traps environment in systems.
 *
 * @param env structure with set of environments
 */
extern void traps_save_env(traps_env_t *env);

/**
 * Restore traps environment in systems received from function
 * @link #traps_save_env() @endlink.
 *
 * @param env structure with set of environments
 */
extern void traps_restore_env(traps_env_t *env);

#if 0
may be not necessary
/**
 * Allows traps in system
 */
extern void traps_enable(void);

/**
 * Prohibits traps in system
 */
extern void traps_disable(void);
#endif

#endif /* TRAPS_CORE_H_ */
