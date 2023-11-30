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

#define DBGDIDR                            /* [RO] Debug ID Register */
#define __ARCH_REG_LOAD__DBGDIDR()         __MRC14(0, c0, c0, 0)

#define DBGDSCRint                         /* [RO] Debug Status and Control Register (internal view) */
#define __ARCH_REG_LOAD__DBGDSCRint()      __MRC14(0, c0, c1, 0)

#define DBGDTRRXint                        /* [RO] Host to Target Data Transfer Register (internal view) */
#define __ARCH_REG_LOAD__DBGDTRRXint()     __MRC14(0, c0, c5, 0)

#define DBGDTRTXint                        /* [WO] Target to Host Data Transfer Register (internal view) */
#define __ARCH_REG_STORE__DBGDTRTXint(val) __MCR14(0, c0, c5, 0, val)

#define DBGWFAR                            /* Watchpoint Fault Address Register */
#define __ARCH_REG_LOAD__DBGWFAR()         __MRC14(0, c0, c6, 0)
#define __ARCH_REG_STORE__DBGWFAR(val)     __MCR14(0, c0, c6, 0, val)

#define DBGVCR                             /* Vector Catch Register */
#define __ARCH_REG_LOAD__DBGVCR()          __MRC14(0, c0, c7, 0)
#define __ARCH_REG_STORE__DBGVCR(val)      __MCR14(0, c0, c7, 0, val)

#define DBGECR                             /* [DBGv7] Event Catch Register */
#define __ARCH_REG_LOAD__DBGECR()          __MRC14(0, c0, c9, 0)
#define __ARCH_REG_STORE__DBGECR(val)      __MCR14(0, c0, c9, 0, val)

#define DBGDSCCR                           /* [DBGv7] Debug State Cache Control Register */
#define __ARCH_REG_LOAD__DBGDSCCR()        __MRC14(0, c0, c10, 0)
#define __ARCH_REG_STORE__DBGDSCCR(val)    __MCR14(0, c0, c10, 0, val)

#define DBGDSMCR                           /* [DBGv7] Debug State MMU Control Register */
#define __ARCH_REG_LOAD__DBGDSMCR()        __MRC14(0, c0, c11, 0)
#define __ARCH_REG_STORE__DBGDSMCR(val)    __MCR14(0, c0, c11, 0, val)

#define DBGDTRRXext                        /* Host to Target Data Transfer Register (external view) */
#define __ARCH_REG_LOAD__DBGDTRRXext()     __MRC14(0, c0, c0, 2)
#define __ARCH_REG_STORE__DBGDTRRXext(val) __MCR14(0, c0, c0, 2, val)

#define DBGDSCRext                         /* Debug Status and Control Register (external view) */
#define __ARCH_REG_LOAD__DBGDSCRext()      __MRC14(0, c0, c2, 2)
#define __ARCH_REG_STORE__DBGDSCRext(val)  __MCR14(0, c0, c2, 2, val)

#define DBGDSCR_ITREN                      (0b1U << 13)
#define DBGDSCR_MDBGEN                     (0b1U << 15)
#define DBGDSCR_EXTDCCMODE_MASK            (0b11U << 20)
#define DBGDSCR_EXTDCCMODE_STALL           (0b1U << 20)

#define DBGDTRTXext                        /* Target to Host Data Transfer Register (external view) */
#define __ARCH_REG_LOAD__DBGDTRTXext()     __MRC14(0, c0, c3, 2)
#define __ARCH_REG_STORE__DBGDTRTXext(val) __MCR14(0, c0, c3, 2, val)

#define DBGDRCR                            /* [DBGv7] Debug Run Control Register */
#define __ARCH_REG_LOAD__DBGDRCR()         __MRC14(0, c0, c4, 2)
#define __ARCH_REG_STORE__DBGDRCR(val)     __MCR14(0, c0, c4, 2, val)

