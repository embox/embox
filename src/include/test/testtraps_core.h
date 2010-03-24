/**
 * @file
 *
 * @brief Describes interface for testing trap's environment
 *
 * @date 15.03.2010
 * @author Anton Bondarev
 */

#ifndef TESTTRAPS_H_
#define TESTTRAPS_H_

#include <types.h>
#include <hal/traps_core.h>
#include <asm/test/testtraps_core.h>

extern void testtraps_set_handler(uint32_t type, int number, trap_handler_t handler);

extern int testtraps_fire_softtrap(uint32_t number);

/**
 * Initializes testing trap's environment.
 *
 * @return testing trap's environment
 * @retval pointer to trap_env structure on success
 * @retval NULL on fail
 */
extern traps_env_t *testtraps_env(void);

#endif /* TESTTRAPS_H_ */
