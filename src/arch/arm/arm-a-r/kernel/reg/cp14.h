/**
 * @file
 * @brief
 *
 * @date 16.06.23
 * @author Aleksey Zhmulin
 */
#ifndef ARMLIB_REG_CP14_H_
#define ARMLIB_REG_CP14_H_

#include <stdint.h>

#define __MRC14(op1, crn, crm, op2)                                        \
	({                                                                     \
		register uint32_t __val;                                           \
		__asm__ __volatile__("mrc p14," #op1 ",%0," #crn "," #crm "," #op2 \
		                     : "=r"(__val));                               \
		__val;                                                             \
	})

#define __MCR14(op1, crn, crm, op2, val)                                   \
	({                                                                     \
		register uint32_t __val = val;                                     \
		__asm__ __volatile__("mcr p14," #op1 ",%0," #crn "," #crm "," #op2 \
		                     :                                             \
		                     : "r"(__val));                                \
	})

/** [RO] Debug ID Register */
#define DBGDIDR                            DBGDIDR
#define __ARCH_REG_LOAD__DBGDIDR()         __MRC14(0, c0, c0, 0)

/** [RO] Debug Status and Control Register (internal view) */
#define DBGDSCRint                         DBGDSCRint
#define __ARCH_REG_LOAD__DBGDSCRint()      __MRC14(0, c0, c1, 0)

/** [RO] Host to Target Data Transfer Register (internal view) */
#define DBGDTRRXint                        DBGDTRRXint
#define __ARCH_REG_LOAD__DBGDTRRXint()     __MRC14(0, c0, c5, 0)

/* [WO] Target to Host Data Transfer Register (internal view) */
#define DBGDTRTXint                        DBGDTRTXint
#define __ARCH_REG_STORE__DBGDTRTXint(val) __MCR14(0, c0, c5, 0, val)

/** Watchpoint Fault Address Register */
#define DBGWFAR                            DBGWFAR
#define __ARCH_REG_LOAD__DBGWFAR()         __MRC14(0, c0, c6, 0)
#define __ARCH_REG_STORE__DBGWFAR(val)     __MCR14(0, c0, c6, 0, val)

/** Vector Catch Register */
#define DBGVCR                             DBGVCR
#define __ARCH_REG_LOAD__DBGVCR()          __MRC14(0, c0, c7, 0)
#define __ARCH_REG_STORE__DBGVCR(val)      __MCR14(0, c0, c7, 0, val)

/** [DBGv7] Event Catch Register */
#define DBGECR                             DBGECR
#define __ARCH_REG_LOAD__DBGECR()          __MRC14(0, c0, c9, 0)
#define __ARCH_REG_STORE__DBGECR(val)      __MCR14(0, c0, c9, 0, val)

/** [DBGv7] Debug State Cache Control Register */
#define DBGDSCCR                           DBGDSCCR
#define __ARCH_REG_LOAD__DBGDSCCR()        __MRC14(0, c0, c10, 0)
#define __ARCH_REG_STORE__DBGDSCCR(val)    __MCR14(0, c0, c10, 0, val)

/** [DBGv7] Debug State MMU Control Register */
#define DBGDSMCR                           DBGDSMCR
#define __ARCH_REG_LOAD__DBGDSMCR()        __MRC14(0, c0, c11, 0)
#define __ARCH_REG_STORE__DBGDSMCR(val)    __MCR14(0, c0, c11, 0, val)

/** Host to Target Data Transfer Register (external view) */
#define DBGDTRRXext                        DBGDTRRXext
#define __ARCH_REG_LOAD__DBGDTRRXext()     __MRC14(0, c0, c0, 2)
#define __ARCH_REG_STORE__DBGDTRRXext(val) __MCR14(0, c0, c0, 2, val)

/** Debug Status and Control Register (external view) */
#define DBGDSCRext                         DBGDSCRext
#define __ARCH_REG_LOAD__DBGDSCRext()      __MRC14(0, c0, c2, 2)
#define __ARCH_REG_STORE__DBGDSCRext(val)  __MCR14(0, c0, c2, 2, val)

#define DBGDSCR_ITREN                      (0b1U << 13)
#define DBGDSCR_MDBGEN                     (0b1U << 15)
#define DBGDSCR_EXTDCCMODE_MASK            (0b11U << 20)
#define DBGDSCR_EXTDCCMODE_STALL           (0b1U << 20)