#define DBGBVR0                            /* Breakpoint Value Register 0 */
#define __ARCH_REG_LOAD__DBGBVR0()         __MRC14(0, c0, c0, 4)
#define __ARCH_REG_STORE__DBGBVR0(val)     __MCR14(0, c0, c0, 4, val)

#define DBGBVR1                            /* Breakpoint Value Register 1 */
#define __ARCH_REG_LOAD__DBGBVR1()         __MRC14(0, c0, c1, 4)
#define __ARCH_REG_STORE__DBGBVR1(val)     __MCR14(0, c0, c1, 4, val)

#define DBGBVR2                            /* Breakpoint Value Register 2 */
#define __ARCH_REG_LOAD__DBGBVR2()         __MRC14(0, c0, c2, 4)
#define __ARCH_REG_STORE__DBGBVR2(val)     __MCR14(0, c0, c2, 4, val)

#define DBGBVR3                            /* Breakpoint Value Register 3 */
#define __ARCH_REG_LOAD__DBGBVR3()         __MRC14(0, c0, c3, 4)
#define __ARCH_REG_STORE__DBGBVR3(val)     __MCR14(0, c0, c3, 4, val)

#define DBGBVR4                            /* Breakpoint Value Register 4 */
#define __ARCH_REG_LOAD__DBGBVR4()         __MRC14(0, c0, c4, 4)
#define __ARCH_REG_STORE__DBGBVR4(val)     __MCR14(0, c0, c4, 4, val)

#define DBGBVR5                            /* Breakpoint Value Register 5 */
#define __ARCH_REG_LOAD__DBGBVR5()         __MRC14(0, c0, c5, 4)
#define __ARCH_REG_STORE__DBGBVR5(val)     __MCR14(0, c0, c5, 4, val)

#define DBGBVR6                            /* Breakpoint Value Register 6 */
#define __ARCH_REG_LOAD__DBGBVR6()         __MRC14(0, c0, c6, 4)
#define __ARCH_REG_STORE__DBGBVR6(val)     __MCR14(0, c0, c6, 4, val)

#define DBGBVR7                            /* Breakpoint Value Register 7 */
#define __ARCH_REG_LOAD__DBGBVR7()         __MRC14(0, c0, c7, 4)
#define __ARCH_REG_STORE__DBGBVR7(val)     __MCR14(0, c0, c7, 4, val)

#define DBGBVR8                            /* Breakpoint Value Register 8 */
#define __ARCH_REG_LOAD__DBGBVR8()         __MRC14(0, c0, c8, 4)
#define __ARCH_REG_STORE__DBGBVR8(val)     __MCR14(0, c0, c8, 4, val)

#define DBGBVR9                            /* Breakpoint Value Register 9 */
#define __ARCH_REG_LOAD__DBGBVR9()         __MRC14(0, c0, c9, 4)
#define __ARCH_REG_STORE__DBGBVR9(val)     __MCR14(0, c0, c9, 4, val)

#define DBGBVR10                           /* Breakpoint Value Register 10 */
#define __ARCH_REG_LOAD__DBGBVR10()        __MRC14(0, c0, c10, 4)
#define __ARCH_REG_STORE__DBGBVR10(val)    __MCR14(0, c0, c10, 4, val)

#define DBGBVR11                           /* Breakpoint Value Register 11 */
#define __ARCH_REG_LOAD__DBGBVR11()        __MRC14(0, c0, c11, 4)
#define __ARCH_REG_STORE__DBGBVR11(val)    __MCR14(0, c0, c11, 4, val)

#define DBGBVR12                           /* Breakpoint Value Register 12 */
#define __ARCH_REG_LOAD__DBGBVR12()        __MRC14(0, c0, c12, 4)
#define __ARCH_REG_STORE__DBGBVR12(val)    __MCR14(0, c0, c12, 4, val)

#define DBGBVR13                           /* Breakpoint Value Register 13 */
#define __ARCH_REG_LOAD__DBGBVR13()        __MRC14(0, c0, c13, 4)
#define __ARCH_REG_STORE__DBGBVR13(val)    __MCR14(0, c0, c13, 4, val)

