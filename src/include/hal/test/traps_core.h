/**
 * @file
 * @brief Describes interface for testing trap's environment
 *
 * @date 15.03.2010
 * @author Anton Bondarev
 */

#ifndef TEST_TRAPS_CORE_H_
#define TEST_TRAPS_CORE_H_

#include <types.h>
#include <hal/env/traps_core.h>
#include <asm/hal/test/traps_core.h>

/**
 * Set specified handler for trap
 * @param type set type of trap
 * @param number set row's number in the trap table
 * @param handler is a pointer ro handler func
 */
extern void testtraps_set_handler(uint32_t type, int number, trap_handler_t handler);

/**
 * Causes trap with specified number
 * @param number
 * @param data is a pointer to memory area with specific data
 */
extern int testtraps_fire_softtrap(uint32_t number, void *data);

/**
 * Initializes testing trap's environment.
 *
 * @return testing trap's environment
 * @retval pointer to trap_env structure on success
 * @retval NULL on fail
 */
extern traps_env_t *testtraps_env(void);

#endif /* TEST_TRAPS_CORE_H_ */