/** Target to Host Data Transfer Register (external view) */
#define DBGDTRTXext                        DBGDTRTXext
#define __ARCH_REG_LOAD__DBGDTRTXext()     __MRC14(0, c0, c3, 2)
#define __ARCH_REG_STORE__DBGDTRTXext(val) __MCR14(0, c0, c3, 2, val)

/** [DBGv7] Debug Run Control Register */
#define DBGDRCR                            DBGDRCR
#define __ARCH_REG_LOAD__DBGDRCR()         __MRC14(0, c0, c4, 2)
#define __ARCH_REG_STORE__DBGDRCR(val)     __MCR14(0, c0, c4, 2, val)

/** Breakpoint Value Register 0 */
#define DBGBVR0                            DBGBVR0
#define __ARCH_REG_LOAD__DBGBVR0()         __MRC14(0, c0, c0, 4)
#define __ARCH_REG_STORE__DBGBVR0(val)     __MCR14(0, c0, c0, 4, val)

/** Breakpoint Value Register 1 */
#define DBGBVR1                            DBGBVR1
#define __ARCH_REG_LOAD__DBGBVR1()         __MRC14(0, c0, c1, 4)
#define __ARCH_REG_STORE__DBGBVR1(val)     __MCR14(0, c0, c1, 4, val)

/** Breakpoint Value Register 2 */
#define DBGBVR2                            DBGBVR2
#define __ARCH_REG_LOAD__DBGBVR2()         __MRC14(0, c0, c2, 4)
#define __ARCH_REG_STORE__DBGBVR2(val)     __MCR14(0, c0, c2, 4, val)

/** Breakpoint Value Register 3 */
#define DBGBVR3                            DBGBVR3
#define __ARCH_REG_LOAD__DBGBVR3()         __MRC14(0, c0, c3, 4)
#define __ARCH_REG_STORE__DBGBVR3(val)     __MCR14(0, c0, c3, 4, val)

/** Breakpoint Value Register 4 */
#define DBGBVR4                            DBGBVR4
#define __ARCH_REG_LOAD__DBGBVR4()         __MRC14(0, c0, c4, 4)
#define __ARCH_REG_STORE__DBGBVR4(val)     __MCR14(0, c0, c4, 4, val)

/** Breakpoint Value Register 5 */
#define DBGBVR5                            DBGBVR5
#define __ARCH_REG_LOAD__DBGBVR5()         __MRC14(0, c0, c5, 4)
#define __ARCH_REG_STORE__DBGBVR5(val)     __MCR14(0, c0, c5, 4, val)

/** Breakpoint Value Register 6 */
#define DBGBVR6                            DBGBVR6
#define __ARCH_REG_LOAD__DBGBVR6()         __MRC14(0, c0, c6, 4)
#define __ARCH_REG_STORE__DBGBVR6(val)     __MCR14(0, c0, c6, 4, val)

/** Breakpoint Value Register 7 */
#define DBGBVR7                            DBGBVR7
#define __ARCH_REG_LOAD__DBGBVR7()         __MRC14(0, c0, c7, 4)
#define __ARCH_REG_STORE__DBGBVR7(val)     __MCR14(0, c0, c7, 4, val)

/** Breakpoint Value Register 8 */
#define DBGBVR8                            DBGBVR8
#define __ARCH_REG_LOAD__DBGBVR8()         __MRC14(0, c0, c8, 4)
#define __ARCH_REG_STORE__DBGBVR8(val)     __MCR14(0, c0, c8, 4, val)

/** Breakpoint Value Register 9 */
#define DBGBVR9                            DBGBVR9
#define __ARCH_REG_LOAD__DBGBVR9()         __MRC14(0, c0, c9, 4)
#define __ARCH_REG_STORE__DBGBVR9(val)     __MCR14(0, c0, c9, 4, val)

/** Breakpoint Value Register 10 */
#define DBGBVR10                           DBGBVR10
#define __ARCH_REG_LOAD__DBGBVR10()        __MRC14(0, c0, c10, 4)
#define __ARCH_REG_STORE__DBGBVR10(val)    __MCR14(0, c0, c10, 4, val)

/** Breakpoint Value Register 11 */
#define DBGBVR11                           DBGBVR11
#define __ARCH_REG_LOAD__DBGBVR11()        __MRC14(0, c0, c11, 4)
#define __ARCH_REG_STORE__DBGBVR11(val)    __MCR14(0, c0, c11, 4, val)