#define DBGBVR14                           /* Breakpoint Value Register 14 */
#define __ARCH_REG_LOAD__DBGBVR14()        __MRC14(0, c0, c14, 4)
#define __ARCH_REG_STORE__DBGBVR14(val)    __MCR14(0, c0, c14, 4, val)

#define DBGBVR15                           /* Breakpoint Value Register 15 */
#define __ARCH_REG_LOAD__DBGBVR15()        __MRC14(0, c0, c15, 4)
#define __ARCH_REG_STORE__DBGBVR15(val)    __MCR14(0, c0, c15, 4, val)

#define DBGBCR0                            /* Breakpoint Control Register 0 */
#define __ARCH_REG_LOAD__DBGBCR0()         __MRC14(0, c0, c0, 5)
#define __ARCH_REG_STORE__DBGBCR0(val)     __MCR14(0, c0, c0, 5, val)

#define DBGBCR1                            /* Breakpoint Control Register 1 */
#define __ARCH_REG_LOAD__DBGBCR1()         __MRC14(0, c0, c1, 5)
#define __ARCH_REG_STORE__DBGBCR1(val)     __MCR14(0, c0, c1, 5, val)

#define DBGBCR2                            /* Breakpoint Control Register 2 */
#define __ARCH_REG_LOAD__DBGBCR2()         __MRC14(0, c0, c2, 5)
#define __ARCH_REG_STORE__DBGBCR2(val)     __MCR14(0, c0, c2, 5, val)

#define DBGBCR3                            /* Breakpoint Control Register 3 */
#define __ARCH_REG_LOAD__DBGBCR3()         __MRC14(0, c0, c3, 5)
#define __ARCH_REG_STORE__DBGBCR3(val)     __MCR14(0, c0, c3, 5, val)

#define DBGBCR4                            /* Breakpoint Control Register 4 */
#define __ARCH_REG_LOAD__DBGBCR4()         __MRC14(0, c0, c4, 5)
#define __ARCH_REG_STORE__DBGBCR4(val)     __MCR14(0, c0, c4, 5, val)

#define DBGBCR5                            /* Breakpoint Control Register 5 */
#define __ARCH_REG_LOAD__DBGBCR5()         __MRC14(0, c0, c5, 5)
#define __ARCH_REG_STORE__DBGBCR5(val)     __MCR14(0, c0, c5, 5, val)

#define DBGBCR6                            /* Breakpoint Control Register 6 */
#define __ARCH_REG_LOAD__DBGBCR6()         __MRC14(0, c0, c6, 5)
#define __ARCH_REG_STORE__DBGBCR6(val)     __MCR14(0, c0, c6, 5, val)

#define DBGBCR7                            /* Breakpoint Control Register 7 */
#define __ARCH_REG_LOAD__DBGBCR7()         __MRC14(0, c0, c7, 5)
#define __ARCH_REG_STORE__DBGBCR7(val)     __MCR14(0, c0, c7, 5, val)

#define DBGBCR8                            /* Breakpoint Control Register 8 */
#define __ARCH_REG_LOAD__DBGBCR8()         __MRC14(0, c0, c8, 5)
#define __ARCH_REG_STORE__DBGBCR8(val)     __MCR14(0, c0, c8, 5, val)

#define DBGBCR9                            /* Breakpoint Control Register 9 */
#define __ARCH_REG_LOAD__DBGBCR9()         __MRC14(0, c0, c9, 5)
#define __ARCH_REG_STORE__DBGBCR9(val)     __MCR14(0, c0, c9, 5, val)

#define DBGBCR10                           /* Breakpoint Control Register 10 */
#define __ARCH_REG_LOAD__DBGBCR10()        __MRC14(0, c0, c10, 5)
#define __ARCH_REG_STORE__DBGBCR10(val)    __MCR14(0, c0, c10, 5, val)

