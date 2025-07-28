#include <embox/test.h>
#include "../mmu.h"
#include <asm/csr.h> 
#include <stdint.h>
#include <util/log.h>
#include <hal/mmu.h>
#include <mem/vmem.h>

EMBOX_TEST_SUITE("RISC-V MMU Test Suite");

#define SATP_MODE_MASK    (0xF000000000000000ULL) // Mask for top 4 bits (mode)

/**
 * Test case for enabling MMU with one-to-one address mapping.
 *
 * This test verifies that the MMU can be enabled correctly and that
 * one-to-one address mapping is set up properly.
 */
TEST_CASE("MMU One-to-One Mapping") {
    uintptr_t pgd[512] __attribute__((aligned(MMU_PAGE_SIZE))) = {0}; // Page Global Directory
    uintptr_t *page_table = pgd; // Simple case: one-to-one mapping
    uintptr_t base_addr = 0x1000; // Base address for the page table
    size_t num_pages = 512; // Number of pages to map

    // Set up one-to-one address mapping
    for (size_t i = 0; i < num_pages; ++i) {
        mmu_set(0, &page_table[i], base_addr + i * MMU_PAGE_SIZE);
    }

    // Create and set MMU context
    mmu_ctx_t ctx = mmu_create_context(pgd);
    mmu_set_context(ctx);

    // Enable MMU
    mmu_on();

    // Verify that MMU is enabled
    // The mode field in the SATP register is stored in the top 4 bits (63-60).
    // We shift SATP_MODE_SV39 by 60 to move it to the correct position in the SATP register.
    test_assert_equal(read_csr(satp) & SATP_MODE_MASK, SATP_MODE_SV39 << 60); // Check that SV39 mode is enabled

    // Verify that one-to-one mapping is set up
    for (size_t i = 0; i < num_pages; ++i) {
        uintptr_t entry = mmu_pte_get(&page_table[i]);
        uintptr_t expected_addr = base_addr + i * MMU_PAGE_SIZE;
        test_assert_equal(entry & MMU_PAGE_MASK, expected_addr);
        test_assert(entry & MMU_PAGE_PRESENT); // Check that the page is marked as present
    }

    // Disable MMU
    mmu_off();

    // Verify that MMU is disabled
    test_assert_equal(read_csr(satp), 0); // Check that the SATP register is cleared
}
