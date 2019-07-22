/**
 * @file
 * @brief Defines specific features for MMU in Microblaze architecture.
 *
 * @date 15.03.10
 * @author Anton Bondarev
 */

#ifndef MICROBLAZE_MMU_CORE_H_
#define MICROBLAZE_MMU_CORE_H_

#include <stdint.h>
#include <asm/bitops.h>

#define RTLBLO_I_BIT           REVERSE_BIT(29)
#define RTLBLO_EX_BIT          REVERSE_BIT(22)
#define RTLBLO_WR_BIT          REVERSE_BIT(23)

#define RTLBHI_SIZE_BIT        REVERSE_BIT(24)
#define RTLBHI_V_BIT           REVERSE_BIT(25)

#define PAGE_CACHEABLE         (1UL << RTLBLO_I_BIT)
#define PAGE_WRITEABLE         (1UL << RTLBLO_WR_BIT)
#define PAGE_EXECUTEABLE       (1UL << RTLBLO_EX_BIT)

#define RTLBHI_SIZE_1K    0x0
#define RTLBHI_SIZE_4K    0x1
#define RTLBHI_SIZE_16K   0x2
#define RTLBHI_SIZE_64K   0x3
#define RTLBHI_SIZE_256K  0x4
#define RTLBHI_SIZE_1M    0x5
#define RTLBHI_SIZE_4M    0x6
#define RTLBHI_SIZE_16M   0x7

#define RTLBLO_SET(var, phy_addr, cacheable, ex, wr)  \
	var = (phy_addr |                             \
			(cacheable << RTLBLO_I_BIT) | \
			(ex << RTLBLO_EX_BIT)       | \
			(wr << RTLBLO_WR_BIT))

#define RTLBHI_SET(var, virt_addr, size) \
	var = (virt_addr | (size << RTLBHI_SIZE_BIT) | \
			(1 << RTLBHI_V_BIT))

/* quantity of utlb records */
#define UTLB_QUANTITY_RECORDS    0x40

/** Microblaze specific MMU table record */
typedef struct mmu_utlb_record {
	uint32_t tlbhi;  /**< register rtlbhi */
	uint32_t tlblo;  /**< register rtlblo */
} mmu_utlb_record_t;

#endif /* MICROBLAZE_MMU_CORE_H_ */