#define DBGBCR11                           /* Breakpoint Control Register 11 */
#define __ARCH_REG_LOAD__DBGBCR11()        __MRC14(0, c0, c11, 5)
#define __ARCH_REG_STORE__DBGBCR11(val)    __MCR14(0, c0, c11, 5, val)

#define DBGBCR12                           /* Breakpoint Control Register 12 */
#define __ARCH_REG_LOAD__DBGBCR12()        __MRC14(0, c0, c12, 5)
#define __ARCH_REG_STORE__DBGBCR12(val)    __MCR14(0, c0, c12, 5, val)

#define DBGBCR13                           /* Breakpoint Control Register 13 */
#define __ARCH_REG_LOAD__DBGBCR13()        __MRC14(0, c0, c13, 5)
#define __ARCH_REG_STORE__DBGBCR13(val)    __MCR14(0, c0, c13, 5, val)

#define DBGBCR14                           /* Breakpoint Control Register 14 */
#define __ARCH_REG_LOAD__DBGBCR14()        __MRC14(0, c0, c14, 5)
#define __ARCH_REG_STORE__DBGBCR14(val)    __MCR14(0, c0, c14, 5, val)

#define DBGBCR15                           /* Breakpoint Control Register 15 */
#define __ARCH_REG_LOAD__DBGBCR15()        __MRC14(0, c0, c15, 5)
#define __ARCH_REG_STORE__DBGBCR15(val)    __MCR14(0, c0, c15, 5, val)

#define DBGBCR_EN                          (0b1U << 0)
#define DBGBCR_PMC_ANY                     (0b11U << 1)
#define DBGBCR_BAS_ANY                     (0b1111U << 5)

#define DBGWVR0                            /* Watchpoint Value Register 0 */
#define __ARCH_REG_LOAD__DBGWVR0()         __MRC14(0, c0, c0, 6)
#define __ARCH_REG_STORE__DBGWVR0(val)     __MCR14(0, c0, c0, 6, val)

#define DBGWVR1                            /* Watchpoint Value Register 1 */
#define __ARCH_REG_LOAD__DBGWVR1()         __MRC14(0, c0, c1, 6)
#define __ARCH_REG_STORE__DBGWVR1(val)     __MCR14(0, c0, c1, 6, val)

#define DBGWVR2                            /* Watchpoint Value Register 2 */
#define __ARCH_REG_LOAD__DBGWVR2()         __MRC14(0, c0, c2, 6)
#define __ARCH_REG_STORE__DBGWVR2(val)     __MCR14(0, c0, c2, 6, val)

#define DBGWVR3                            /* Watchpoint Value Register 3 */
#define __ARCH_REG_LOAD__DBGWVR3()         __MRC14(0, c0, c3, 6)
#define __ARCH_REG_STORE__DBGWVR3(val)     __MCR14(0, c0, c3, 6, val)

#define DBGWVR4                            /* Watchpoint Value Register 4 */
#define __ARCH_REG_LOAD__DBGWVR4()         __MRC14(0, c0, c4, 6)
#define __ARCH_REG_STORE__DBGWVR4(val)     __MCR14(0, c0, c4, 6, val)

#define DBGWVR5                            /* Watchpoint Value Register 5 */
#define __ARCH_REG_LOAD__DBGWVR5()         __MRC14(0, c0, c5, 6)
#define __ARCH_REG_STORE__DBGWVR5(val)     __MCR14(0, c0, c5, 6, val)

#define DBGWVR6                            /* Watchpoint Value Register 6 */
#define __ARCH_REG_LOAD__DBGWVR6()         __MRC14(0, c0, c6, 6)
#define __ARCH_REG_STORE__DBGWVR6(val)     __MCR14(0, c0, c6, 6, val)

#define DBGWVR7                            /* Watchpoint Value Register 7 */
#define __ARCH_REG_LOAD__DBGWVR7()         __MRC14(0, c0, c7, 6)
#define __ARCH_REG_STORE__DBGWVR7(val)     __MCR14(0, c0, c7, 6, val)

