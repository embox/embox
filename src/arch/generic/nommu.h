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

typedef uintptr_t __mmu_paddr_t;
typedef uintptr_t __mmu_vaddr_t;

typedef uint32_t __mmu_ctx_t;

typedef uint32_t __mmu_pgd_t;
typedef uint32_t __mmu_pmd_t;
typedef uint32_t __mmu_pte_t;

#endif /* ARCH_NOMMU_H_ */
