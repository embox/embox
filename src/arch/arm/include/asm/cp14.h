/**
 * @file
 * @brief
 *
 * @date 16.06.23
 * @author Aleksey Zhmulin
 */

#ifndef ARM_CP14_H_
#define ARM_CP14_H_

#include <stdint.h>

#define __MCR14(val, op1, crn, crm, op2)                                   \
	({                                                                     \
		uint32_t _val = val;                                               \
		__asm__ __volatile__("mcr p14," #op1 ",%0," #crn "," #crm "," #op2 \
		                     :                                             \
		                     : "r"(_val));                                 \
	})

#define __MRC14(op1, crn, crm, op2)                                        \
	({                                                                     \
		uint32_t _val;                                                     \
		__asm__ __volatile__("mrc p14," #op1 ",%0," #crn "," #crm "," #op2 \
		                     : "=r"(_val));                                \
		_val;                                                              \
	})

#define CP14_STORE(reg, val) CP14_STORE_##reg(val)
#define CP14_LOAD(reg)       CP14_LOAD_##reg()

#define CP14_ORIN(reg, mask) \
	CP14_STORE_##reg(CP14_LOAD_##reg() | (uint32_t)(mask))

#define CP14_ANDIN(reg, mask) \
	CP14_STORE_##reg(CP14_LOAD_##reg() & (uint32_t)(mask))

#define CP14_CLEAR(reg, mask) \
	CP14_STORE_##reg(CP14_LOAD_##reg() & (~((uint32_t)(mask))))

/*
 * Debug Registers
 *
 * Available only in DBGv7
 * DBGECR, DBGDSCCR, DBGDSMCR, DBGDRCR
 *
 * Available only in DBGv7.1
 * DBGBXVRm, DBGOSDLR, DBGDEVID2, DBGDEVID1
 *
 * Read only
 * DBGDIDR, DBGDSCRint, DBGDTRRXint, DBGDRAR, DBGOSLSR, DBGOSSRR, DBGPRSR,
 * DBGPRSR, DBGDSAR, DBGAUTHSTATUS, DBGDEVID2, DBGDEVID1, DBGDEVID
 *
 * Write only
 * DBGDTRTXint, DBGOSLAR
 */