#define DBGWVR8                            /* Watchpoint Value Register 8 */
#define __ARCH_REG_LOAD__DBGWVR8()         __MRC14(0, c0, c8, 6)
#define __ARCH_REG_STORE__DBGWVR8(val)     __MCR14(0, c0, c8, 6, val)

#define DBGWVR9                            /* Watchpoint Value Register 9 */
#define __ARCH_REG_LOAD__DBGWVR9()         __MRC14(0, c0, c9, 6)
#define __ARCH_REG_STORE__DBGWVR9(val)     __MCR14(0, c0, c9, 6, val)

#define DBGWVR10                           /* Watchpoint Value Register 10 */
#define __ARCH_REG_LOAD__DBGWVR10()        __MRC14(0, c0, c10, 6)
#define __ARCH_REG_STORE__DBGWVR10(val)    __MCR14(0, c0, c10, 6, val)

#define DBGWVR11                           /* Watchpoint Value Register 11 */
#define __ARCH_REG_LOAD__DBGWVR11()        __MRC14(0, c0, c11, 6)
#define __ARCH_REG_STORE__DBGWVR11(val)    __MCR14(0, c0, c11, 6, val)

#define DBGWVR12                           /* Watchpoint Value Register 12 */
#define __ARCH_REG_LOAD__DBGWVR12()        __MRC14(0, c0, c12, 6)
#define __ARCH_REG_STORE__DBGWVR12(val)    __MCR14(0, c0, c12, 6, val)

#define DBGWVR13                           /* Watchpoint Value Register 13 */
#define __ARCH_REG_LOAD__DBGWVR13()        __MRC14(0, c0, c13, 6)
#define __ARCH_REG_STORE__DBGWVR13(val)    __MCR14(0, c0, c13, 6, val)

#define DBGWVR14                           /* Watchpoint Value Register 14 */
#define __ARCH_REG_LOAD__DBGWVR14()        __MRC14(0, c0, c14, 6)
#define __ARCH_REG_STORE__DBGWVR14(val)    __MCR14(0, c0, c14, 6, val)

#define DBGWVR15                           /* Watchpoint Value Register 15 */
#define __ARCH_REG_LOAD__DBGWVR15()        __MRC14(0, c0, c15, 6)
#define __ARCH_REG_STORE__DBGWVR15(val)    __MCR14(0, c0, c15, 6, val)

#define DBGWCR0                            /* Watchpoint Control Register 0 */
#define __ARCH_REG_LOAD__DBGWCR0()         __MRC14(0, c0, c0, 7)
#define __ARCH_REG_STORE__DBGWCR0(val)     __MCR14(0, c0, c0, 7, val)

#define DBGWCR1                            /* Watchpoint Control Register 1 */
#define __ARCH_REG_LOAD__DBGWCR1()         __MRC14(0, c0, c1, 7)
#define __ARCH_REG_STORE__DBGWCR1(val)     __MCR14(0, c0, c1, 7, val)

#define DBGWCR2                            /* Watchpoint Control Register 2 */
#define __ARCH_REG_LOAD__DBGWCR2()         __MRC14(0, c0, c2, 7)
#define __ARCH_REG_STORE__DBGWCR2(val)     __MCR14(0, c0, c2, 7, val)

#define DBGWCR3                            /* Watchpoint Control Register 3 */
#define __ARCH_REG_LOAD__DBGWCR3()         __MRC14(0, c0, c3, 7)
#define __ARCH_REG_STORE__DBGWCR3(val)     __MCR14(0, c0, c3, 7, val)

#define DBGWCR4                            /* Watchpoint Control Register 4 */
#define __ARCH_REG_LOAD__DBGWCR4()         __MRC14(0, c0, c4, 7)
#define __ARCH_REG_STORE__DBGWCR4(val)     __MCR14(0, c0, c4, 7, val)

#define DBGWCR5                            /* Watchpoint Control Register 5 */
#define __ARCH_REG_LOAD__DBGWCR5()         __MRC14(0, c0, c5, 7)
#define __ARCH_REG_STORE__DBGWCR5(val)     __MCR14(0, c0, c5, 7, val)

