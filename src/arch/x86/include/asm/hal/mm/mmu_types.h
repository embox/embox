/**
 * @file
 * @brief Defines environment for MMU in x86 architecture.
 *
 * @date 20.03.12
 * @author Gleb Efimov
 */

#ifndef X86_MMU_TYPES_H_
#define X86_MMU_TYPES_H_

#include <types.h>

typedef uint32_t __mmu_paddr_t;
typedef uint32_t __mmu_vaddr_t;
typedef uint32_t __mmu_pgd_t;
typedef uint32_t __mmu_pmd_t;
typedef uint32_t __mmu_pte_t;

typedef uint32_t __mmu_page_flags_t;

typedef uint32_t __mmu_ctx_t; /* 256 process id and error error code*/

typedef struct __mmu_env {
	uint32_t status;          /**< MMU enabled/disabled */

	__mmu_ctx_t *ctx;              /**< context table */
	uint32_t cur_ctx;

	uint32_t data_fault_cnt;  /**< Counter for data page faults */
	uint32_t inst_fault_cnt;  /**< Counter for instruction page faults */
	uint32_t fault_addr;      /**< Last fault address */
}__mmu_env_t;


#endif /* X86_MMU_TYPES_H_ */