/** Breakpoint Value Register 12 */
#define DBGBVR12                           DBGBVR12
#define __ARCH_REG_LOAD__DBGBVR12()        __MRC14(0, c0, c12, 4)
#define __ARCH_REG_STORE__DBGBVR12(val)    __MCR14(0, c0, c12, 4, val)

/** Breakpoint Value Register 13 */
#define DBGBVR13                           DBGBVR13
#define __ARCH_REG_LOAD__DBGBVR13()        __MRC14(0, c0, c13, 4)
#define __ARCH_REG_STORE__DBGBVR13(val)    __MCR14(0, c0, c13, 4, val)

/** Breakpoint Value Register 14 */
#define DBGBVR14                           DBGBVR14
#define __ARCH_REG_LOAD__DBGBVR14()        __MRC14(0, c0, c14, 4)
#define __ARCH_REG_STORE__DBGBVR14(val)    __MCR14(0, c0, c14, 4, val)

/** Breakpoint Value Register 15 */
#define DBGBVR15                           DBGBVR15
#define __ARCH_REG_LOAD__DBGBVR15()        __MRC14(0, c0, c15, 4)
#define __ARCH_REG_STORE__DBGBVR15(val)    __MCR14(0, c0, c15, 4, val)

/** Breakpoint Control Register 0 */
#define DBGBCR0                            DBGBCR0
#define __ARCH_REG_LOAD__DBGBCR0()         __MRC14(0, c0, c0, 5)
#define __ARCH_REG_STORE__DBGBCR0(val)     __MCR14(0, c0, c0, 5, val)

/** Breakpoint Control Register 1 */
#define DBGBCR1                            DBGBCR1
#define __ARCH_REG_LOAD__DBGBCR1()         __MRC14(0, c0, c1, 5)
#define __ARCH_REG_STORE__DBGBCR1(val)     __MCR14(0, c0, c1, 5, val)

/** Breakpoint Control Register 2 */
#define DBGBCR2                            DBGBCR2
#define __ARCH_REG_LOAD__DBGBCR2()         __MRC14(0, c0, c2, 5)
#define __ARCH_REG_STORE__DBGBCR2(val)     __MCR14(0, c0, c2, 5, val)

/** Breakpoint Control Register 3 */
#define DBGBCR3                            DBGBCR3
#define __ARCH_REG_LOAD__DBGBCR3()         __MRC14(0, c0, c3, 5)
#define __ARCH_REG_STORE__DBGBCR3(val)     __MCR14(0, c0, c3, 5, val)

/** Breakpoint Control Register 4 */
#define DBGBCR4                            DBGBCR4
#define __ARCH_REG_LOAD__DBGBCR4()         __MRC14(0, c0, c4, 5)
#define __ARCH_REG_STORE__DBGBCR4(val)     __MCR14(0, c0, c4, 5, val)

/** Breakpoint Control Register 5 */
#define DBGBCR5                            DBGBCR5
#define __ARCH_REG_LOAD__DBGBCR5()         __MRC14(0, c0, c5, 5)
#define __ARCH_REG_STORE__DBGBCR5(val)     __MCR14(0, c0, c5, 5, val)

/** Breakpoint Control Register 6 */
#define DBGBCR6                            DBGBCR6
#define __ARCH_REG_LOAD__DBGBCR6()         __MRC14(0, c0, c6, 5)
#define __ARCH_REG_STORE__DBGBCR6(val)     __MCR14(0, c0, c6, 5, val)

/** Breakpoint Control Register 7 */
#define DBGBCR7                            DBGBCR7
#define __ARCH_REG_LOAD__DBGBCR7()         __MRC14(0, c0, c7, 5)
#define __ARCH_REG_STORE__DBGBCR7(val)     __MCR14(0, c0, c7, 5, val)

/** Breakpoint Control Register 8 */
#define DBGBCR8                            DBGBCR8
#define __ARCH_REG_LOAD__DBGBCR8()         __MRC14(0, c0, c8, 5)
#define __ARCH_REG_STORE__DBGBCR8(val)     __MCR14(0, c0, c8, 5, val)

/** Breakpoint Control Register 9 */
#define DBGBCR9                            DBGBCR9
#define __ARCH_REG_LOAD__DBGBCR9()         __MRC14(0, c0, c9, 5)
#define __ARCH_REG_STORE__DBGBCR9(val)     __MCR14(0, c0, c9, 5, val)

