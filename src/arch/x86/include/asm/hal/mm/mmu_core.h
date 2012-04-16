
/**
 * @file
 * @brief Defines specific features for mmu in x86 architecture.
 *
 * @date 20.03.12
 * @author Gleb Efimov
 */

#ifndef X86_MMU_CORE_H_
#define X86_MMU_CORE_H_

#include <hal/mm/mmu_core.h>


/** Level-2 Table:1024 entries, 4 bytes a piece */
#define __MMU_MTABLE_SIZE   0x400UL
/** Level-1 Table: 1024 entries, 4 bytes a piece */
#define __MMU_GTABLE_SIZE   0x400UL

/** 4K-byte pages */
#define __MMU_PAGE_SIZE		0x400UL

#define MMU_PTE_CACHE        0x08

#define __MMU_PAGE_CACHEABLE   MMU_PTE_CACHE

#endif /* X86_MMU_CORE_H_ */
