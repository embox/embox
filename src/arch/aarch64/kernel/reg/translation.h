/**
 * @file
 * @brief
 *
 * @date 02.12.23
 * @author Aleksey Zhmulin
 */
#ifndef AARCH64_REG_TRANSLATION_H_
#define AARCH64_REG_TRANSLATION_H_

#include <stdint.h>

#define __AT(reg, val)                                          \
	({                                                          \
		register uint64_t __val = val;                          \
		__asm__ __volatile__("AT " #reg ",%x0" : : "r"(__val)); \
	})

/** [WO] Address Translate Stages 1 and 2 EL0 Read */
#define AT_S12E0R                        AT_S12E0R
#define __ARCH_REG_STORE__AT_S12E0R(val) __AT(S12E0R, val)

/** [WO] Address Translate Stages 1 and 2 EL0 Write */
#define AT_S12E0W                        AT_S12E0W
#define __ARCH_REG_STORE__AT_S12E0W(val) __AT(S12E0W, val)

/** [WO] Address Translate Stages 1 and 2 EL1 Read */
#define AT_S12E1R                        AT_S12E1R
#define __ARCH_REG_STORE__AT_S12E1R(val) __AT(S12E1R, val)

/** [WO] Address Translate Stages 1 and 2 EL1 Write */
#define AT_S12E1W                        AT_S12E1W
#define __ARCH_REG_STORE__AT_S12E1W(val) __AT(S12E1W, val)

/** [WO] Address Translate Stage 1 EL0 Read */
#define AT_S1E0R                         AT_S1E0R
#define __ARCH_REG_STORE__AT_S1E0R(val)  __AT(S1E0R, val)

/** [WO] Address Translate Stage 1 EL0 Write */
#define AT_S1E0W                         AT_S1E0W
#define __ARCH_REG_STORE__AT_S1E0W(val)  __AT(S1E0W, val)

/** [WO] Address Translate Stage 1 EL1 Read */
#define AT_S1E1R                         AT_S1E1R
#define __ARCH_REG_STORE__AT_S1E1R(val)  __AT(S1E1R, val)

/** [WO] Address Translate Stage 1 EL1 Write */
#define AT_S1E1W                         AT_S1E1W
#define __ARCH_REG_STORE__AT_S1E1W(val)  __AT(S1E1W, val)

/** [WO] Address Translate Stage 1 EL2 Read */
#define AT_S1E2R                         AT_S1E2R
#define __ARCH_REG_STORE__AT_S1E2R(val)  __AT(S1E2R, val)

/** [WO] Address Translate Stage 1 EL2 Write */
#define AT_S1E2W                         AT_S1E2W
#define __ARCH_REG_STORE__AT_S1E2W(val)  __AT(S1E2W, val)

/** [WO] Address Translate Stage 1 EL3 Read */
#define AT_S1E3R                         AT_S1E3R
#define __ARCH_REG_STORE__AT_S1E3R(val)  __AT(S1E3R, val)

/** [WO] Address Translate Stage 1 EL3 Write */
#define AT_S1E3W                         AT_S1E3W
#define __ARCH_REG_STORE__AT_S1E3W(val)  __AT(S1E3W, val)

#endif /* AARCH64_REG_TRANSLATION_H_ */