/** Breakpoint Control Register 10 */
#define DBGBCR10                           DBGBCR10
#define __ARCH_REG_LOAD__DBGBCR10()        __MRC14(0, c0, c10, 5)
#define __ARCH_REG_STORE__DBGBCR10(val)    __MCR14(0, c0, c10, 5, val)

/** Breakpoint Control Register 11 */
#define DBGBCR11                           DBGBCR11
#define __ARCH_REG_LOAD__DBGBCR11()        __MRC14(0, c0, c11, 5)
#define __ARCH_REG_STORE__DBGBCR11(val)    __MCR14(0, c0, c11, 5, val)

/** Breakpoint Control Register 12 */
#define DBGBCR12                           DBGBCR12
#define __ARCH_REG_LOAD__DBGBCR12()        __MRC14(0, c0, c12, 5)
#define __ARCH_REG_STORE__DBGBCR12(val)    __MCR14(0, c0, c12, 5, val)

/** Breakpoint Control Register 13 */
#define DBGBCR13                           DBGBCR13
#define __ARCH_REG_LOAD__DBGBCR13()        __MRC14(0, c0, c13, 5)
#define __ARCH_REG_STORE__DBGBCR13(val)    __MCR14(0, c0, c13, 5, val)

/** Breakpoint Control Register 14 */
#define DBGBCR14                           DBGBCR14
#define __ARCH_REG_LOAD__DBGBCR14()        __MRC14(0, c0, c14, 5)
#define __ARCH_REG_STORE__DBGBCR14(val)    __MCR14(0, c0, c14, 5, val)

/** Breakpoint Control Register 15 */
#define DBGBCR15                           DBGBCR15
#define __ARCH_REG_LOAD__DBGBCR15()        __MRC14(0, c0, c15, 5)
#define __ARCH_REG_STORE__DBGBCR15(val)    __MCR14(0, c0, c15, 5, val)

#define DBGBCR_EN                          (0b1U << 0)
#define DBGBCR_PMC_ANY                     (0b11U << 1)
#define DBGBCR_BAS_ANY                     (0b1111U << 5)

/** Watchpoint Value Register 0 */
#define DBGWVR0                            DBGWVR0
#define __ARCH_REG_LOAD__DBGWVR0()         __MRC14(0, c0, c0, 6)
#define __ARCH_REG_STORE__DBGWVR0(val)     __MCR14(0, c0, c0, 6, val)

/** Watchpoint Value Register 1 */
#define DBGWVR1                            DBGWVR1
#define __ARCH_REG_LOAD__DBGWVR1()         __MRC14(0, c0, c1, 6)
#define __ARCH_REG_STORE__DBGWVR1(val)     __MCR14(0, c0, c1, 6, val)

/** Watchpoint Value Register 2 */
#define DBGWVR2                            DBGWVR2
#define __ARCH_REG_LOAD__DBGWVR2()         __MRC14(0, c0, c2, 6)
#define __ARCH_REG_STORE__DBGWVR2(val)     __MCR14(0, c0, c2, 6, val)

/** Watchpoint Value Register 3 */
#define DBGWVR3                            DBGWVR3
#define __ARCH_REG_LOAD__DBGWVR3()         __MRC14(0, c0, c3, 6)
#define __ARCH_REG_STORE__DBGWVR3(val)     __MCR14(0, c0, c3, 6, val)

/** Watchpoint Value Register 4 */
#define DBGWVR4                            DBGWVR4
#define __ARCH_REG_LOAD__DBGWVR4()         __MRC14(0, c0, c4, 6)
#define __ARCH_REG_STORE__DBGWVR4(val)     __MCR14(0, c0, c4, 6, val)

/** Watchpoint Value Register 5 */
#define DBGWVR5                            DBGWVR5
#define __ARCH_REG_LOAD__DBGWVR5()         __MRC14(0, c0, c5, 6)
#define __ARCH_REG_STORE__DBGWVR5(val)     __MCR14(0, c0, c5, 6, val)

/** Watchpoint Value Register 6 */
#define DBGWVR6                            DBGWVR6
#define __ARCH_REG_LOAD__DBGWVR6()         __MRC14(0, c0, c6, 6)
#define __ARCH_REG_STORE__DBGWVR6(val)     __MCR14(0, c0, c6, 6, val)

