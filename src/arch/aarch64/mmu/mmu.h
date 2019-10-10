/**
 * @file mmu.h
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 07.08.2019
 */

#ifndef AARCH_MMU_H_
#define AARCH_MMU_H_

#include <stdint.h>

#include <framework/mod/options.h>

typedef uintptr_t __mmu_reg_t;
typedef uintptr_t __mmu_vaddr_t;
typedef uintptr_t __mmu_paddr_t;
typedef uintptr_t __mmu_ctx_t;

#define AARCH64_MMU_TYPE_MASK  0x3
#define AARCH64_MMU_TYPE_BLOCK 0x1
#define AARCH64_MMU_TYPE_TABLE 0x3 /* Translation walk level is 0, 1 or 2 */
#define AARCH64_MMU_TYPE_PAGE  0x3 /* Translation walk level is 3 */

/* Without extensions Aarch64 VA are 48-bit long */
#define MMU_VADDR_WIDTH	48

#define AARCH64_MMU_GRANULE \
	OPTION_MODULE_GET(embox__arch__aarch64__mmu, NUMBER, granule)

#if AARCH64_MMU_GRANULE == 4
#define MMU_LEVELS 4
#define __MMU_SHIFT_0   39
#define __MMU_SHIFT_1   30
#define __MMU_SHIFT_2   21
#define __MMU_SHIFT_3   12

#elif AARCH64_MMU_GRANULE == 16
#define MMU_LEVELS 4
#define __MMU_SHIFT_0  47
#define __MMU_SHIFT_1  36
#define __MMU_SHIFT_2  25
#define __MMU_SHIFT_3  14

#elif AARCH64_MMU_GRANULE == 64
#define MMU_LEVELS 3
#define __MMU_SHIFT_0  42
#define __MMU_SHIFT_1  29
#define __MMU_SHIFT_2  16

#else
#error No granule size specified
#endif /* AARCH_MMU_GRANULE */

/* All blocks and all pages have the same low property bits */
#define AARCH64_MMU_PROPERTY_AF               0x0400 /* Access flag */
#define AARCH64_MMU_PROPERTY_AP_MASK          0x00C0 /* Access permission */
#define AARCH64_MMU_PROPERTY_AP_RW            0x0000 /* Read-write for all EL > 0 */
#define AARCH64_MMU_PROPERTY_AP_RO            0x0080 /* Read-only for all EL > 0 */
#define AARCH64_MMU_PROPERTY_MEM_ATTR_MASK    0x003C /* Properties for next level table */
#define AARCH64_MMU_PROPERTY_MEM_ATTR_DEVICE  0x003C /* Refer D5.5.3 section in armv8-a manual for more details */
#define AARCH64_MMU_PROPERTY_MEM_ATTR_NORMAL  0x0018

#define AARCH64_MMU_PROPERTY_UXN              (1LL << 53) /* Unpriveleged eXecute-Never */
#define AARCH64_MMU_PROPERTY_PXN              (1LL << 54) /* Priveleged eXecute-Never */

#include <inttypes.h>
#define __PRIxMMUREG PRIx64

#endif /* AARCH_MMU_H_ */