#define DBGWCR6                            /* Watchpoint Control Register 6 */
#define __ARCH_REG_LOAD__DBGWCR6()         __MRC14(0, c0, c6, 7)
#define __ARCH_REG_STORE__DBGWCR6(val)     __MCR14(0, c0, c6, 7, val)

#define DBGWCR7                            /* Watchpoint Control Register 7 */
#define __ARCH_REG_LOAD__DBGWCR7()         __MRC14(0, c0, c7, 7)
#define __ARCH_REG_STORE__DBGWCR7(val)     __MCR14(0, c0, c7, 7, val)

#define DBGWCR8                            /* Watchpoint Control Register 8 */
#define __ARCH_REG_LOAD__DBGWCR8()         __MRC14(0, c0, c8, 7)
#define __ARCH_REG_STORE__DBGWCR8(val)     __MCR14(0, c0, c8, 7, val)

#define DBGWCR9                            /* Watchpoint Control Register 9 */
#define __ARCH_REG_LOAD__DBGWCR9()         __MRC14(0, c0, c9, 7)
#define __ARCH_REG_STORE__DBGWCR9(val)     __MCR14(0, c0, c9, 7, val)

#define DBGWCR10                           /* Watchpoint Control Register 10 */
#define __ARCH_REG_LOAD__DBGWCR10()        __MRC14(0, c0, c10, 7)
#define __ARCH_REG_STORE__DBGWCR10(val)    __MCR14(0, c0, c10, 7, val)

#define DBGWCR11                           /* Watchpoint Control Register 11 */
#define __ARCH_REG_LOAD__DBGWCR11()        __MRC14(0, c0, c11, 7)
#define __ARCH_REG_STORE__DBGWCR11(val)    __MCR14(0, c0, c11, 7, val)

#define DBGWCR12                           /* Watchpoint Control Register 12 */
#define __ARCH_REG_LOAD__DBGWCR12()        __MRC14(0, c0, c12, 7)
#define __ARCH_REG_STORE__DBGWCR12(val)    __MCR14(0, c0, c12, 7, val)

#define DBGWCR13                           /* Watchpoint Control Register 13 */
#define __ARCH_REG_LOAD__DBGWCR13()        __MRC14(0, c0, c13, 7)
#define __ARCH_REG_STORE__DBGWCR13(val)    __MCR14(0, c0, c13, 7, val)

#define DBGWCR14                           /* Watchpoint Control Register 14 */
#define __ARCH_REG_LOAD__DBGWCR14()        __MRC14(0, c0, c14, 7)
#define __ARCH_REG_STORE__DBGWCR14(val)    __MCR14(0, c0, c14, 7, val)

#define DBGWCR15                           /* Watchpoint Control Register 15 */
#define __ARCH_REG_LOAD__DBGWCR15()        __MRC14(0, c0, c15, 7)
#define __ARCH_REG_STORE__DBGWCR15(val)    __MCR14(0, c0, c15, 7, val)

#define DBGDRAR                            /* [RO] Debug ROM Address Register */
#define __ARCH_REG_LOAD__DBGDRAR()         __MRC14(0, c1, c0, 0)

#define DBGBXVR0                           /* [DBGv7.1] Breakpoint Extended Value Register 0 */
#define __ARCH_REG_LOAD__DBGBXVR0()        __MRC14(0, c1, c0, 1)
#define __ARCH_REG_STORE__DBGBXVR0(val)    __MCR14(0, c1, c0, 1, val)

#define DBGBXVR1                           /* [DBGv7.1] Breakpoint Extended Value Register 1 */
#define __ARCH_REG_LOAD__DBGBXVR1()        __MRC14(0, c1, c1, 1)
#define __ARCH_REG_STORE__DBGBXVR1(val)    __MCR14(0, c1, c1, 1, val)

