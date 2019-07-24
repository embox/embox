/**
 * @file
 * @brief
 *
 * @date 04.10.2012
 * @author Anton Bulychev
 */

#ifndef X86_MMU_H_
#define X86_MMU_H_

#include <stdint.h>
#include <inttypes.h>

#define MMU_LEVELS              2

#define __MMU_PGD_SHIFT         22
#define __MMU_PTE_SHIFT         12

typedef uintptr_t __mmu_paddr_t;
typedef uintptr_t __mmu_vaddr_t;

typedef uint32_t __mmu_ctx_t;

typedef uint32_t __mmu_reg_t;

#define __MMU_SHIFT_0    __MMU_PGD_SHIFT
#define __MMU_SHIFT_1    __MMU_PTE_SHIFT

#define __PRIxMMUREG PRIx32

#endif /* X86_MMU_H_ */
