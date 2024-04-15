/**
 * @file
 * @brief
 *
 * @date 02.12.23
 * @author Aleksey Zhmulin
 */
#ifndef AARCH64_REG_SPECREG_H_
#define AARCH64_REG_SPECREG_H_

#include <stdint.h>

#define __MSR_imm(reg, val) __asm__ __volatile__("msr " #reg ",#" #val)

#define __MRS(reg)                                          \
	({                                                      \
		register uint64_t __val;                            \
		__asm__ __volatile__("mrs %0," #reg : "=r"(__val)); \
		__val;                                              \
	})

#define __MSR(reg, val)                                          \
	({                                                           \
		register uint64_t __val = val;                           \
		__asm__ __volatile__("msr " #reg ",%x0" : : "r"(__val)); \
	})

/** [RO] Current Exception Level */
#define CurrentEL                        CurrentEL
#define __ARCH_REG_LOAD__CurrentEL()     __MRS(CurrentEL)

#define CurrentEL_EL                     /* Current exception level */
#define CurrentEL_EL_MASK                0b11
#define CurrentEL_EL_SHIFT               2

/** Interrupt Mask Bits */
#define DAIF                             DAIF
#define __ARCH_REG_LOAD__DAIF()          __MRS(DAIF)
#define __ARCH_REG_STORE__DAIF(val)      __MSR(DAIF, val)

#define DAIF_D                           (1U << 9) /* Debug mask bit */
#define DAIF_A                           (1U << 8) /* System Error mask bit */
#define DAIF_I                           (1U << 7) /* IRQ mask bit */
#define DAIF_F                           (1U << 6) /* FIQ mask bit */

/** [WO] Set any or all of DAIF to 1 */
#define DAIFSet                          DAIFSet
#define __ARCH_REG_STORE__DAIFSet(val)   __MSR_imm(DAIFSet, val)

/** [WO] Clear any or all of DAIF to 0 */
#define DAIFClr                          DAIFClr
#define __ARCH_REG_STORE__DAIFClr(val)   __MSR_imm(DAIFClr, val)

#define DAIF_D_imm                       (1U << 3) /* Debug mask bit */
#define DAIF_A_imm                       (1U << 2) /* System Error mask bit */
#define DAIF_I_imm                       (1U << 1) /* IRQ mask bit */
#define DAIF_F_imm                       (1U << 0) /* FIQ mask bit */

/** Debug Link Register */
#define DLR_EL0                          DLR_EL0
#define __ARCH_REG_LOAD__DLR_EL0()       __MRS(DLR_EL0)
#define __ARCH_REG_STORE__DLR_EL0(val)   __MSR(DLR_EL0, val)

/** Debug Saved Program Status Register */
#define DSPSR_EL0                        DSPSR_EL0
#define __ARCH_REG_LOAD__DSPSR_EL0()     __MRS(DSPSR_EL0)
#define __ARCH_REG_STORE__DSPSR_EL0(val) __MSR(DSPSR_EL0, val)

/** Exception Link Register (EL1) */
#define ELR_EL1                          ELR_EL1
#define __ARCH_REG_LOAD__ELR_EL1()       __MRS(ELR_EL1)
#define __ARCH_REG_STORE__ELR_EL1(val)   __MSR(ELR_EL1, val)

/** Exception Link Register (EL2) */
#define ELR_EL2                          ELR_EL2
#define __ARCH_REG_LOAD__ELR_EL2()       __MRS(ELR_EL2)
#define __ARCH_REG_STORE__ELR_EL2(val)   __MSR(ELR_EL2, val)

/** Exception Link Register (EL3) */
#define ELR_EL3                          ELR_EL3
#define __ARCH_REG_LOAD__ELR_EL3()       __MRS(ELR_EL3)
#define __ARCH_REG_STORE__ELR_EL3(val)   __MSR(ELR_EL3, val)

/** Floating-point Control Register */
#define FPCR                             FPCR
#define __ARCH_REG_LOAD__FPCR()          __MRS(FPCR)
#define __ARCH_REG_STORE__FPCR(val)      __MSR(FPCR, val)

/** Floating-point Status Register */
#define FPSR                             FPSR
#define __ARCH_REG_LOAD__FPSR()          __MRS(FPSR)
#define __ARCH_REG_STORE__FPSR(val)      __MSR(FPSR, val)

/** Condition Flags */
#define NZCV                             NZCV
#define __ARCH_REG_LOAD__NZCV()          __MRS(NZCV)
#define __ARCH_REG_STORE__NZCV(val)      __MSR(NZCV, val)

/** Stack Pointer (EL0) */
#define SP_EL0                           SP_EL0
#define __ARCH_REG_LOAD__SP_EL0()        __MRS(SP_EL0)
#define __ARCH_REG_STORE__SP_EL0(val)    __MSR(SP_EL0, val)

/** Stack Pointer (EL1) */
#define SP_EL1                           SP_EL1
#define __ARCH_REG_LOAD__SP_EL1()        __MRS(SP_EL1)
#define __ARCH_REG_STORE__SP_EL1(val)    __MSR(SP_EL1, val)

/** Stack Pointer (EL2) */
#define SP_EL2                           SP_EL2
#define __ARCH_REG_LOAD__SP_EL2()        __MRS(SP_EL2)
#define __ARCH_REG_STORE__SP_EL2(val)    __MSR(SP_EL2, val)

/** Stack Pointer (EL3) */
#define SP_EL3                           SP_EL3
#define __ARCH_REG_LOAD__SP_EL3()        __MRS(SP_EL3)
#define __ARCH_REG_STORE__SP_EL3(val)    __MSR(SP_EL3, val)

/** Select the Stack Pointer, between SP_EL0 and SP_ELx */
#define SPSel                            SPSel
#define __ARCH_REG_LOAD__SPSel()         __MRS(SPSel)
#define __ARCH_REG_STORE__SPSel(val)     __MSR_imm(SPSel, val)

/** Saved Program Status Register (EL1) */
#define SPSR_EL1                         SPSR_EL1
#define __ARCH_REG_LOAD__SPSR_EL1()      __MRS(SPSR_EL1)
#define __ARCH_REG_STORE__SPSR_EL1(val)  __MSR(SPSR_EL1, val)

/** Saved Program Status Register (EL2) */
#define SPSR_EL2                         SPSR_EL2
#define __ARCH_REG_LOAD__SPSR_EL2()      __MRS(SPSR_EL2)
#define __ARCH_REG_STORE__SPSR_EL2(val)  __MSR(SPSR_EL2, val)

/** Saved Program Status Register (EL3) */
#define SPSR_EL3                         SPSR_EL3
#define __ARCH_REG_LOAD__SPSR_EL3()      __MRS(SPSR_EL3)
#define __ARCH_REG_STORE__SPSR_EL3(val)  __MSR(SPSR_EL3, val)

#define SPSR_ELn_M_EL0t                  0b00000
#define SPSR_ELn_M_EL1t                  0b00100
#define SPSR_ELn_M_EL1h                  0b00101
#define SPSR_ELn_M_EL2t                  0b01000
#define SPSR_ELn_M_EL2h                  0b01001
#define SPSR_ELn_M_EL3t                  0b01100
#define SPSR_ELn_M_EL3h                  0b01101

#define SPSR_ELn_D                       (1U << 9) /* Debug mask bit */
#define SPSR_ELn_A                       (1U << 8) /* System Error mask bit */
#define SPSR_ELn_I                       (1U << 7) /* IRQ mask bit */
#define SPSR_ELn_F                       (1U << 6) /* FIQ mask bit */

#define SPSR_ELn_M                       /* Mode that an exception was taken from */
#define SPSR_ELn_M_MASK                  0xb11111U
#define SPSR_ELn_M_SHIFT                 0

#endif /* AARCH64_REG_SPECREG_H_ */
