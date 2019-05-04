/**
 * @file
 * @brief
 *
 * @date 03.12.12
 * @author Anton Bulychev
 */

#ifndef ARCH_GENERIC_MMU_H_
#define ARCH_GENERIC_MMU_H_

#include <stdint.h>

#define __MMU_PGD_SHIFT         24
#define __MMU_PMD_SHIFT         18
#define __MMU_PTE_SHIFT         12

typedef uintptr_t __mmu_paddr_t;
typedef uintptr_t __mmu_vaddr_t;

typedef uint32_t __mmu_ctx_t;

typedef uint32_t __mmu_pgd_t;
typedef uint32_t __mmu_pmd_t;
typedef uint32_t __mmu_pte_t;

typedef uint32_t __mmu_reg_t;

/*
 * Additional information, which was taken from SPARC and is used in
 * another RISC architectures.
 */

extern void mmu_set_val(void *addr, unsigned long value);

extern __mmu_pgd_t *context_table[];

extern int ctx_counter;

#define MMU_CTX_PMASK        0xfffffff0
#define MMU_PTD_PMASK        0xfffffff0
#define MMU_PTE_PMASK        0xffffff00

#define MMU_ET_PTD           0x1
#define MMU_ET_PTE           0x2
#define MMU_ET_PRESENT       0x3

#define MMU_PAGE_WRITABLE    ((1UL << 0) << 2)
#define MMU_PAGE_SOMEFLAG    ((1UL << 1) << 2)
#define MMU_PAGE_SUPERVISOR  ((1UL << 2) << 2)

#define MMU_PAGE_CACHEABLE   (1UL << 7)

/* Useful defines */
#define mmu_set_ptd_entry(addr,ptr)                              \
	mmu_set_val((void *)(addr),                                  \
			    ((((unsigned long)(ptr))>>4) & MMU_PTD_PMASK)    \
				| (MMU_ET_PTD))

#define mmu_set_pte_entry(addr,ptr,flags)                        \
	mmu_set_val((void *)(addr),                                  \
			    ((((unsigned long)(ptr))>>4) & MMU_PTE_PMASK)    \
				| (MMU_ET_PTE | flags))

#define mmu_get_ptd_ptr(entry) \
	((((unsigned long)(entry)) & MMU_PTD_PMASK) << 4)

#define mmu_get_pte_ptr(entry) \
	((((unsigned long)(entry)) & MMU_PTE_PMASK) << 4)

#endif /* ARCH_GENERIC_MMU_H */
