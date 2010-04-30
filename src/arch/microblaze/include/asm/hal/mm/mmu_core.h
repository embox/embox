/**
 * @file
 *
 * @brief Defines specific features for MMU in Microblaze architecture.
 *
 * @date 15.03.2010
 * @author Anton Bondarev
 */

#ifndef MICROBLAZE_MMU_CORE_H_
#define MICROBLAZE_MMU_CORE_H_

#include <types.h>

/* quantity of utlb records */
#define UTLB_QUANTITY_RECORDS    0x40


#define RTLBLO_I_BIT      REVERSE_BIT(29)
#define RTLBLO_EX_BIT     REVERSE_BIT(22)
#define RTLBLO_WR_BIT     REVERSE_BIT(23)

#define RTLBHI_SIZE_BIT   REVERSE_BIT(24)
#define RTLBHI_V_BIT      REVERSE_BIT(25)


#define RTLBHI_SIZE_1K    0x0
#define RTLBHI_SIZE_4K    0x1
#define RTLBHI_SIZE_16K   0x2
#define RTLBHI_SIZE_64K   0x3
#define RTLBHI_SIZE_256K  0x4
#define RTLBHI_SIZE_1M    0x5
#define RTLBHI_SIZE_4M    0x6
#define RTLBHI_SIZE_16M   0x7

#define RTLBLO_SET(var, phy_addr, cacheable, ex, wr) \
	var = (phy_addr                     | \
			(cacheable << RTLBLO_I_BIT) | \
			(ex << RTLBLO_EX_BIT)       | \
			(wr << RTLBLO_WR_BIT))

#define RTLBHI_SET(var, virt_addr, size) \
	var = (virt_addr | (size << RTLBHI_SIZE_BIT) | \
			(1 << RTLBHI_V_BIT))

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
typedef uint16_t __mmu_ctx_t; /* 256 process id and error error code*/

/**
 * Describes structure for MMU environment for Microblaze architecture.
 */
typedef struct __mmu_env {
	uint32_t status;                    /**< MMU enabled/disabled */

	__mmu_table_record_t * utlb_table;  /**< Pointer to utlb content */

	size_t data_fault_cnt;              /**< Counter for data page faults */
	size_t inst_fault_cnt;              /**< Counter for instruction page faults */
	__mmu_vaddr_t fault_addr;           /**< Last fault address */
}__mmu_env_t;

#endif /* MICROBLAZE_MMU_CORE_H_ */
