/**
 * @file
 * @brief Describes interface for testing trap's environment
 *
 * @date 15.03.10
 * @author Anton Bondarev
 */

#ifndef TEST_TRAPS_CORE_H_
#define TEST_TRAPS_CORE_H_

#include <hal/env/traps_core.h>
#include <asm/hal/test/traps_core.h>
#include <stdint.h>

/**
 * Set specified handler for trap
 * @param type set type of trap
 * @param number set row's number in the trap table
 * @param handler is a pointer ro handler func
 */
extern void testtraps_set_handler(uint32_t type, int number, trap_handler_t handler);

typedef trap_handler_t fault_handler_t;

enum fault_type {
	MMU_DATA_SECUR,
	MMU_DATA_MISS,
	DIV_ZERO,
};

/**
 * @brief Set handler for IRQ, abort, traps and so on
 *
 * @param test_event_type
 * @param handler
 */
extern void set_fault_handler(enum fault_type type, fault_handler_t handler);

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


/**
 * @brief Switch to user mode to have some faults
 */
extern void mmu_drop_privileges(void);

/**
 * @brief Switch to sys mode to avoid errors in kernel space
 */
extern void mmu_sys_privileges(void);

#endif /* TEST_TRAPS_CORE_H_ */
