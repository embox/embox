/**
 * @file
 *
 * @brief RISC-V Physical Memory Protection (PMP) Driver Header
 *
 * @date 02.08.2024
 * @author Suraj Ravindra Sonawane
 */

#include <stdint.h>
#include <stddef.h>

#ifndef PMP_H_
#define PMP_H_

// PMP Configuration Base Address and Number of Registers
#define PMP_CFG_BASE  0x3A0
#define PMP_ADDR_BASE 0x3B0
#define PMP_NUM_REGISTERS 16 // Number of PMP registers available

// PMP Access Flags
#define PMP_R 0x1  // Read Access
#define PMP_W 0x2  // Write Access
#define PMP_X 0x4  // Execute Access
#define PMP_NA 0x0 // No Access
#define PMP_TOR 0x8 // Top of Range
#define PMP_NA4 0x10 // Naturally Aligned 4-byte Region
#define PMP_NAPOT 0x20 // Naturally Aligned Power of Two Region

typedef struct {
    uintptr_t address;
    uint32_t config;
} pmp_region_t;

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
                   unsigned long *pmp_addr, unsigned long *pmp_cfg, size_t pmp_count, size_t page_size);

/**
 * Writes to a PMP configuration register.
 *
 * @param reg The index of the PMP configuration register to write to.
 * @param value The value to write to the register.
 */
void write_pmpcfg(uint32_t reg, uint32_t value);

/**
 * Writes to a PMP address register.
 *
 * @param reg The index of the PMP address register to write to.
 * @param value The address value to write to the register.
 */
void write_pmpaddr(uint32_t reg, uint32_t value);

#endif /* PMP_H_ */
