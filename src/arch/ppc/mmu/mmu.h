/**
 * @file
 * @brief
 *
 * @date 13.12.12
 * @author Ilia Vaprol
 */

#ifndef PPC_MMU_H_
#define PPC_MMU_H_

#include <stdint.h>

#define MMU_LEVELS              2

#define __MMU_PGD_SHIFT         22
#define __MMU_PTE_SHIFT         12

typedef uint32_t __mmu_paddr_t;
typedef uint32_t __mmu_vaddr_t;

typedef uint32_t __mmu_ctx_t;

typedef uint32_t __mmu_reg_t;

#define __MMU_SHIFT_0    __MMU_PGD_SHIFT
#define __MMU_SHIFT_1    __MMU_PTE_SHIFT

#endif /* PPC_MMU_H_ */