/** Watchpoint Value Register 7 */
#define DBGWVR7                            DBGWVR7
#define __ARCH_REG_LOAD__DBGWVR7()         __MRC14(0, c0, c7, 6)
#define __ARCH_REG_STORE__DBGWVR7(val)     __MCR14(0, c0, c7, 6, val)

/** Watchpoint Value Register 8 */
#define DBGWVR8                            DBGWVR8
#define __ARCH_REG_LOAD__DBGWVR8()         __MRC14(0, c0, c8, 6)
#define __ARCH_REG_STORE__DBGWVR8(val)     __MCR14(0, c0, c8, 6, val)

/** Watchpoint Value Register 9 */
#define DBGWVR9                            DBGWVR9
#define __ARCH_REG_LOAD__DBGWVR9()         __MRC14(0, c0, c9, 6)
#define __ARCH_REG_STORE__DBGWVR9(val)     __MCR14(0, c0, c9, 6, val)

/** Watchpoint Value Register 10 */
#define DBGWVR10                           DBGWVR10
#define __ARCH_REG_LOAD__DBGWVR10()        __MRC14(0, c0, c10, 6)
#define __ARCH_REG_STORE__DBGWVR10(val)    __MCR14(0, c0, c10, 6, val)

/** Watchpoint Value Register 11 */
#define DBGWVR11                           DBGWVR11
#define __ARCH_REG_LOAD__DBGWVR11()        __MRC14(0, c0, c11, 6)
#define __ARCH_REG_STORE__DBGWVR11(val)    __MCR14(0, c0, c11, 6, val)

/** Watchpoint Value Register 12 */
#define DBGWVR12                           DBGWVR12
#define __ARCH_REG_LOAD__DBGWVR12()        __MRC14(0, c0, c12, 6)
#define __ARCH_REG_STORE__DBGWVR12(val)    __MCR14(0, c0, c12, 6, val)

/** Watchpoint Value Register 13 */
#define DBGWVR13                           DBGWVR13
#define __ARCH_REG_LOAD__DBGWVR13()        __MRC14(0, c0, c13, 6)
#define __ARCH_REG_STORE__DBGWVR13(val)    __MCR14(0, c0, c13, 6, val)

/** Watchpoint Value Register 14 */
#define DBGWVR14                           DBGWVR14
#define __ARCH_REG_LOAD__DBGWVR14()        __MRC14(0, c0, c14, 6)
#define __ARCH_REG_STORE__DBGWVR14(val)    __MCR14(0, c0, c14, 6, val)

/** Watchpoint Value Register 15 */
#define DBGWVR15                           DBGWVR15
#define __ARCH_REG_LOAD__DBGWVR15()        __MRC14(0, c0, c15, 6)
#define __ARCH_REG_STORE__DBGWVR15(val)    __MCR14(0, c0, c15, 6, val)

/** Watchpoint Control Register 0 */
#define DBGWCR0                            DBGWCR0
#define __ARCH_REG_LOAD__DBGWCR0()         __MRC14(0, c0, c0, 7)
#define __ARCH_REG_STORE__DBGWCR0(val)     __MCR14(0, c0, c0, 7, val)

/** Watchpoint Control Register 1 */
#define DBGWCR1                            DBGWCR1
#define __ARCH_REG_LOAD__DBGWCR1()         __MRC14(0, c0, c1, 7)
#define __ARCH_REG_STORE__DBGWCR1(val)     __MCR14(0, c0, c1, 7, val)

/** Watchpoint Control Register 2 */
#define DBGWCR2                            DBGWCR2
#define __ARCH_REG_LOAD__DBGWCR2()         __MRC14(0, c0, c2, 7)
#define __ARCH_REG_STORE__DBGWCR2(val)     __MCR14(0, c0, c2, 7, val)

/** Watchpoint Control Register 3 */
#define DBGWCR3                            DBGWCR3
#define __ARCH_REG_LOAD__DBGWCR3()         __MRC14(0, c0, c3, 7)
#define __ARCH_REG_STORE__DBGWCR3(val)     __MCR14(0, c0, c3, 7, val)

/** Watchpoint Control Register 4 */
#define DBGWCR4                            DBGWCR4
#define __ARCH_REG_LOAD__DBGWCR4()         __MRC14(0, c0, c4, 7)
#define __ARCH_REG_STORE__DBGWCR4(val)     __MCR14(0, c0, c4, 7, val)

