#include <embox/test.h>
#include "../pmp.h"
#include <hal/reg.h>

EMBOX_TEST_SUITE("PMP Driver Test Suite");

/**
 * Test case for setting a PMP entry.
 *
 * This test case validates the functionality of the `set_pmp_entry` function.
 * It ensures that the function correctly configures a PMP (Physical Memory Protection)
 * entry based on the given parameters and that the configuration is properly
 * reflected in the PMP address and configuration registers.
 */
TEST_CASE("Set PMP Entry") {
    unsigned int index = 0; // Starting index for PMP entries
    unsigned int flags = PMP_R | PMP_X | PMP_TOR; // Flags indicating read, execute, and Top of Range (TOR) permissions
    uintptr_t base = 0x80000100; // Base address for the PMP entry
    size_t size = 0x1000; // Size of the memory region to protect
    unsigned long pmp_addr[PMP_NUM_REGISTERS] = {0}; // Array to store configured PMP addresses
    unsigned long pmp_cfg[(PMP_NUM_REGISTERS + 3) / 4] = {0}; // Array to store configured PMP configuration values
    size_t pmp_count = PMP_NUM_REGISTERS; // Total number of available PMP registers
    size_t page_size = 0x1000; // Size of a memory page

    // Call the function to set up the PMP entry
    set_pmp_entry(&index, flags, base, size, pmp_addr, pmp_cfg, pmp_count, page_size);

    // Verify that the configured PMP address and configuration match the expected values
    test_assert_equal(pmp_addr[0], base >> 2); // Check if the address is correctly shifted
    test_assert_equal(pmp_cfg[0], flags); // Verify that the flags match the expected configuration
}

/**
 * Test case for invalid PMP entry.
 *
 * This test case examines how the `set_pmp_entry` function handles invalid input parameters,
 * such as an out-of-bounds index. The function should not write any PMP entries when the index
 * is invalid.
 */
TEST_CASE("Invalid PMP Entry") {
    unsigned int index = PMP_NUM_REGISTERS; // Invalid index value (out of bounds)
    unsigned int flags = PMP_R | PMP_X; // Flags indicating read and execute permissions
    uintptr_t base = 0x80000200; // Base address for the PMP entry
    size_t size = 0x1000; // Size of the memory region to protect
    unsigned long pmp_addr[PMP_NUM_REGISTERS] = {0}; // Array to store PMP addresses (should remain unchanged)
    unsigned long pmp_cfg[(PMP_NUM_REGISTERS + 3) / 4] = {0}; // Array to store PMP configuration values (should remain unchanged)
    size_t pmp_count = PMP_NUM_REGISTERS; // Total number of available PMP registers
    size_t page_size = 0x1000; // Size of a memory page

    // Call the function to test with an invalid index
    set_pmp_entry(&index, flags, base, size, pmp_addr, pmp_cfg, pmp_count, page_size);
}

/**
 * Test case for boundary conditions.
 *
 * This test case evaluates how the `set_pmp_entry` function handles boundary conditions,
 * such as when the size of the memory region to protect is larger than a single page.
 * It checks if the function correctly handles the case where multiple PMP entries are needed.
 */
TEST_CASE("Boundary Conditions") {
    unsigned int index = 0; // Starting index for PMP entries
    unsigned int flags = PMP_R | PMP_X | PMP_NAPOT; // Flags indicating read, execute, and Naturally Aligned Power of Two (NAPOT) permissions
    uintptr_t base = 0x80000100; // Base address for the PMP entry
    size_t size = 0x2000; // Size of the memory region to protect (larger than a single page)
    unsigned long pmp_addr[PMP_NUM_REGISTERS] = {0}; // Array to store configured PMP addresses
    unsigned long pmp_cfg[(PMP_NUM_REGISTERS + 3) / 4] = {0}; // Array to store configured PMP configuration values
    size_t pmp_count = PMP_NUM_REGISTERS; // Total number of available PMP registers
    size_t page_size = 0x1000; // Size of a memory page

    // Call the function to test boundary conditions
    set_pmp_entry(&index, flags, base, size, pmp_addr, pmp_cfg, pmp_count, page_size);

    // Verify that PMP entries were set up correctly
    // Adjust assertions based on the expected configuration for NAPOT entries
    test_assert_equal(pmp_addr[0], (base >> 2) | (0x2000 - 1)); // Check if address matches expected NAPOT configuration
    test_assert_equal(pmp_cfg[0], flags); // Verify that the configuration flags match the expected values
}
