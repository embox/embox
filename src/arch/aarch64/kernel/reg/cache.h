/**
 * @file
 * @brief
 *
 * @date 02.12.23
 * @author Aleksey Zhmulin
 */
#ifndef AARCH64_REG_CACHE_H_
#define AARCH64_REG_CACHE_H_

#include <stdint.h>

#define __DC(reg, val)                                          \
	({                                                          \
		register uint64_t __val = val;                          \
		__asm__ __volatile__("DC " #reg ",%x0" : : "r"(__val)); \
	})

#define __IC0(reg) __asm__ __volatile__("IC " #reg)

#define __IC1(reg, val)                                         \
	({                                                          \
		register uint64_t __val = val;                          \
		__asm__ __volatile__("IC " #reg ",%x0" : : "r"(__val)); \
	})

/** [WO] Data or unified Cache line Clean and Invalidate by Set/Way */
#define DC_CISW                           DC_CISW
#define __ARCH_REG_STORE__DC_CISW(val)    __DC(CISW, val)

/** [WO] Data or unified Cache line Clean and Invalidate by VA to PoC */
#define DC_CIVAC                          DC_CIVAC
#define __ARCH_REG_STORE__DC_CIVAC(val)   __DC(CIVAC, val)

/** [WO] Data or unified Cache line Clean by Set/Way */
#define DC_CSW                            DC_CSW
#define __ARCH_REG_STORE__DC_CSW(val)     __DC(CSW, val)

/** [WO] Data or unified Cache line Clean by VA to PoC */
#define DC_CVAC                           DC_CVAC
#define __ARCH_REG_STORE__DC_CVAC(val)    __DC(CVAC, val)

/** [WO] Data or unified Cache line Clean by VA to PoU */
#define DC_CVAU                           DC_CVAU
#define __ARCH_REG_STORE__DC_CVAU(val)    __DC(CVAU, val)

/** [WO] Data or unified Cache line Invalidate by Set/Way */
#define DC_ISW                            DC_ISW
#define __ARCH_REG_STORE__DC_ISW(val)     __DC(ISW, val)

/** [WO] Data or unified Cache line Invalidate by VA to PoC */
#define DC_IVAC                           DC_IVAC
#define __ARCH_REG_STORE__DC_IVAC(val)    __DC(IVAC, val)

/** [WO] Data Cache Zero by VA */
#define DC_ZVA                            DC_ZVA
#define __ARCH_REG_STORE__DC_ZVA(val)     __DC(ZVA, val)

/** [WO] Instruction Cache Invalidate All to PoU */
#define IC_IALLU                          IC_IALLU
#define __ARCH_REG_STORE__IC_IALLU(val)   __IC0(IALLU)

/** [WO] Instruction Cache Invalidate All to PoU, Inner Shareable */
#define IC_IALLUIS                        IC_IALLUIS
#define __ARCH_REG_STORE__IC_IALLUIS(val) __IC0(IALLUIS)

/** [WO] Instruction Cache line Invalidate by VA to PoU */
#define IC_IVAU                           IC_IVAU
#define __ARCH_REG_STORE__IC_IVAU(val)    __IC1(IVAU, val)

#endif /* AARCH64_REG_CACHE_H_ */
