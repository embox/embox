/**
 * @file
 * @brief
 *
 * @date 12.11.12
 * @author Anton Bulychev
 */

#ifndef ARCH_NOMMU_H_
#define ARCH_NOMMU_H_

#define NOMMU

#include <stdint.h>

#define __MMU_PGD_SHIFT         24
#define __MMU_PMD_SHIFT         18
#define __MMU_PTE_SHIFT         12

#ifndef __ASSEMBLER__
typedef uintptr_t __mmu_paddr_t;
typedef uintptr_t __mmu_vaddr_t;

typedef uint32_t __mmu_ctx_t;

typedef uint32_t __mmu_pgd_t;
typedef uint32_t __mmu_pmd_t;
typedef uint32_t __mmu_pte_t;

typedef uint32_t __mmu_reg_t;

#endif /*__ASSEMBLER__ */

#endif /* ARCH_NOMMU_H_ */
