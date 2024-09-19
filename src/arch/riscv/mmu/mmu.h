/**
 * @file
 * @brief RISC-V MMU Header File
 *
 * @date 04.09.2024
 */

#ifndef RISCV_MMU_H_
#define RISCV_MMU_H_

#include <stdint.h>
#include <inttypes.h>

#define MMU_LEVELS              2

// Define PTE flags
#define MMU_PAGE_PRESENT        (1UL << 0)   // Page Table Entry Present flag
#define MMU_PAGE_WRITABLE       (1UL << 1)   // Page Table Entry Writable flag
#define MMU_PAGE_USERMODE       (1UL << 2)   // Page Table Entry Usermode flag
#define MMU_PAGE_DISABLE_CACHE  (1UL << 3)   // Page Table Entry Disable Cache flag (if applicable)
#define MMU_PAGE_EXECUTABLE     (1UL << 5)   // Page Table Entry Executable flag

typedef uintptr_t __mmu_paddr_t;   // Physical address type
typedef uintptr_t __mmu_vaddr_t;   // Virtual address type

typedef uint32_t __mmu_ctx_t;      // MMU context type
typedef uint64_t __mmu_reg_t;      // 64-bit PTE for RISC-V

#define __PRIxMMUREG PRIx64       // For 64-bit PTE format specifier

// Page entry shifts for each level in the paging hierarchy

#define __MMU_SHIFT_0 22
#define __MMU_SHIFT_1 12
#define __MMU_SHIFT_2 21
#define __MMU_SHIFT_3 12

#define SATP_MODE_SV32    1
#define SATP_MODE_SV39    (8ULL) 
#endif /* RISCV_MMU_H_ */
