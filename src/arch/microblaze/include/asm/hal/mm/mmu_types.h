/**
 * @file
 *
 * @date 04.07.10
 * @author Anton Bondarev
 */

#ifndef MICROBLAZE_MMU_TYPES_H_
#define MICROBLAZE_MMU_TYPES_H_

#include <types.h>

/* quantity of utlb records */
#define UTLB_QUANTITY_RECORDS    0x40

/** Microblaze specific MMU table record */
typedef struct __mmu_table_record {
	uint32_t tlbhi;  /**< register rtlbhi */
	uint32_t tlblo;  /**< register rtlblo */
} __mmu_table_record_t;

typedef __mmu_table_record_t __mmu_table_t[UTLB_QUANTITY_RECORDS];

typedef uint32_t __mmu_page_flags_t;

typedef uint32_t __mmu_paddr_t;
typedef uint32_t __mmu_vaddr_t;
typedef uint32_t __mmu_pgd_t;
typedef uint32_t __mmu_pmd_t;
typedef uint32_t __mmu_pte_t;
typedef uint16_t __mmu_ctx_t; /* 256 process id and error error code*/

/**
 * Describes structure for MMU environment for Microblaze architecture.
 */
typedef struct __mmu_env {
	uint32_t status;                 /**< MMU enabled/disabled */

	__mmu_table_record_t *utlb_table;/**< Pointer to utlb content */

	size_t data_fault_cnt;           /**< Counter for data page faults */
	size_t inst_fault_cnt;    /**< Counter for instruction page faults */
	__mmu_vaddr_t fault_addr;        /**< Last fault address */
}__mmu_env_t;

#endif /* MICROBLAZE_MMU_TYPES_H_ */
