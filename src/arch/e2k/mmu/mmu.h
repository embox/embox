/**
 * @file
 *
 * @date Apr 26, 2019
 * author: Anton Bondarev
 */

#ifndef SRC_ARCH_E2K_MMU_MMU_H_
#define SRC_ARCH_E2K_MMU_MMU_H_

#include <stdint.h>
#include <inttypes.h>

#define PAGE_SHIFT           12
#define MMU_LEVELS           4

#define __MMU_PGD_SHIFT      (PAGE_SHIFT + 3 * (PAGE_SHIFT-3)) /* 39 */
#define __MMU_PUD_SHIFT      (PAGE_SHIFT + 2 * (PAGE_SHIFT-3)) /* 30 */
#define __MMU_PMD_SHIFT      (PAGE_SHIFT + 1 * (PAGE_SHIFT-3)) /* 21 */
#define __MMU_PTE_SHIFT      (PAGE_SHIFT)

#define __MMU_SHIFT_0       __MMU_PGD_SHIFT
#define __MMU_SHIFT_1       __MMU_PUD_SHIFT
#define __MMU_SHIFT_2       __MMU_PMD_SHIFT
#define __MMU_SHIFT_3       __MMU_PTE_SHIFT

#define E2K_MMU_PAGE_P       0x0000000000000001ULL /* Page Present bit */
#define E2K_MMU_PAGE_W       0x0000000000000002ULL /* Writable (0 - only read) */
#define E2K_MMU_PAGE_UU2     0x0000000000000004ULL /* unused bit # 2 */
#define E2K_MMU_PAGE_PWT     0x0000000000000008ULL /* Write Through */
#define E2K_MMU_PAGE_CD1     0x0000000000000010ULL /* Cache disable (right bit) */
#define E2K_MMU_PAGE_A       0x0000000000000020ULL /* Accessed Page */
#define E2K_MMU_PAGE_D       0x0000000000000040ULL /* Page Dirty */
#define E2K_MMU_PAGE_HUGE    0x0000000000000080ULL /* Page Size */
#define E2K_MMU_PAGE_G       0x0000000000000100ULL /* Global Page */
#define E2K_MMU_PAGE_CD2     0x0000000000000200ULL /* Cache disable (left bit) */
#define E2K_MMU_PAGE_NWA     0x0000000000000400ULL /* Prohibit address writing */
#define E2K_MMU_PAGE_AVAIL   0x0000000000000800ULL /* Available page */
#define E2K_MMU_PAGE_PFN     0x000000fffffff000ULL /* Physical Page Number */
#define E2K_MMU_PAGE_VALID   0x0000010000000000ULL /* Valid Page */
#define E2K_MMU_PAGE_PV      0x0000020000000000ULL /* PriVileged Page */
#define E2K_MMU_PAGE_INT_PR  0x0000040000000000ULL /* Integer address access Protection */
#define E2K_MMU_PAGE_NON_EX  0x0000080000000000ULL /* Non Executable Page */
#define E2K_MMU_PAGE_RES     0x0000f00000000000ULL /* Reserved bits */
#define E2K_MMU_PAGE_C_UNIT  0xffff000000000000ULL /* Compilation Unit */

#define KERNEL_VMLPT_BASE_ADDR    0x0000ff8000000000UL

#ifndef __ASSEMBLER__

typedef uint64_t __mmu_paddr_t;
typedef uint64_t __mmu_vaddr_t;

typedef uint64_t __mmu_ctx_t;

typedef uint64_t __mmu_pgd_t;
typedef uint64_t __mmu_pmd_t;
typedef uint64_t __mmu_pte_t;

typedef uint64_t __mmu_reg_t;

#define __PRIxMMUREG PRIx64

#endif

#endif /* SRC_ARCH_E2K_MMU_MMU_H_ */