/** Watchpoint Control Register 5 */
#define DBGWCR5                            DBGWCR5
#define __ARCH_REG_LOAD__DBGWCR5()         __MRC14(0, c0, c5, 7)
#define __ARCH_REG_STORE__DBGWCR5(val)     __MCR14(0, c0, c5, 7, val)

/** Watchpoint Control Register 6 */
#define DBGWCR6                            DBGWCR6
#define __ARCH_REG_LOAD__DBGWCR6()         __MRC14(0, c0, c6, 7)
#define __ARCH_REG_STORE__DBGWCR6(val)     __MCR14(0, c0, c6, 7, val)

/** Watchpoint Control Register 7 */
#define DBGWCR7                            DBGWCR7
#define __ARCH_REG_LOAD__DBGWCR7()         __MRC14(0, c0, c7, 7)
#define __ARCH_REG_STORE__DBGWCR7(val)     __MCR14(0, c0, c7, 7, val)

/** Watchpoint Control Register 8 */
#define DBGWCR8                            DBGWCR8
#define __ARCH_REG_LOAD__DBGWCR8()         __MRC14(0, c0, c8, 7)
#define __ARCH_REG_STORE__DBGWCR8(val)     __MCR14(0, c0, c8, 7, val)

/** Watchpoint Control Register 9 */
#define DBGWCR9                            DBGWCR9
#define __ARCH_REG_LOAD__DBGWCR9()         __MRC14(0, c0, c9, 7)
#define __ARCH_REG_STORE__DBGWCR9(val)     __MCR14(0, c0, c9, 7, val)

/** Watchpoint Control Register 10 */
#define DBGWCR10                           DBGWCR10
#define __ARCH_REG_LOAD__DBGWCR10()        __MRC14(0, c0, c10, 7)
#define __ARCH_REG_STORE__DBGWCR10(val)    __MCR14(0, c0, c10, 7, val)

/** Watchpoint Control Register 11 */
#define DBGWCR11                           DBGWCR11
#define __ARCH_REG_LOAD__DBGWCR11()        __MRC14(0, c0, c11, 7)
#define __ARCH_REG_STORE__DBGWCR11(val)    __MCR14(0, c0, c11, 7, val)

/** Watchpoint Control Register 12 */
#define DBGWCR12                           DBGWCR12
#define __ARCH_REG_LOAD__DBGWCR12()        __MRC14(0, c0, c12, 7)
#define __ARCH_REG_STORE__DBGWCR12(val)    __MCR14(0, c0, c12, 7, val)

/** Watchpoint Control Register 13 */
#define DBGWCR13                           DBGWCR13
#define __ARCH_REG_LOAD__DBGWCR13()        __MRC14(0, c0, c13, 7)
#define __ARCH_REG_STORE__DBGWCR13(val)    __MCR14(0, c0, c13, 7, val)

/** Watchpoint Control Register 14 */
#define DBGWCR14                           DBGWCR14
#define __ARCH_REG_LOAD__DBGWCR14()        __MRC14(0, c0, c14, 7)
#define __ARCH_REG_STORE__DBGWCR14(val)    __MCR14(0, c0, c14, 7, val)

/** Watchpoint Control Register 15 */
#define DBGWCR15                           DBGWCR15
#define __ARCH_REG_LOAD__DBGWCR15()        __MRC14(0, c0, c15, 7)
#define __ARCH_REG_STORE__DBGWCR15(val)    __MCR14(0, c0, c15, 7, val)

/** [RO] Debug ROM Address Register */
#define DBGDRAR                            DBGDRAR
#define __ARCH_REG_LOAD__DBGDRAR()         __MRC14(0, c1, c0, 0)

/** [DBGv7.1] Breakpoint Extended Value Register 0 */
#define DBGBXVR0                           DBGBXVR0
#define __ARCH_REG_LOAD__DBGBXVR0()        __MRC14(0, c1, c0, 1)
#define __ARCH_REG_STORE__DBGBXVR0(val)    __MCR14(0, c1, c0, 1, val)

/** [DBGv7.1] Breakpoint Extended Value Register 1 */
#define DBGBXVR1                           DBGBXVR1
#define __ARCH_REG_LOAD__DBGBXVR1()        __MRC14(0, c1, c1, 1)
#define __ARCH_REG_STORE__DBGBXVR1(val)    __MCR14(0, c1, c1, 1, val)

