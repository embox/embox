/**
 * @file
 * @brief
 *
 * @date 02.12.23
 * @author Aleksey Zhmulin
 */
#ifndef AARCH64_REG_TLB_H_
#define AARCH64_REG_TLB_H_

#include <stdint.h>

#define __TLBI0(reg) __asm__ __volatile__("TLBI " #reg)

#define __TLBI1(reg, val)                                         \
	({                                                            \
		register uint64_t __val = val;                            \
		__asm__ __volatile__("TLBI " #reg ",%x0" : : "r"(__val)); \
	})

/** [WO] TLB Invalidate All entries, EL1 */
#define TLBI_ALLE1                               TLBI_ALLE1
#define __ARCH_REG_STORE__TLBI_ALLE1(val)        __TLBI0(ALLE1)

/** [WO] TLB Invalidate All entries, EL1, Inner Shareable */
#define TLBI_ALLE1IS                             TLBI_ALLE1IS
#define __ARCH_REG_STORE__TLBI_ALLE1IS(val)      __TLBI0(ALLE1IS)

/** [WO] TLB Invalidate All entries, EL2 */
#define TLBI_ALLE2                               TLBI_ALLE2
#define __ARCH_REG_STORE__TLBI_ALLE2(val)        __TLBI0(ALLE2)

/** [WO] TLB Invalidate All entries, EL2, Inner Shareable */
#define TLBI_ALLE2IS                             TLBI_ALLE2IS
#define __ARCH_REG_STORE__TLBI_ALLE2IS(val)      __TLBI0(ALLE2IS)

/** [WO] TLB Invalidate All entries, EL3 */
#define TLBI_ALLE3                               TLBI_ALLE3
#define __ARCH_REG_STORE__TLBI_ALLE3(val)        __TLBI0(ALLE3)

/** [WO] TLB Invalidate All entries, EL3, Inner Shareable */
#define TLBI_ALLE3IS                             TLBI_ALLE3IS
#define __ARCH_REG_STORE__TLBI_ALLE3IS(val)      __TLBI0(ALLE3IS)

/** [WO] TLB Invalidate by ASID, EL1 */
#define TLBI_ASIDE1                              TLBI_ASIDE1
#define __ARCH_REG_STORE__TLBI_ASIDE1(val)       __TLBI1(ASIDE1, val)

/** [WO] TLB Invalidate by ASID, EL1, Inner Shareable */
#define TLBI_ASIDE1IS                            TLBI_ASIDE1IS
#define __ARCH_REG_STORE__TLBI_ASIDE1IS(val)     __TLBI1(ASIDE1IS, val)

/** [WO] TLB Invalidate by Intermediate Physical Address, Stage 2, EL1 */
#define TLBI_IPAS2E1                             TLBI_IPAS2E1
#define __ARCH_REG_STORE__TLBI_IPAS2E1(val)      __TLBI1(IPAS2E1, val)

/** [WO] TLB Invalidate by Intermediate Physical Address, Stage 2, EL1, Inner Shareable */
#define TLBI_IPAS2E1IS                           TLBI_IPAS2E1IS
#define __ARCH_REG_STORE__TLBI_IPAS2E1IS(val)    __TLBI1(IPAS2E1IS, val)

/** [WO] TLB Invalidate by Intermediate Physical Address, Stage 2, Last level, EL1 */
#define TLBI_IPAS2LE1                            TLBI_IPAS2LE1
#define __ARCH_REG_STORE__TLBI_IPAS2LE1(val)     __TLBI1(IPAS2LE1, val)

/** [WO] TLB Invalidate by Intermediate Physical Address, Stage 2, Last level, EL1, Inner Shareable */
#define TLBI_IPAS2LE1IS                          TLBI_IPAS2LE1IS
#define __ARCH_REG_STORE__TLBI_IPAS2LE1IS(val)   __TLBI1(IPAS2LE1IS, val)

/** [WO] TLB Invalidate by VA, All ASID, EL1 */
#define TLBI_VAAE1                               TLBI_VAAE1
#define __ARCH_REG_STORE__TLBI_VAAE1(val)        __TLBI1(VAAE1, val)

/** [WO] TLB Invalidate by VA, All ASID, EL1, Inner Shareable */
#define TLBI_VAAE1IS                             TLBI_VAAE1IS
#define __ARCH_REG_STORE__TLBI_VAAE1IS(val)      __TLBI1(VAAE1IS, val)

/** [WO] TLB Invalidate by VA, All ASID, Last level, EL1 */
#define TLBI_VAALE1                              TLBI_VAALE1
#define __ARCH_REG_STORE__TLBI_VAALE1(val)       __TLBI1(VAALE1, val)

/** [WO] TLB Invalidate by VA, All ASID, EL1, Inner Shareable */
#define TLBI_VAALE1IS                            TLBI_VAALE1IS
#define __ARCH_REG_STORE__TLBI_VAALE1IS(val)     __TLBI1(VAALE1IS, val)

/** [WO] TLB Invalidate by VA, EL1 */
#define TLBI_VAE1                                TLBI_VAE1
#define __ARCH_REG_STORE__TLBI_VAE1(val)         __TLBI1(VAE1, val)

/** [WO] TLB Invalidate by VA, EL1, Inner Shareable */
#define TLBI_VAE1IS                              TLBI_VAE1IS
#define __ARCH_REG_STORE__TLBI_VAE1IS(val)       __TLBI1(VAE1IS, val)

/** [WO] TLB Invalidate by VA, EL2 */
#define TLBI_VAE2                                TLBI_VAE2
#define __ARCH_REG_STORE__TLBI_VAE2(val)         __TLBI1(VAE2, val)

/** [WO] TLB Invalidate by VA, EL2, Inner Shareable */
#define TLBI_VAE2IS                              TLBI_VAE2IS
#define __ARCH_REG_STORE__TLBI_VAE2IS(val)       __TLBI1(VAE2IS, val)

/** [WO] TLB Invalidate by VA, EL3 */
#define TLBI_VAE3                                TLBI_VAE3
#define __ARCH_REG_STORE__TLBI_VAE3(val)         __TLBI1(VAE3, val)

/** [WO] TLB Invalidate by VA, EL3, Inner Shareable */
#define TLBI_VAE3IS                              TLBI_VAE3IS
#define __ARCH_REG_STORE__TLBI_VAE3IS(val)       __TLBI1(VAE3IS, val)

/** [WO] TLB Invalidate by VA, Last level, EL1 */
#define TLBI_VALE1                               TLBI_VALE1
#define __ARCH_REG_STORE__TLBI_VALE1(val)        __TLBI1(VALE1, val)

/** [WO] TLB Invalidate by VA, Last level, EL1, Inner Shareable */
#define TLBI_VALE1IS                             TLBI_VALE1IS
#define __ARCH_REG_STORE__TLBI_VALE1IS(val)      __TLBI1(VALE1IS, val)

/** [WO] TLB Invalidate by VA, Last level, EL2 */
#define TLBI_VALE2                               TLBI_VALE2
#define __ARCH_REG_STORE__TLBI_VALE2(val)        __TLBI1(VALE2, val)

/** [WO] TLB Invalidate by VA, Last level, EL2, Inner Shareable */
#define TLBI_VALE2IS                             TLBI_VALE2IS
#define __ARCH_REG_STORE__TLBI_VALE2IS(val)      __TLBI1(VALE2IS, val)

/** [WO] TLB Invalidate by VA, Last level, EL3 */
#define TLBI_VALE3                               TLBI_VALE3
#define __ARCH_REG_STORE__TLBI_VALE3(val)        __TLBI1(VALE3, val)

/** [WO] TLB Invalidate by VA, Last level, EL3, Inner Shareable */
#define TLBI_VALE3IS                             TLBI_VALE3IS
#define __ARCH_REG_STORE__TLBI_VALE3IS(val)      __TLBI1(VALE3IS, val)

/** [WO] TLB Invalidate by VMID, All entries at stage 1, EL1 */
#define TLBI_VMALLE1                             TLBI_VMALLE1
#define __ARCH_REG_STORE__TLBI_VMALLE1(val)      __TLBI0(VMALLE1)

/** [WO] TLB Invalidate by VMID, All entries at stage 1, EL1, Inner Shareable */
#define TLBI_VMALLE1IS                           TLBI_VMALLE1IS
#define __ARCH_REG_STORE__TLBI_VMALLE1IS(val)    __TLBI0(VMALLE1IS)

/** [WO] TLB Invalidate by VMID, All entries at Stage 1 and 2, EL1 */
#define TLBI_VMALLS12E1                          TLBI_VMALLS12E1
#define __ARCH_REG_STORE__TLBI_VMALLS12E1(val)   __TLBI0(VMALLS12E1)

/** [WO] TLB Invalidate by VMID, All entries at Stage 1 and 2, EL1, Inner Shareable */
#define TLBI_VMALLS12E1IS                        TLBI_VMALLS12E1IS
#define __ARCH_REG_STORE__TLBI_VMALLS12E1IS(val) __TLBI0(VMALLS12E1IS)

#endif /* AARCH64_REG_TLB_H_ */
