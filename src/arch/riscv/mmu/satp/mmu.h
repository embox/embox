
#ifndef RISCV64_MMU_H_
#define RISCV64_MMU_H_

#include <stdint.h>
#include <inttypes.h>

#ifdef RISC_MMU_SV48
#define MMU_LEVELS              4
#define SATP_MODE               (9ULL)
#else
#define MMU_LEVELS              3
#define SATP_MODE               (8ULL)
#endif

#define MMU_PTE_PMASK           (~((0x1FFUL << 53) | 0x3FFUL))

#define SATP_MODE_MASK          (0xFUL << 60)
#define SATP_ASID_MASK          (0xFFFFUL << 44)
#define SATP_PPN_MASK           (0xFFFFFFFFFFFUL)

// Define PTE flags
#define MMU_PAGE_VALID          (1UL << 0)   // Page Table Entry Present flag
#define MMU_PAGE_READABLE       (1UL << 1)   // Page Table Entry Readable flag
#define MMU_PAGE_WRITABLE       (1UL << 2)   // Page Table Entry Writable flag
#define MMU_PAGE_EXECUTABLE     (1UL << 3)   // Page Table Entry Executable flag
#define MMU_PAGE_USERMODE       (1UL << 4)   // Page Table Entry Usermode flag
#define MMU_PAGE_GLOBAL         (1UL << 5)   // Page Table Entry Usermode flag
#define MMU_PAGE_ACCESSED       (1UL << 6)   // Page Table Entry Accessed flag
#define MMU_PAGE_DIRTY          (1UL << 7)   // Page Table Entry Dirty flag

#define MMU_INIT_FLAGS          (MMU_PAGE_VALID | MMU_PAGE_ACCESSED | MMU_PAGE_DIRTY)
#define MMU_FLAG_MASK           ((MMU_INIT_FLAGS | MMU_PAGE_READABLE | MMU_PAGE_WRITABLE) | \
                                (MMU_PAGE_EXECUTABLE | MMU_PAGE_USERMODE | MMU_PAGE_GLOBAL))

// Page entry shifts for each level in the paging hierarchy

#define __MMU_SHIFT_0 39
#define __MMU_SHIFT_1 30
#define __MMU_SHIFT_2 21
#define __MMU_SHIFT_3 12

typedef uintptr_t __mmu_paddr_t;   // Physical address type
typedef uintptr_t __mmu_vaddr_t;   // Virtual address type

typedef uint64_t __mmu_ctx_t;      // MMU context type
typedef uint64_t __mmu_reg_t;      // 64-bit PTE for RISC-V

#endif