/** [DBGv7.1] Breakpoint Extended Value Register 2 */
#define DBGBXVR2                           DBGBXVR2
#define __ARCH_REG_LOAD__DBGBXVR2()        __MRC14(0, c1, c2, 1)
#define __ARCH_REG_STORE__DBGBXVR2(val)    __MCR14(0, c1, c2, 1, val)

/** [DBGv7.1] Breakpoint Extended Value Register 3 */
#define DBGBXVR3                           DBGBXVR3
#define __ARCH_REG_LOAD__DBGBXVR3()        __MRC14(0, c1, c3, 1)
#define __ARCH_REG_STORE__DBGBXVR3(val)    __MCR14(0, c1, c3, 1, val)

/** [DBGv7.1] Breakpoint Extended Value Register 4 */
#define DBGBXVR4                           DBGBXVR4
#define __ARCH_REG_LOAD__DBGBXVR4()        __MRC14(0, c1, c4, 1)
#define __ARCH_REG_STORE__DBGBXVR4(val)    __MCR14(0, c1, c4, 1, val)

/** [DBGv7.1] Breakpoint Extended Value Register 5 */
#define DBGBXVR5                           DBGBXVR5
#define __ARCH_REG_LOAD__DBGBXVR5()        __MRC14(0, c1, c5, 1)
#define __ARCH_REG_STORE__DBGBXVR5(val)    __MCR14(0, c1, c5, 1, val)

/** [DBGv7.1] Breakpoint Extended Value Register 6 */
#define DBGBXVR6                           DBGBXVR6
#define __ARCH_REG_LOAD__DBGBXVR6()        __MRC14(0, c1, c6, 1)
#define __ARCH_REG_STORE__DBGBXVR6(val)    __MCR14(0, c1, c6, 1, val)

/** [DBGv7.1] Breakpoint Extended Value Register 7 */
#define DBGBXVR7                           DBGBXVR7
#define __ARCH_REG_LOAD__DBGBXVR7()        __MRC14(0, c1, c7, 1)
#define __ARCH_REG_STORE__DBGBXVR7(val)    __MCR14(0, c1, c7, 1, val)

/** [DBGv7.1] Breakpoint Extended Value Register 8 */
#define DBGBXVR8                           DBGBXVR8
#define __ARCH_REG_LOAD__DBGBXVR8()        __MRC14(0, c1, c8, 1)
#define __ARCH_REG_STORE__DBGBXVR8(val)    __MCR14(0, c1, c8, 1, val)

/** [DBGv7.1] Breakpoint Extended Value Register 9 */
#define DBGBXVR9                           DBGBXVR9
#define __ARCH_REG_LOAD__DBGBXVR9()        __MRC14(0, c1, c9, 1)
#define __ARCH_REG_STORE__DBGBXVR9(val)    __MCR14(0, c1, c9, 1, val)

/** [DBGv7.1] Breakpoint Extended Value Register 10 */
#define DBGBXVR10                          DBGBXVR10
#define __ARCH_REG_LOAD__DBGBXVR10()       __MRC14(0, c1, c10, 1)
#define __ARCH_REG_STORE__DBGBXVR10(val)   __MCR14(0, c1, c10, 1, val)

/** [DBGv7.1] Breakpoint Extended Value Register 11 */
#define DBGBXVR11                          DBGBXVR11
#define __ARCH_REG_LOAD__DBGBXVR11()       __MRC14(0, c1, c11, 1)
#define __ARCH_REG_STORE__DBGBXVR11(val)   __MCR14(0, c1, c11, 1, val)

/** [DBGv7.1] Breakpoint Extended Value Register 12 */
#define DBGBXVR12                          DBGBXVR12
#define __ARCH_REG_LOAD__DBGBXVR12()       __MRC14(0, c1, c12, 1)
#define __ARCH_REG_STORE__DBGBXVR12(val)   __MCR14(0, c1, c12, 1, val)

/** [DBGv7.1] Breakpoint Extended Value Register 13 */
#define DBGBXVR13                          DBGBXVR13
#define __ARCH_REG_LOAD__DBGBXVR13()       __MRC14(0, c1, c13, 1)
#define __ARCH_REG_STORE__DBGBXVR13(val)   __MCR14(0, c1, c13, 1, val)

