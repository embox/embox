/**
 * @file
 * @brief Defines specific features for MMU in Microblaze architecture.
 *
 * @date 15.03.10
 * @author Anton Bondarev
 */

#ifndef MICROBLAZE_MMU_CORE_H_
#define MICROBLAZE_MMU_CORE_H_

#include <types.h>
#include <bitops.h>
#include <hal/mm/mmu_types.h>


#define RTLBLO_I_BIT           REVERSE_BIT(29)
#define RTLBLO_EX_BIT          REVERSE_BIT(22)
#define RTLBLO_WR_BIT          REVERSE_BIT(23)

#define RTLBHI_SIZE_BIT        REVERSE_BIT(24)
#define RTLBHI_V_BIT           REVERSE_BIT(25)

#define __MMU_PAGE_CACHEABLE    (1 << RTLBLO_I_BIT)
#define __MMU_PAGE_WRITEABLE    (1 << RTLBLO_WR_BIT)
#define __MMU_PAGE_EXECUTEABLE  (1 << RTLBLO_EX_BIT)

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

#endif /* MICROBLAZE_MMU_CORE_H_ */