#define CP14_LOAD_DBGDIDR()       __MRC14(0, c0, c0, 0)
#define CP14_LOAD_DBGDSCRint()    __MRC14(0, c0, c1, 0)
#define CP14_LOAD_DBGDTRRXint()   __MRC14(0, c0, c5, 0)
#define CP14_LOAD_DBGWFAR()       __MRC14(0, c0, c6, 0)
#define CP14_LOAD_DBGVCR()        __MRC14(0, c0, c7, 0)
#define CP14_LOAD_DBGECR()        __MRC14(0, c0, c9, 0)
#define CP14_LOAD_DBGDSCCR()      __MRC14(0, c0, c10, 0)
#define CP14_LOAD_DBGDSMCR()      __MRC14(0, c0, c11, 0)
#define CP14_LOAD_DBGDTRRXext()   __MRC14(0, c0, c0, 2)
#define CP14_LOAD_DBGDSCRext()    __MRC14(0, c0, c2, 2)
#define CP14_LOAD_DBGDTRTXext()   __MRC14(0, c0, c3, 2)
#define CP14_LOAD_DBGDRCR()       __MRC14(0, c0, c4, 2)
#define CP14_LOAD_DBGBVR0()       __MRC14(0, c0, c0, 4)
#define CP14_LOAD_DBGBVR1()       __MRC14(0, c0, c1, 4)
#define CP14_LOAD_DBGBVR2()       __MRC14(0, c0, c2, 4)
#define CP14_LOAD_DBGBVR3()       __MRC14(0, c0, c3, 4)
#define CP14_LOAD_DBGBVR4()       __MRC14(0, c0, c4, 4)
#define CP14_LOAD_DBGBVR5()       __MRC14(0, c0, c5, 4)
#define CP14_LOAD_DBGBVR6()       __MRC14(0, c0, c6, 4)
#define CP14_LOAD_DBGBVR7()       __MRC14(0, c0, c7, 4)
#define CP14_LOAD_DBGBVR8()       __MRC14(0, c0, c8, 4)
#define CP14_LOAD_DBGBVR9()       __MRC14(0, c0, c9, 4)
#define CP14_LOAD_DBGBVR10()      __MRC14(0, c0, c10, 4)
#define CP14_LOAD_DBGBVR11()      __MRC14(0, c0, c11, 4)
#define CP14_LOAD_DBGBVR12()      __MRC14(0, c0, c12, 4)
#define CP14_LOAD_DBGBVR13()      __MRC14(0, c0, c13, 4)
#define CP14_LOAD_DBGBVR14()      __MRC14(0, c0, c14, 4)
#define CP14_LOAD_DBGBVR15()      __MRC14(0, c0, c15, 4)
#define CP14_LOAD_DBGBCR0()       __MRC14(0, c0, c0, 5)
#define CP14_LOAD_DBGBCR1()       __MRC14(0, c0, c1, 5)
#define CP14_LOAD_DBGBCR2()       __MRC14(0, c0, c2, 5)
#define CP14_LOAD_DBGBCR3()       __MRC14(0, c0, c3, 5)
#define CP14_LOAD_DBGBCR4()       __MRC14(0, c0, c4, 5)
#define CP14_LOAD_DBGBCR5()       __MRC14(0, c0, c5, 5)
#define CP14_LOAD_DBGBCR6()       __MRC14(0, c0, c6, 5)
#define CP14_LOAD_DBGBCR7()       __MRC14(0, c0, c7, 5)
#define CP14_LOAD_DBGBCR8()       __MRC14(0, c0, c8, 5)
#define CP14_LOAD_DBGBCR9()       __MRC14(0, c0, c9, 5)
#define CP14_LOAD_DBGBCR10()      __MRC14(0, c0, c10, 5)
#define CP14_LOAD_DBGBCR11()      __MRC14(0, c0, c11, 5)
#define CP14_LOAD_DBGBCR12()      __MRC14(0, c0, c12, 5)
#define CP14_LOAD_DBGBCR13()      __MRC14(0, c0, c13, 5)
#define CP14_LOAD_DBGBCR14()      __MRC14(0, c0, c14, 5)
#define CP14_LOAD_DBGBCR15()      __MRC14(0, c0, c15, 5)
#define CP14_LOAD_DBGWVR0()       __MRC14(0, c0, c0, 6)
#define CP14_LOAD_DBGWVR1()       __MRC14(0, c0, c1, 6)
#define CP14_LOAD_DBGWVR2()       __MRC14(0, c0, c2, 6)
#define CP14_LOAD_DBGWVR3()       __MRC14(0, c0, c3, 6)
#define CP14_LOAD_DBGWVR4()       __MRC14(0, c0, c4, 6)
#define CP14_LOAD_DBGWVR5()       __MRC14(0, c0, c5, 6)
#define CP14_LOAD_DBGWVR6()       __MRC14(0, c0, c6, 6)
#define CP14_LOAD_DBGWVR7()       __MRC14(0, c0, c7, 6)
#define CP14_LOAD_DBGWVR8()       __MRC14(0, c0, c8, 6)
#define CP14_LOAD_DBGWVR9()       __MRC14(0, c0, c9, 6)
#define CP14_LOAD_DBGWVR10()      __MRC14(0, c0, c10, 6)
#define CP14_LOAD_DBGWVR11()      __MRC14(0, c0, c11, 6)
#define CP14_LOAD_DBGWVR12()      __MRC14(0, c0, c12, 6)
#define CP14_LOAD_DBGWVR13()      __MRC14(0, c0, c13, 6)
#define CP14_LOAD_DBGWVR14()      __MRC14(0, c0, c14, 6)
#define CP14_LOAD_DBGWVR15()      __MRC14(0, c0, c15, 6)
#define CP14_LOAD_DBGWCR0()       __MRC14(0, c0, c0, 7)
#define CP14_LOAD_DBGWCR1()       __MRC14(0, c0, c1, 7)
#define CP14_LOAD_DBGWCR2()       __MRC14(0, c0, c2, 7)
#define CP14_LOAD_DBGWCR3()       __MRC14(0, c0, c3, 7)
#define CP14_LOAD_DBGWCR4()       __MRC14(0, c0, c4, 7)
#define CP14_LOAD_DBGWCR5()       __MRC14(0, c0, c5, 7)
#define CP14_LOAD_DBGWCR6()       __MRC14(0, c0, c6, 7)
#define CP14_LOAD_DBGWCR7()       __MRC14(0, c0, c7, 7)
#define CP14_LOAD_DBGWCR8()       __MRC14(0, c0, c8, 7)
#define CP14_LOAD_DBGWCR9()       __MRC14(0, c0, c9, 7)
#define CP14_LOAD_DBGWCR10()      __MRC14(0, c0, c10, 7)
#define CP14_LOAD_DBGWCR11()      __MRC14(0, c0, c11, 7)
#define CP14_LOAD_DBGWCR12()      __MRC14(0, c0, c12, 7)
#define CP14_LOAD_DBGWCR13()      __MRC14(0, c0, c13, 7)
#define CP14_LOAD_DBGWCR14()      __MRC14(0, c0, c14, 7)
#define CP14_LOAD_DBGWCR15()      __MRC14(0, c0, c15, 7)
#define CP14_LOAD_DBGDRAR()       __MRC14(0, c1, c0, 0)
#define CP14_LOAD_DBGBXVR0()      __MRC14(0, c1, c0, 1)
#define CP14_LOAD_DBGBXVR1()      __MRC14(0, c1, c1, 1)
#define CP14_LOAD_DBGBXVR2()      __MRC14(0, c1, c2, 1)
#define CP14_LOAD_DBGBXVR3()      __MRC14(0, c1, c3, 1)
#define CP14_LOAD_DBGBXVR4()      __MRC14(0, c1, c4, 1)
#define CP14_LOAD_DBGBXVR5()      __MRC14(0, c1, c5, 1)
#define CP14_LOAD_DBGBXVR6()      __MRC14(0, c1, c6, 1)
#define CP14_LOAD_DBGBXVR7()      __MRC14(0, c1, c7, 1)
#define CP14_LOAD_DBGBXVR8()      __MRC14(0, c1, c8, 1)
#define CP14_LOAD_DBGBXVR9()      __MRC14(0, c1, c9, 1)
#define CP14_LOAD_DBGBXVR10()     __MRC14(0, c1, c10, 1)
#define CP14_LOAD_DBGBXVR11()     __MRC14(0, c1, c11, 1)
#define CP14_LOAD_DBGBXVR12()     __MRC14(0, c1, c12, 1)
#define CP14_LOAD_DBGBXVR13()     __MRC14(0, c1, c13, 1)
#define CP14_LOAD_DBGBXVR14()     __MRC14(0, c1, c14, 1)
#define CP14_LOAD_DBGBXVR15()     __MRC14(0, c1, c15, 1)
#define CP14_LOAD_DBGOSLSR()      __MRC14(0, c1, c1, 4)
#define CP14_LOAD_DBGOSSRR()      __MRC14(0, c1, c2, 4)
#define CP14_LOAD_DBGOSDLR()      __MRC14(0, c1, c3, 4)
#define CP14_LOAD_DBGPRCR()       __MRC14(0, c1, c4, 4)
#define CP14_LOAD_DBGPRSR()       __MRC14(0, c1, c5, 4)
#define CP14_LOAD_DBGDSAR()       __MRC14(0, c2, c0, 0)
#define CP14_LOAD_DBGITCTRL()     __MRC14(0, c7, c0, 4)
#define CP14_LOAD_DBGCLAIMSET()   __MRC14(0, c7, c8, 6)
#define CP14_LOAD_DBGCLAIMCLR()   __MRC14(0, c7, c9, 6)
#define CP14_LOAD_DBGAUTHSTATUS() __MRC14(0, c7, c14, 6)
#define CP14_LOAD_DBGDEVID2()     __MRC14(0, c7, c0, 7)
#define CP14_LOAD_DBGDEVID1()     __MRC14(0, c7, c1, 7)
#define CP14_LOAD_DBGDEVID()      __MRC14(0, c7, c2, 7)

