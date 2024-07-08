/**
 * @file
 *
 * @brief Implementation of the RISC-V Core Local Interruptor (CLINT) for interrupt control and timer management.
 *
 * @date 05.07.2024
 * @authored by Suraj Ravindra Sonawane
 */

#include <stdint.h>
#include <drivers/irqctrl.h>
#include <hal/reg.h>

#define CLINT_ADDR      OPTION_GET(NUMBER, base_addr)
#define MSIP_OFFSET     OPTION_GET(NUMBER, msip_offset)
#define MTIMECMP_OFFSET OPTION_GET(NUMBER, mtimecmp_offset)
#define MTIME_OFFSET    OPTION_GET(NUMBER, mtime_offset)

#define MSIP_ADDR       (CLINT_ADDR + MSIP_OFFSET)
#define MTIMECMP_ADDR   (CLINT_ADDR + MTIMECMP_OFFSET)
#define MTIME_ADDR      (CLINT_ADDR + MTIME_OFFSET)

/**
 * Initializes the CLINT.
 *
 * This function initializes the CLINT by clearing the MSIP (Software Interrupt) and setting MTIMECMP
 * to its maximum value (0xFFFFFFFFFFFFFFFF).
 *
 * @return 0 on success.
 */
static int clint_init(void) {
    // Initial configuration: clear MSIP and set MTIMECMP to max value
    REG32_STORE(MSIP_ADDR, 0);                      // Clear MSIP by writing 0 to its address
    REG64_STORE(MTIMECMP_ADDR, 0xFFFFFFFFFFFFFFFF); // Set MTIMECMP to max value
    REG64_STORE(MTIME_ADDR, 0);                     // Initialize MTIME to 0
    return 0;
}

/**
 * Configures the Software Interrupt (MSIP).
 *
 * This function configures the MSIP by writing a specific value (0 or 1) to its address.
 *
 * @param value The value (0 or 1) to set for MSIP.
 */
void clint_configure_msip(uint8_t value) {
    REG32_STORE(MSIP_ADDR, value & 1); // Write the least significant bit of 'value' to MSIP_ADDR
}

/**
 * Sets the MTIMECMP register value.
 *
 * This function sets the MTIMECMP register to the provided 64-bit value.
 *
 * @param value The value to set for MTIMECMP.
 */
void clint_set_mtimecmp(uint64_t value) {
    REG64_STORE(MTIMECMP_ADDR, value); // Write 'value' to MTIMECMP_ADDR
}

/**
 * Retrieves the current value of MTIME.
 *
 * This function reads and returns the current value of MTIME.
 *
 * @return The current value of MTIME.
 */
uint64_t clint_get_mtime(void) {
    return REG64_LOAD(MTIME_ADDR); // Read and return the value at MTIME_ADDR
}

IRQCTRL_DEF(riscv_clint, clint_init);