#define DBGBXVR2                           /* [DBGv7.1] Breakpoint Extended Value Register 2 */
#define __ARCH_REG_LOAD__DBGBXVR2()        __MRC14(0, c1, c2, 1)
#define __ARCH_REG_STORE__DBGBXVR2(val)    __MCR14(0, c1, c2, 1, val)

#define DBGBXVR3                           /* [DBGv7.1] Breakpoint Extended Value Register 3 */
#define __ARCH_REG_LOAD__DBGBXVR3()        __MRC14(0, c1, c3, 1)
#define __ARCH_REG_STORE__DBGBXVR3(val)    __MCR14(0, c1, c3, 1, val)

#define DBGBXVR4                           /* [DBGv7.1] Breakpoint Extended Value Register 4 */
#define __ARCH_REG_LOAD__DBGBXVR4()        __MRC14(0, c1, c4, 1)
#define __ARCH_REG_STORE__DBGBXVR4(val)    __MCR14(0, c1, c4, 1, val)

#define DBGBXVR5                           /* [DBGv7.1] Breakpoint Extended Value Register 5 */
#define __ARCH_REG_LOAD__DBGBXVR5()        __MRC14(0, c1, c5, 1)
#define __ARCH_REG_STORE__DBGBXVR5(val)    __MCR14(0, c1, c5, 1, val)

#define DBGBXVR6                           /* [DBGv7.1] Breakpoint Extended Value Register 6 */
#define __ARCH_REG_LOAD__DBGBXVR6()        __MRC14(0, c1, c6, 1)
#define __ARCH_REG_STORE__DBGBXVR6(val)    __MCR14(0, c1, c6, 1, val)

#define DBGBXVR7                           /* [DBGv7.1] Breakpoint Extended Value Register 7 */
#define __ARCH_REG_LOAD__DBGBXVR7()        __MRC14(0, c1, c7, 1)
#define __ARCH_REG_STORE__DBGBXVR7(val)    __MCR14(0, c1, c7, 1, val)

#define DBGBXVR8                           /* [DBGv7.1] Breakpoint Extended Value Register 8 */
#define __ARCH_REG_LOAD__DBGBXVR8()        __MRC14(0, c1, c8, 1)
#define __ARCH_REG_STORE__DBGBXVR8(val)    __MCR14(0, c1, c8, 1, val)

#define DBGBXVR9                           /* [DBGv7.1] Breakpoint Extended Value Register 9 */
#define __ARCH_REG_LOAD__DBGBXVR9()        __MRC14(0, c1, c9, 1)
#define __ARCH_REG_STORE__DBGBXVR9(val)    __MCR14(0, c1, c9, 1, val)

#define DBGBXVR10                          /* [DBGv7.1] Breakpoint Extended Value Register 10 */
#define __ARCH_REG_LOAD__DBGBXVR10()       __MRC14(0, c1, c10, 1)
#define __ARCH_REG_STORE__DBGBXVR10(val)   __MCR14(0, c1, c10, 1, val)

#define DBGBXVR11                          /* [DBGv7.1] Breakpoint Extended Value Register 11 */
#define __ARCH_REG_LOAD__DBGBXVR11()       __MRC14(0, c1, c11, 1)
#define __ARCH_REG_STORE__DBGBXVR11(val)   __MCR14(0, c1, c11, 1, val)

#define DBGBXVR12                          /* [DBGv7.1] Breakpoint Extended Value Register 12 */
#define __ARCH_REG_LOAD__DBGBXVR12()       __MRC14(0, c1, c12, 1)
#define __ARCH_REG_STORE__DBGBXVR12(val)   __MCR14(0, c1, c12, 1, val)

#define DBGBXVR13                          /* [DBGv7.1] Breakpoint Extended Value Register 13 */
#define __ARCH_REG_LOAD__DBGBXVR13()       __MRC14(0, c1, c13, 1)
#define __ARCH_REG_STORE__DBGBXVR13(val)   __MCR14(0, c1, c13, 1, val)

