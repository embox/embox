/**
 * @file
 *
 * @brief RISC-V Physical Memory Protection (PMP) Driver
 *
 * @date 02.08.2024
 * @author Suraj Ravindra Sonawane
 */

#include "pmp.h"
#include <hal/reg.h>
#include <embox/unit.h>

/**
 * Writes to a PMP configuration register.
 *
 * @param reg The index of the PMP configuration register to write to.
 * @param value The value to write to the register.
 */
void write_pmpcfg(uint32_t reg, uint32_t value) {
    REG32_STORE(PMP_CFG_BASE + reg * 4, value);
}

/**
 * Writes to a PMP address register.
 *
 * @param reg The index of the PMP address register to write to.
 * @param value The address value to write to the register.
 */
void write_pmpaddr(uint32_t reg, uint32_t value) {
    REG32_STORE(PMP_ADDR_BASE + reg * 4, value);
}

/**
 * Initializes the PMP subsystem by clearing all PMP entries and setting up 
 * predefined PMP regions based on configuration options.
 *
 */
static int pmp_init(void) {
    unsigned long pmp_addr[PMP_NUM_REGISTERS] = {0};
    unsigned long pmp_cfg[(PMP_NUM_REGISTERS + 3) / 4] = {0};
    unsigned int index = 0;

    // Set up PMP entries based on configuration options
    #ifdef CONFIG_ROM_REGION
    set_pmp_entry(&index, PMP_R | PMP_X | PMP_TOR, ROM_BASE, ROM_SIZE, pmp_addr, pmp_cfg, PMP_NUM_REGISTERS, page_size);
    #endif

    #ifdef CONFIG_NULL_POINTER_EXCEPTION_DETECTION_PMP
    set_pmp_entry(&index, PMP_NA, CONFIG_NULL_PTR_BASE, CONFIG_NULL_PTR_SIZE, pmp_addr, pmp_cfg, PMP_NUM_REGISTERS, page_size);
    #endif

    #ifdef CONFIG_PMP_STACK_GUARD
    set_pmp_entry(&index, PMP_NA, CONFIG_STACK_GUARD_BASE, CONFIG_STACK_GUARD_SIZE, pmp_addr, pmp_cfg, PMP_NUM_REGISTERS, page_size);
    set_pmp_entry(&index, PMP_R | PMP_X | PMP_TOR, CONFIG_MPRV_BASE, CONFIG_MPRV_SIZE, pmp_addr, pmp_cfg, PMP_NUM_REGISTERS, page_size);
    #endif

    // Write configured PMP entries to registers
    for (unsigned int i = 0; i < index; i++) {
        write_pmpaddr(i, pmp_addr[i]);
        write_pmpcfg(i / 4, pmp_cfg[i / 4]);
    }

    return 0;
}

/**
 * Configures a PMP entry with specific parameters.
 *
 * @param index Pointer to the current index of the PMP entry to configure.
 * @param flags The flags that define the permissions and type of the PMP entry.
 * @param base The base address for the PMP entry.
 * @param size The size of the memory region to protect.
 * @param pmp_addr Array of PMP address values to be configured.
 * @param pmp_cfg Array of PMP configuration values to be set.
 * @param pmp_count The total number of PMP registers available.
 * @param page_size The size of a memory page, used to determine entry granularity.
 */
void set_pmp_entry(unsigned int *index, unsigned int flags, uintptr_t base, size_t size,
                   unsigned long *pmp_addr, unsigned long *pmp_cfg, size_t pmp_count, size_t page_size) {
    unsigned int i;
    size_t num_entries = (size + page_size - 1) / page_size;
    size_t napot_size;

    // Ensure there are enough PMP registers available
    if (*index + num_entries > pmp_count) {
        return; // Not enough PMP registers available
    }

    if (flags & PMP_TOR) {
        // TOR: Top of Range
        pmp_addr[*index] = base >> 2;
        pmp_cfg[*index / 4] |= flags;
        (*index)++;
    } else if (flags & PMP_NA4) {
        // NA4: Naturally Aligned 4-byte
        pmp_addr[*index] = base >> 2;
        pmp_cfg[*index / 4] |= flags;
        (*index)++;
    } else if (flags & PMP_NAPOT) {
        // NAPOT: Naturally Aligned Power of Two
        napot_size = size;
        napot_size = 1;
        while (napot_size < size) {
            napot_size <<= 1;
        }
        pmp_addr[*index] = (base >> 2) | (napot_size - 1);
        pmp_cfg[*index / 4] |= flags;
        (*index)++;
    } else {
        // Default case: Divide the region into pages
        for (i = 0; i < num_entries; i++) {
            pmp_addr[*index + i] = base + i * page_size;
            pmp_cfg[*index + i / 4] |= flags;
        }
        *index += num_entries;
    }
}

EMBOX_UNIT_INIT(pmp_init);