/**
 * @file mmu.h
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 07.08.2019
 */

#ifndef AARCH_MMU_H_
#define AARCH_MMU_H_

#include <inttypes.h>
#include <stdint.h>

#include <framework/mod/options.h>

#define AARCH64_MMU_GRANULE \
	OPTION_MODULE_GET(embox__arch__aarch64__mmu, NUMBER, granule)

/* Without extensions Aarch64 VA are 48-bit long */
#define MMU_VADDR_WIDTH 48

#define __PRIxMMUREG    PRIx64

#if AARCH64_MMU_GRANULE == 4
#define MMU_LEVELS    4
#define __MMU_SHIFT_0 39
#define __MMU_SHIFT_1 30
#define __MMU_SHIFT_2 21
#define __MMU_SHIFT_3 12
#elif AARCH64_MMU_GRANULE == 16
#define MMU_LEVELS    4
#define __MMU_SHIFT_0 47
#define __MMU_SHIFT_1 36
#define __MMU_SHIFT_2 25
#define __MMU_SHIFT_3 14
#elif AARCH64_MMU_GRANULE == 64
#define MMU_LEVELS    3
#define __MMU_SHIFT_0 42
#define __MMU_SHIFT_1 29
#define __MMU_SHIFT_2 16
#else
#error No granule size specified
#endif /* AARCH_MMU_GRANULE */

#define MMU_DESC_VD             (1UL << 0)  /* Descriptor is valid */
#define MMU_DESC_TP             (1UL << 1)  /* Block or Table/Page */
#define MMU_DESC_NS             (1UL << 5)  /* Non-secure bit */
#define MMU_DESC_AF             (1UL << 10) /* The Access flag */
#define MMU_DESC_PXN            (1UL << 53) /* The Execute-never bit EL1 */
#define MMU_DESC_UXN            (1UL << 54) /* The Execute-never bit EL0 */

#define MMU_DESC_ATTRINDX       /* Stage 1 memory attributes index field */
#define MMU_DESC_ATTRINDX_MASK  0b111UL
#define MMU_DESC_ATTRINDX_SHIFT 2

#define MMU_DESC_AP             /* Data Access Permissions bits */
#define MMU_DESC_AP_MASK        0b11UL
#define MMU_DESC_AP_SHIFT       6
#define MMU_DESC_AP_RW1         0b00UL /* Read-write EL1 */
#define MMU_DESC_AP_RO1         0b10UL /* Read-only EL1 */
#define MMU_DESC_AP_RW0         0b01UL /* Read-write EL0 and EL1 */
#define MMU_DESC_AP_RO0         0b11UL /* Read-only EL0 and EL1 */

typedef uintptr_t __mmu_reg_t;
typedef uintptr_t __mmu_vaddr_t;
typedef uintptr_t __mmu_paddr_t;
typedef uintptr_t __mmu_ctx_t;

#endif /* AARCH_MMU_H_ */