#define DBGBXVR14                          /* [DBGv7.1] Breakpoint Extended Value Register 14 */
#define __ARCH_REG_LOAD__DBGBXVR14()       __MRC14(0, c1, c14, 1)
#define __ARCH_REG_STORE__DBGBXVR14(val)   __MCR14(0, c1, c14, 1, val)

#define DBGBXVR15                          /* [DBGv7.1] Breakpoint Extended Value Register 15 */
#define __ARCH_REG_LOAD__DBGBXVR15()       __MRC14(0, c1, c15, 1)
#define __ARCH_REG_STORE__DBGBXVR15(val)   __MCR14(0, c1, c15, 1, val)

#define DBGOSLAR                           /* [WO] OS Lock Access Register */
#define __ARCH_REG_STORE__DBGOSLAR(val)    __MCR14(0, c1, c0, 4, val)

#define DBGOSLSR                           /* [RO] OS Lock Status Register */
#define __ARCH_REG_LOAD__DBGOSLSR()        __MRC14(0, c1, c1, 4)

#define DBGOSSRR                           /* [RO] OS Save and Restore Register */
#define __ARCH_REG_LOAD__DBGOSSRR()        __MRC14(0, c1, c2, 4)
#define __ARCH_REG_STORE__DBGOSSRR(val)    __MCR14(0, c1, c2, 4, val)

#define DBGOSDLR                           /* [DBGv7.1] OS Double Lock Register */
#define __ARCH_REG_LOAD__DBGOSDLR()        __MRC14(0, c1, c3, 4)
#define __ARCH_REG_STORE__DBGOSDLR(val)    __MCR14(0, c1, c3, 4, val)

#define DBGPRCR                            /* Device Powerdown and Reset Control Register */
#define __ARCH_REG_LOAD__DBGPRCR()         __MRC14(0, c1, c4, 4)
#define __ARCH_REG_STORE__DBGPRCR(val)     __MCR14(0, c1, c4, 4, val)

#define DBGPRSR                            /* [RO] Device Powerdown and Reset Status Register */
#define __ARCH_REG_LOAD__DBGPRSR()         __MRC14(0, c1, c5, 4)

#define DBGDSAR                            /* [RO] Debug Self Address Offset Register */
#define __ARCH_REG_LOAD__DBGDSAR()         __MRC14(0, c2, c0, 0)

#define DBGITCTRL                          /* Integration Mode Control Register */
#define __ARCH_REG_LOAD__DBGITCTRL()       __MRC14(0, c7, c0, 4)
#define __ARCH_REG_STORE__DBGITCTRL(val)   __MCR14(0, c7, c0, 4, val)

#define DBGCLAIMSET                        /* Claim Tag Set Register */
#define __ARCH_REG_LOAD__DBGCLAIMSET()     __MRC14(0, c7, c8, 6)
#define __ARCH_REG_STORE__DBGCLAIMSET(val) __MCR14(0, c7, c8, 6, val)

#define DBGCLAIMCLR                        /* Claim Tag Clear Register */
#define __ARCH_REG_LOAD__DBGCLAIMCLR()     __MRC14(0, c7, c9, 6)
#define __ARCH_REG_STORE__DBGCLAIMCLR(val) __MCR14(0, c7, c9, 6, val)

#define DBGAUTHSTATUS                      /* [RO] Authentication Status Register */
#define __ARCH_REG_LOAD__DBGAUTHSTATUS()   __MRC14(0, c7, c14, 6)

#define DBGDEVID2                          /* [RO/DBGv7.1] Debug Device ID Register 2 */
#define __ARCH_REG_LOAD__DBGDEVID2()       __MRC14(0, c7, c0, 7)

#define DBGDEVID1                          /* [RO/DBGv7.1] Debug Device ID Register 1 */
#define __ARCH_REG_LOAD__DBGDEVID1()       __MRC14(0, c7, c1, 7)

#define DBGDEVID                           /* [RO] Debug Device ID Register */
#define __ARCH_REG_LOAD__DBGDEVID()        __MRC14(0, c7, c2, 7)

#endif /* ARMLIB_REG_CP14_H_ */
