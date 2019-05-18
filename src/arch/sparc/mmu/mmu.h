/**
 * @file
 * @brief
 *
 * @date 20.07.2014
 * @author Alexander Kalmuk
 */

#ifndef MMU_H_
#define MMU_H_

// FROM arch/generic/mmu/mmu.h

#include <stdint.h>

#define __MMU_PGD_SHIFT         24
#define __MMU_PMD_SHIFT         18
#define __MMU_PTE_SHIFT         12

#ifndef __ASSEMBLER__

typedef uint32_t __mmu_paddr_t;
typedef uint32_t __mmu_vaddr_t;

typedef uint32_t __mmu_ctx_t;

typedef uint32_t __mmu_pgd_t;
typedef uint32_t __mmu_pmd_t;
typedef uint32_t __mmu_pte_t;

typedef uint32_t __mmu_reg_t;

/*
 * Additional information, which was taken from SPARC and is used in
 * another RISC architectures.
 */

//extern void mmu_set_val(void *addr, unsigned long value);

extern __mmu_pgd_t *context_table[];

extern int ctx_counter;

#endif

#define MMU_CTX_PMASK        0xfffffff0
#define MMU_PTD_PMASK        0xfffffff0
#define MMU_PTE_PMASK        0xffffff00

#define MMU_ET_PTD           0x1
#define MMU_ET_PTE           0x2

#define MMU_PAGE_WRITABLE    ((1UL << 0) << 2)
#define MMU_PAGE_EXECUTABLE  ((1UL << 1) << 2)
#define MMU_PAGE_SUPERVISOR  ((1UL << 2) << 2)

#define MMU_PAGE_CACHEABLE   (1UL << 7)

#endif /* MMU_H_ */