/** [DBGv7.1] Breakpoint Extended Value Register 14 */
#define DBGBXVR14                          DBGBXVR14
#define __ARCH_REG_LOAD__DBGBXVR14()       __MRC14(0, c1, c14, 1)
#define __ARCH_REG_STORE__DBGBXVR14(val)   __MCR14(0, c1, c14, 1, val)

/** [DBGv7.1] Breakpoint Extended Value Register 15 */
#define DBGBXVR15                          DBGBXVR15
#define __ARCH_REG_LOAD__DBGBXVR15()       __MRC14(0, c1, c15, 1)
#define __ARCH_REG_STORE__DBGBXVR15(val)   __MCR14(0, c1, c15, 1, val)

/* [WO] OS Lock Access Register */
#define DBGOSLAR                           DBGOSLAR
#define __ARCH_REG_STORE__DBGOSLAR(val)    __MCR14(0, c1, c0, 4, val)

/** [RO] OS Lock Status Register */
#define DBGOSLSR                           DBGOSLSR
#define __ARCH_REG_LOAD__DBGOSLSR()        __MRC14(0, c1, c1, 4)

/** [RO] OS Save and Restore Register */
#define DBGOSSRR                           DBGOSSRR
#define __ARCH_REG_LOAD__DBGOSSRR()        __MRC14(0, c1, c2, 4)
#define __ARCH_REG_STORE__DBGOSSRR(val)    __MCR14(0, c1, c2, 4, val)

/** [DBGv7.1] OS Double Lock Register */
#define DBGOSDLR                           DBGOSDLR
#define __ARCH_REG_LOAD__DBGOSDLR()        __MRC14(0, c1, c3, 4)
#define __ARCH_REG_STORE__DBGOSDLR(val)    __MCR14(0, c1, c3, 4, val)

/** Device Powerdown and Reset Control Register */
#define DBGPRCR                            DBGPRCR
#define __ARCH_REG_LOAD__DBGPRCR()         __MRC14(0, c1, c4, 4)
#define __ARCH_REG_STORE__DBGPRCR(val)     __MCR14(0, c1, c4, 4, val)

/** [RO] Device Powerdown and Reset Status Register */
#define DBGPRSR                            DBGPRSR
#define __ARCH_REG_LOAD__DBGPRSR()         __MRC14(0, c1, c5, 4)

/** [RO] Debug Self Address Offset Register */
#define DBGDSAR                            DBGDSAR
#define __ARCH_REG_LOAD__DBGDSAR()         __MRC14(0, c2, c0, 0)

/** Integration Mode Control Register */
#define DBGITCTRL                          DBGITCTRL
#define __ARCH_REG_LOAD__DBGITCTRL()       __MRC14(0, c7, c0, 4)
#define __ARCH_REG_STORE__DBGITCTRL(val)   __MCR14(0, c7, c0, 4, val)

/** Claim Tag Set Register */
#define DBGCLAIMSET                        DBGCLAIMSET
#define __ARCH_REG_LOAD__DBGCLAIMSET()     __MRC14(0, c7, c8, 6)
#define __ARCH_REG_STORE__DBGCLAIMSET(val) __MCR14(0, c7, c8, 6, val)

/** Claim Tag Clear Register */
#define DBGCLAIMCLR                        DBGCLAIMCLR
#define __ARCH_REG_LOAD__DBGCLAIMCLR()     __MRC14(0, c7, c9, 6)
#define __ARCH_REG_STORE__DBGCLAIMCLR(val) __MCR14(0, c7, c9, 6, val)

/** [RO] Authentication Status Register */
#define DBGAUTHSTATUS                      DBGAUTHSTATUS
#define __ARCH_REG_LOAD__DBGAUTHSTATUS()   __MRC14(0, c7, c14, 6)

/** [RO/DBGv7.1] Debug Device ID Register 2 */
#define DBGDEVID2                          DBGDEVID2
#define __ARCH_REG_LOAD__DBGDEVID2()       __MRC14(0, c7, c0, 7)

/** [RO/DBGv7.1] Debug Device ID Register 1 */
#define DBGDEVID1                          DBGDEVID1
#define __ARCH_REG_LOAD__DBGDEVID1()       __MRC14(0, c7, c1, 7)

/** [RO] Debug Device ID Register */
#define DBGDEVID                           DBGDEVID
#define __ARCH_REG_LOAD__DBGDEVID()        __MRC14(0, c7, c2, 7)

#endif /* ARMLIB_REG_CP14_H_ */
