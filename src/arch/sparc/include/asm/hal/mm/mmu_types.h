/**
 * @file
 *
 * @date 04.07.2010
 * @author Anton Bondarev
 */

#ifndef SPARC_MMU_TYPES_H_
#define SPARC_MMU_TYPES_H_

#include <types.h>

typedef uint32_t __mmu_paddr_t;
typedef uint32_t __mmu_vaddr_t;
typedef uint32_t __mmu_pgd_t;
typedef uint32_t __mmu_pmd_t;
typedef uint32_t __mmu_pte_t;

typedef uint32_t __mmu_page_flags_t;

typedef uint32_t __mmu_ctx_t; /* 256 process id and error error code*/

/**
 * Describes structure for MMU environment for SPARC architecture.
 */
typedef struct __mmu_env {
	uint32_t status;          /**< MMU enabled/disabled */

	__mmu_ctx_t *ctx;              /**< context table */
	uint32_t cur_ctx;

	uint32_t data_fault_cnt;  /**< Counter for data page faults */
	uint32_t inst_fault_cnt;  /**< Counter for instruction page faults */
	uint32_t fault_addr;      /**< Last fault address */
}__mmu_env_t;

#endif /* SPARC_MMU_TYPES_H_ */