#define CP14_STORE_DBGDTRTXint(val) __MCR14(val, 0, c0, c5, 0)
#define CP14_STORE_DBGWFAR(val)     __MCR14(val, 0, c0, c6, 0)
#define CP14_STORE_DBGVCR(val)      __MCR14(val, 0, c0, c7, 0)
#define CP14_STORE_DBGECR(val)      __MCR14(val, 0, c0, c9, 0)
#define CP14_STORE_DBGDSCCR(val)    __MCR14(val, 0, c0, c10, 0)
#define CP14_STORE_DBGDSMCR(val)    __MCR14(val, 0, c0, c11, 0)
#define CP14_STORE_DBGDTRRXext(val) __MCR14(val, 0, c0, c0, 2)
#define CP14_STORE_DBGDSCRext(val)  __MCR14(val, 0, c0, c2, 2)
#define CP14_STORE_DBGDTRTXext(val) __MCR14(val, 0, c0, c3, 2)
#define CP14_STORE_DBGDRCR(val)     __MCR14(val, 0, c0, c4, 2)
#define CP14_STORE_DBGBVR0(val)     __MCR14(val, 0, c0, c0, 4)
#define CP14_STORE_DBGBVR1(val)     __MCR14(val, 0, c0, c1, 4)
#define CP14_STORE_DBGBVR2(val)     __MCR14(val, 0, c0, c2, 4)
#define CP14_STORE_DBGBVR3(val)     __MCR14(val, 0, c0, c3, 4)
#define CP14_STORE_DBGBVR4(val)     __MCR14(val, 0, c0, c4, 4)
#define CP14_STORE_DBGBVR5(val)     __MCR14(val, 0, c0, c5, 4)
#define CP14_STORE_DBGBVR6(val)     __MCR14(val, 0, c0, c6, 4)
#define CP14_STORE_DBGBVR7(val)     __MCR14(val, 0, c0, c7, 4)
#define CP14_STORE_DBGBVR8(val)     __MCR14(val, 0, c0, c8, 4)
#define CP14_STORE_DBGBVR9(val)     __MCR14(val, 0, c0, c9, 4)
#define CP14_STORE_DBGBVR10(val)    __MCR14(val, 0, c0, c10, 4)
#define CP14_STORE_DBGBVR11(val)    __MCR14(val, 0, c0, c11, 4)
#define CP14_STORE_DBGBVR12(val)    __MCR14(val, 0, c0, c12, 4)
#define CP14_STORE_DBGBVR13(val)    __MCR14(val, 0, c0, c13, 4)
#define CP14_STORE_DBGBVR14(val)    __MCR14(val, 0, c0, c14, 4)
#define CP14_STORE_DBGBVR15(val)    __MCR14(val, 0, c0, c15, 4)
#define CP14_STORE_DBGBCR0(val)     __MCR14(val, 0, c0, c0, 5)
#define CP14_STORE_DBGBCR1(val)     __MCR14(val, 0, c0, c1, 5)
#define CP14_STORE_DBGBCR2(val)     __MCR14(val, 0, c0, c2, 5)
#define CP14_STORE_DBGBCR3(val)     __MCR14(val, 0, c0, c3, 5)
#define CP14_STORE_DBGBCR4(val)     __MCR14(val, 0, c0, c4, 5)
#define CP14_STORE_DBGBCR5(val)     __MCR14(val, 0, c0, c5, 5)
#define CP14_STORE_DBGBCR6(val)     __MCR14(val, 0, c0, c6, 5)
#define CP14_STORE_DBGBCR7(val)     __MCR14(val, 0, c0, c7, 5)
#define CP14_STORE_DBGBCR8(val)     __MCR14(val, 0, c0, c8, 5)
#define CP14_STORE_DBGBCR9(val)     __MCR14(val, 0, c0, c9, 5)
#define CP14_STORE_DBGBCR10(val)    __MCR14(val, 0, c0, c10, 5)
#define CP14_STORE_DBGBCR11(val)    __MCR14(val, 0, c0, c11, 5)
#define CP14_STORE_DBGBCR12(val)    __MCR14(val, 0, c0, c12, 5)
#define CP14_STORE_DBGBCR13(val)    __MCR14(val, 0, c0, c13, 5)
#define CP14_STORE_DBGBCR14(val)    __MCR14(val, 0, c0, c14, 5)
#define CP14_STORE_DBGBCR15(val)    __MCR14(val, 0, c0, c15, 5)
#define CP14_STORE_DBGWVR0(val)     __MCR14(val, 0, c0, c0, 6)
#define CP14_STORE_DBGWVR1(val)     __MCR14(val, 0, c0, c1, 6)
#define CP14_STORE_DBGWVR2(val)     __MCR14(val, 0, c0, c2, 6)
#define CP14_STORE_DBGWVR3(val)     __MCR14(val, 0, c0, c3, 6)
#define CP14_STORE_DBGWVR4(val)     __MCR14(val, 0, c0, c4, 6)
#define CP14_STORE_DBGWVR5(val)     __MCR14(val, 0, c0, c5, 6)
#define CP14_STORE_DBGWVR6(val)     __MCR14(val, 0, c0, c6, 6)
#define CP14_STORE_DBGWVR7(val)     __MCR14(val, 0, c0, c7, 6)
#define CP14_STORE_DBGWVR8(val)     __MCR14(val, 0, c0, c8, 6)
#define CP14_STORE_DBGWVR9(val)     __MCR14(val, 0, c0, c9, 6)
#define CP14_STORE_DBGWVR10(val)    __MCR14(val, 0, c0, c10, 6)
#define CP14_STORE_DBGWVR11(val)    __MCR14(val, 0, c0, c11, 6)
#define CP14_STORE_DBGWVR12(val)    __MCR14(val, 0, c0, c12, 6)
#define CP14_STORE_DBGWVR13(val)    __MCR14(val, 0, c0, c13, 6)
#define CP14_STORE_DBGWVR14(val)    __MCR14(val, 0, c0, c14, 6)
#define CP14_STORE_DBGWVR15(val)    __MCR14(val, 0, c0, c15, 6)
#define CP14_STORE_DBGWCR0(val)     __MCR14(val, 0, c0, c0, 7)
#define CP14_STORE_DBGWCR1(val)     __MCR14(val, 0, c0, c1, 7)
#define CP14_STORE_DBGWCR2(val)     __MCR14(val, 0, c0, c2, 7)
#define CP14_STORE_DBGWCR3(val)     __MCR14(val, 0, c0, c3, 7)
#define CP14_STORE_DBGWCR4(val)     __MCR14(val, 0, c0, c4, 7)
#define CP14_STORE_DBGWCR5(val)     __MCR14(val, 0, c0, c5, 7)
#define CP14_STORE_DBGWCR6(val)     __MCR14(val, 0, c0, c6, 7)
#define CP14_STORE_DBGWCR7(val)     __MCR14(val, 0, c0, c7, 7)
#define CP14_STORE_DBGWCR8(val)     __MCR14(val, 0, c0, c8, 7)
#define CP14_STORE_DBGWCR9(val)     __MCR14(val, 0, c0, c9, 7)
#define CP14_STORE_DBGWCR10(val)    __MCR14(val, 0, c0, c10, 7)
#define CP14_STORE_DBGWCR11(val)    __MCR14(val, 0, c0, c11, 7)
#define CP14_STORE_DBGWCR12(val)    __MCR14(val, 0, c0, c12, 7)
#define CP14_STORE_DBGWCR13(val)    __MCR14(val, 0, c0, c13, 7)
#define CP14_STORE_DBGWCR14(val)    __MCR14(val, 0, c0, c14, 7)
#define CP14_STORE_DBGWCR15(val)    __MCR14(val, 0, c0, c15, 7)
#define CP14_STORE_DBGBXVR0(val)    __MCR14(val, 0, c1, c0, 1)
#define CP14_STORE_DBGBXVR1(val)    __MCR14(val, 0, c1, c1, 1)
#define CP14_STORE_DBGBXVR2(val)    __MCR14(val, 0, c1, c2, 1)
#define CP14_STORE_DBGBXVR3(val)    __MCR14(val, 0, c1, c3, 1)
#define CP14_STORE_DBGBXVR4(val)    __MCR14(val, 0, c1, c4, 1)
#define CP14_STORE_DBGBXVR5(val)    __MCR14(val, 0, c1, c5, 1)
#define CP14_STORE_DBGBXVR6(val)    __MCR14(val, 0, c1, c6, 1)
#define CP14_STORE_DBGBXVR7(val)    __MCR14(val, 0, c1, c7, 1)
#define CP14_STORE_DBGBXVR8(val)    __MCR14(val, 0, c1, c8, 1)
#define CP14_STORE_DBGBXVR9(val)    __MCR14(val, 0, c1, c9, 1)
#define CP14_STORE_DBGBXVR10(val)   __MCR14(val, 0, c1, c10, 1)
#define CP14_STORE_DBGBXVR11(val)   __MCR14(val, 0, c1, c11, 1)
#define CP14_STORE_DBGBXVR12(val)   __MCR14(val, 0, c1, c12, 1)
#define CP14_STORE_DBGBXVR13(val)   __MCR14(val, 0, c1, c13, 1)
#define CP14_STORE_DBGBXVR14(val)   __MCR14(val, 0, c1, c14, 1)
#define CP14_STORE_DBGBXVR15(val)   __MCR14(val, 0, c1, c15, 1)
#define CP14_STORE_DBGOSLAR(val)    __MCR14(val, 0, c1, c0, 4)
#define CP14_STORE_DBGOSSRR(val)    __MCR14(val, 0, c1, c2, 4)
#define CP14_STORE_DBGOSDLR(val)    __MCR14(val, 0, c1, c3, 4)
#define CP14_STORE_DBGPRCR(val)     __MCR14(val, 0, c1, c4, 4)
#define CP14_STORE_DBGITCTRL(val)   __MCR14(val, 0, c7, c0, 4)
#define CP14_STORE_DBGCLAIMSET(val) __MCR14(val, 0, c7, c8, 6)
#define CP14_STORE_DBGCLAIMCLR(val) __MCR14(val, 0, c7, c9, 6)

#endif /* ARM_CP14_H_ */
