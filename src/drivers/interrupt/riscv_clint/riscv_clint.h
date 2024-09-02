/**
 * @file clint.h
 *
 * @brief Function definitions for the RISC-V Core Local Interruptor (CLINT).
 *
 * @date 05.07.2024
 * @authored by Suraj Ravindra Sonawane
 */

#ifndef CLINT_H_
#define CLINT_H_

#include <stdint.h>

/**
 * Initializes the CLINT.
 *
 * This function initializes the CLINT by clearing the MSIP (Software Interrupt) and setting MTIMECMP
 * to its maximum value (0xFFFFFFFFFFFFFFFF).
 *
 * @return 0 on success.
 */
extern int clint_init(void);

/**
 * Configures the Software Interrupt (MSIP).
 *
 * This function configures the MSIP by writing a specific value (0 or 1) to its address.
 *
 * @param value The value (0 or 1) to set for MSIP.
 * @param hart_id The hart id (only for SiFive CLINT).
 */
extern void clint_configure_msip(uint8_t value, int hart_id);

/**
 * Sets the MTIMECMP register value.
 *
 * This function sets the MTIMECMP register to the provided 64-bit value.
 *
 * @param value The value to set for MTIMECMP.
 * @param hart_id The hart id (only for SiFive CLINT).
 */
extern void clint_set_mtimecmp(uint64_t value, int hart_id);

/**
 * Retrieves the current value of MTIME.
 *
 * This function reads and returns the current value of MTIME.
 *
 * @return The current value of MTIME.
 */
extern uint64_t clint_get_mtime(void);

#endif /* CLINT_H_ */