/**
 * @file
 * @brief
 *
 * @date 27.11.23
 * @author Aleksey Zhmulin
 */
#ifndef ARMLIB_REG_CP15_H_
#define ARMLIB_REG_CP15_H_

#include <stdint.h>

#define __MRC15(op1, crn, crm, op2, clobbers)                              \
	({                                                                     \
		register uint32_t __val;                                           \
		__asm__ __volatile__("mrc p15," #op1 ",%0," #crn "," #crm "," #op2 \
		                     : "=r"(__val)                                 \
		                     :                                             \
		                     : clobbers);                                  \
		__val;                                                             \
	})

#define __MCR15(op1, crn, crm, op2, clobbers, val)                         \
	({                                                                     \
		register uint32_t __val = val;                                     \
		__asm__ __volatile__("mcr p15," #op1 ",%0," #crn "," #crm "," #op2 \
		                     :                                             \
		                     : "r"(__val)                                  \
		                     : clobbers);                                  \
	})

/** [RO] Main ID Register */
#define MIDR                              MIDR
#define __ARCH_REG_LOAD__MIDR()           __MRC15(0, c0, c0, 0, )

/** [RO] Cache Type Register */
#define CTR                               CTR
#define __ARCH_REG_LOAD__CTR()            __MRC15(0, c0, c0, 1, )

/** [RO] TCM Type Register */
#define TCMTR                             TCMTR
#define __ARCH_REG_LOAD__TCMTR()          __MRC15(0, c0, c0, 2, )

/** [RO] TLB Type Register */
#define TLBTR                             TLBTR
#define __ARCH_REG_LOAD__TLBTR()          __MRC15(0, c0, c0, 3, )

/** [RO] Multiprocessor Affinity Register */
#define MPIDR                             MPIDR
#define __ARCH_REG_LOAD__MPIDR()          __MRC15(0, c0, c0, 5, )

/** [RO] Revision ID Register */
#define REVIDR                            REVIDR
#define __ARCH_REG_LOAD__REVIDR()         __MRC15(0, c0, c0, 6, )

/** [RO] Processor Feature Register 0 */
#define PFR0                              PFR0
#define __ARCH_REG_LOAD__PFR0()           __MRC15(0, c0, c1, 0, )

/** [RO] Processor Feature Register 1 */
#define PFR1                              PFR1
#define __ARCH_REG_LOAD__PFR1()           __MRC15(0, c0, c1, 1, )

/** [RO] Debug Feature Register 0 */
#define DFR0                              DFR0
#define __ARCH_REG_LOAD__DFR0()           __MRC15(0, c0, c1, 2, )

/** [RO] Auxiliary Feature Register 0 */
#define AFR0                              AFR0
#define __ARCH_REG_LOAD__AFR0()           __MRC15(0, c0, c1, 3, )

/** [RO] Memory Model Feature Register 0 */
#define MMFR0                             MMFR0
#define __ARCH_REG_LOAD__MMFR0()          __MRC15(0, c0, c1, 4, )

/** [RO] Memory Model Feature Register 1 */
#define MMFR1                             MMFR1
#define __ARCH_REG_LOAD__MMFR1()          __MRC15(0, c0, c1, 5, )

/** [RO] Memory Model Feature Register 2 */
#define MMFR2                             MMFR2
#define __ARCH_REG_LOAD__MMFR2()          __MRC15(0, c0, c1, 6, )

/** [RO] Memory Model Feature Register 3 */
#define MMFR3                             MMFR3
#define __ARCH_REG_LOAD__MMFR3()          __MRC15(0, c0, c1, 7, )

/** [RO] Instruction Set Attribute Register 0 */
#define ISAR0                             ISAR0
#define __ARCH_REG_LOAD__ISAR0()          __MRC15(0, c0, c2, 0, )

/** [RO] Instruction Set Attribute Register 1 */
#define ISAR1                             ISAR1
#define __ARCH_REG_LOAD__ISAR1()          __MRC15(0, c0, c2, 1, )

/** [RO] Instruction Set Attribute Register 2 */
#define ISAR2                             ISAR2
#define __ARCH_REG_LOAD__ISAR2()          __MRC15(0, c0, c2, 2, )

/** [RO] Instruction Set Attribute Register 3 */
#define ISAR3                             ISAR3
#define __ARCH_REG_LOAD__ISAR3()          __MRC15(0, c0, c2, 3, )

/** [RO] Instruction Set Attribute Register 4 */
#define ISAR4                             ISAR4
#define __ARCH_REG_LOAD__ISAR4()          __MRC15(0, c0, c2, 4, )

/** [RO] Instruction Set Attribute Register 5 */
#define ISAR5                             ISAR5
#define __ARCH_REG_LOAD__ISAR5()          __MRC15(0, c0, c2, 5, )

/** [RO] Cache Size ID Registers */
#define CCSIDR                            CCSIDR
#define __ARCH_REG_LOAD__CCSIDR()         __MRC15(1, c0, c0, 0, )

/** [RO] Cache Level ID Register */
#define CLIDR                             CLIDR
#define __ARCH_REG_LOAD__CLIDR()          __MRC15(1, c0, c0, 1, )

/** [RO] Auxiliary ID Register */
#define AIDR                              AIDR
#define __ARCH_REG_LOAD__AIDR()           __MRC15(1, c0, c0, 7, )

/** Cache Size Selection Register */
#define CSSELR                            CSSELR
#define __ARCH_REG_LOAD__CSSELR()         __MRC15(2, c0, c0, 0, )
#define __ARCH_REG_STORE__CSSELR(val)     __MCR15(2, c0, c0, 0, , val)

/** Virtualization Processor ID Register */
#define VPIDR                             VPIDR
#define __ARCH_REG_LOAD__VPIDR()          __MRC15(4, c0, c0, 0, )
#define __ARCH_REG_STORE__VPIDR(val)      __MCR15(4, c0, c0, 0, , val)

/** Virtualization Processor ID Register */
#define VPIDR                             VPIDR
#define __ARCH_REG_LOAD__VPIDR()          __MRC15(4, c0, c0, 0, )
#define __ARCH_REG_STORE__VPIDR(val)      __MCR15(4, c0, c0, 0, , val)

/** System Control Register */
#define SCTLR                             SCTLR
#define __ARCH_REG_LOAD__SCTLR()          __MRC15(0, c1, c0, 0, "cc")
#define __ARCH_REG_STORE__SCTLR(val)      __MCR15(0, c1, c0, 0, "cc", val)

#define SCTLR_M                           (1U << 0) /* MMU enabled */
#define SCTLR_A                           (1U << 1) /* Strict alignment fault checking enabled */
#define SCTLR_C                           (1U << 2) /* Data caching enabled */
#define SCTLR_W                           (1U << 3) /* Write buffer enable */
#define SCTLR_P                           (1U << 4) /* 32-bit exception handler */
#define SCTLR_D                           (1U << 5) /* 32-bit data address range */
#define SCTLR_L                           (1U << 6) /* Implementation defined */
#define SCTLR_B                           (1U << 7) /* Big endian */
#define SCTLR_S                           (1U << 8) /* System MMU protection */
#define SCTLR_R                           (1U << 9) /* ROM MMU protection */
#define SCTLR_F                           (1U << 10) /* Implementation defined */
#define SCTLR_Z                           (1U << 11) /* program flow prediction enabled */
#define SCTLR_I                           (1U << 12) /* instruction caching enabled */
#define SCTLR_V                           (1U << 13) /* Vectors relocated to 0xffff0000 */
#define SCTLR_RR                          (1U << 14) /* Round Robin cache replacement */
#define SCTLR_EE                          (1U << 25) /* Exception (Big) Endian */
#define SCTLR_TRE                         (1U << 28) /* TEX remap enable */
#define SCTLR_AFE                         (1U << 29) /* Access flag enable */
#define SCTLR_TE                          (1U << 30) /* Thumb exception enable */

/** Auxiliary Control Register */
#define ACTLR                             ACTLR
#define __ARCH_REG_LOAD__ACTLR()          __MRC15(0, c1, c0, 1, "cc")
#define __ARCH_REG_STORE__ACTLR(val)      __MCR15(0, c1, c0, 1, "cc", val)

#define ACTLR_L2EN                        (1U << 1) /* L2 prefetch enabled */
#define ACTLR_L1EN                        (1U << 2) /* L1 prefetch enabled */

/** Coprocessor Access Control Register */
#define CPACR                             CPACR
#define __ARCH_REG_LOAD__CPACR()          __MRC15(0, c1, c0, 2, "cc")
#define __ARCH_REG_STORE__CPACR(val)      __MCR15(0, c1, c0, 2, "cc", val)

/** Secure Configuration Register */
#define SCR                               SCR
#define __ARCH_REG_LOAD__SCR()            __MRC15(0, c1, c1, 0, "cc")
#define __ARCH_REG_STORE__SCR(val)        __MCR15(0, c1, c1, 0, "cc", val)

/** Secure Debug Enable Register */
#define SDER                              SDER
#define __ARCH_REG_LOAD__SDER()           __MRC15(0, c1, c1, 1, "cc")
#define __ARCH_REG_STORE__SDER(val)       __MCR15(0, c1, c1, 1, "cc", val)

/** Non-Secure Access Control Register */
#define NSACR                             NSACR
#define __ARCH_REG_LOAD__NSACR()          __MRC15(0, c1, c1, 2, "cc")
#define __ARCH_REG_STORE__NSACR(val)      __MCR15(0, c1, c1, 2, "cc", val)

/** Virtualization Control Register */
#define VCR                               VCR
#define __ARCH_REG_LOAD__VCR()            __MRC15(0, c1, c1, 3, )
#define __ARCH_REG_STORE__VCR(val)        __MCR15(0, c1, c1, 3, , val)

/** Hyp System Control Register */
#define HSCTLR                            HSCTLR
#define __ARCH_REG_LOAD__HSCTLR()         __MRC15(4, c1, c0, 0, )
#define __ARCH_REG_STORE__HSCTLR(val)     __MCR15(4, c1, c0, 0, , val)

/** Hyp Auxiliary Control Register */
#define HACTLR                            HACTLR
#define __ARCH_REG_LOAD__HACTLR()         __MRC15(4, c1, c0, 1, )
#define __ARCH_REG_STORE__HACTLR(val)     __MCR15(4, c1, c0, 1, , val)

/** Hyp Configuration Register */
#define HCR                               HCR
#define __ARCH_REG_LOAD__HCR()            __MRC15(4, c1, c1, 0, )
#define __ARCH_REG_STORE__HCR(val)        __MCR15(4, c1, c1, 0, , val)

/** Hyp Debug Configuration Register */
#define HDCR                              HDCR
#define __ARCH_REG_LOAD__HDCR()           __MRC15(4, c1, c1, 1, )
#define __ARCH_REG_STORE__HDCR(val)       __MCR15(4, c1, c1, 1, , val)

/** Hyp Coprocessor Trap Register */
#define HCPTR                             HCPTR
#define __ARCH_REG_LOAD__HCPTR()          __MRC15(4, c1, c1, 2, )
#define __ARCH_REG_STORE__HCPTR(val)      __MCR15(4, c1, c1, 2, , val)

/** Hyp System Trap Register */
#define HSTR                              HSTR
#define __ARCH_REG_LOAD__HSTR()           __MRC15(4, c1, c1, 3, )
#define __ARCH_REG_STORE__HSTR(val)       __MCR15(4, c1, c1, 3, , val)

/** Hyp Auxiliary Configuration Register */
#define HACR                              HACR
#define __ARCH_REG_LOAD__HACR()           __MRC15(4, c1, c1, 7, )
#define __ARCH_REG_STORE__HACR(val)       __MCR15(4, c1, c1, 7, , val)

/** Translation Table Base Register 0 */
#define TTBR0                             TTBR0
#define __ARCH_REG_LOAD__TTBR0()          __MRC15(0, c2, c0, 0, )
#define __ARCH_REG_STORE__TTBR0(val)      __MCR15(0, c2, c0, 0, , val)

#define TTBR0_ADDR_MASK                   0xFFFFFF00

/** Translation Table Base Register 1 */
#define TTBR1                             TTBR1
#define __ARCH_REG_LOAD__TTBR1()          __MRC15(0, c2, c0, 1, )
#define __ARCH_REG_STORE__TTBR1(val)      __MCR15(0, c2, c0, 1, , val)

/** Translation Table Base Control Register */
#define TTBCR                             TTBCR
#define __ARCH_REG_LOAD__TTBCR()          __MRC15(0, c2, c0, 2, )
#define __ARCH_REG_STORE__TTBCR(val)      __MCR15(0, c2, c0, 2, , val)

#define TTBCR_PAE_SUPPORT                 0x80000000

/** Hyp Translation Control Register */
#define HTCR                              HTCR
#define __ARCH_REG_LOAD__HTCR()           __MRC15(4, c2, c0, 2, )
#define __ARCH_REG_STORE__HTCR(val)       __MCR15(4, c2, c0, 2, , val)

/** Virtualization Translation Control Register */
#define VTCR                              VTCR
#define __ARCH_REG_LOAD__VTCR()           __MRC15(4, c2, c1, 2, )
#define __ARCH_REG_STORE__VTCR(val)       __MCR15(4, c2, c1, 2, , val)

/** Domain Access Control Register */
#define DACR                              DACR
#define __ARCH_REG_LOAD__DACR()           __MRC15(0, c3, c0, 0, )
#define __ARCH_REG_STORE__DACR(val)       __MCR15(0, c3, c0, 0, , val)

/** Data Fault Status Register */
#define DFSR                              DFSR
#define __ARCH_REG_LOAD__DFSR()           __MRC15(0, c5, c0, 0, )
#define __ARCH_REG_STORE__DFSR(val)       __MCR15(0, c5, c0, 0, , val)

/** Instruction Fault Status Register */
#define IFSR                              IFSR
#define __ARCH_REG_LOAD__IFSR()           __MRC15(0, c5, c0, 1, )
#define __ARCH_REG_STORE__IFSR(val)       __MCR15(0, c5, c0, 1, , val)

/** Auxiliary Data Fault Status Register */
#define ADFSR                             ADFSR
#define __ARCH_REG_LOAD__ADFSR()          __MRC15(0, c5, c1, 0, )
#define __ARCH_REG_STORE__ADFSR(val)      __MCR15(0, c5, c1, 0, , val)

/** Auxiliary Instruction Fault Status Register */
#define AIFSR                             AIFSR
#define __ARCH_REG_LOAD__AIFSR()          __MRC15(0, c5, c1, 1, )
#define __ARCH_REG_STORE__AIFSR(val)      __MCR15(0, c5, c1, 1, , val)

/** Hyp Auxiliary Data Fault Syndrome Register */
#define HADFSR                            HADFSR
#define __ARCH_REG_LOAD__HADFSR()         __MRC15(4, c5, c1, 0, )
#define __ARCH_REG_STORE__HADFSR(val)     __MCR15(4, c5, c1, 0, , val)

/** Hyp Auxiliary Instruction Fault Syndrome Register */
#define HAIFSR                            HAIFSR
#define __ARCH_REG_LOAD__HAIFSR()         __MRC15(4, c5, c1, 1, )
#define __ARCH_REG_STORE__HAIFSR(val)     __MCR15(4, c5, c1, 1, , val)

/** Hyp Syndrome Register */
#define HSR                               HSR
#define __ARCH_REG_LOAD__HSR()            __MRC15(4, c5, c2, 0, )
#define __ARCH_REG_STORE__HSR(val)        __MCR15(4, c5, c2, 0, , val)

/** Data Fault Address Register */
#define DFAR                              DFAR
#define __ARCH_REG_LOAD__DFAR()           __MRC15(0, c6, c0, 0, )
#define __ARCH_REG_STORE__DFAR(val)       __MCR15(0, c6, c0, 0, , val)

/** Instruction Fault Address Register */
#define IFAR                              IFAR
#define __ARCH_REG_LOAD__IFAR()           __MRC15(0, c6, c0, 2, )
#define __ARCH_REG_STORE__IFAR(val)       __MCR15(0, c6, c0, 2, , val)

/** Hyp Data Fault Address Register */
#define HDFAR                             HDFAR
#define __ARCH_REG_LOAD__HDFAR()          __MRC15(4, c6, c0, 0, )
#define __ARCH_REG_STORE__HDFAR(val)      __MCR15(4, c6, c0, 0, , val)

/** Hyp Instruction Fault Address Register */
#define HIFAR                             HIFAR
#define __ARCH_REG_LOAD__HIFAR()          __MRC15(4, c6, c0, 2, )
#define __ARCH_REG_STORE__HIFAR(val)      __MCR15(4, c6, c0, 2, , val)

/** Hyp IPA Fault Address Register */
#define HPFAR                             HPFAR
#define __ARCH_REG_LOAD__HPFAR()          __MRC15(4, c6, c0, 4, )
#define __ARCH_REG_STORE__HPFAR(val)      __MCR15(4, c6, c0, 4, , val)

/** Hyp IPA Fault Address Register */
#define HPFAR                             HPFAR
#define __ARCH_REG_LOAD__HPFAR()          __MRC15(4, c6, c0, 4, )
#define __ARCH_REG_STORE__HPFAR(val)      __MCR15(4, c6, c0, 4, , val)

/** [WO] Wait For Interrupt */
#define WFI                               WFI
#define __ARCH_REG_STORE__WFI(val)        __MCR15(0, c7, c0, 4, , val)

/** [WO] Invalidate all instruction caches Inner Shareable to PoU */
#define ICIALLUIS                         ICIALLUIS
#define __ARCH_REG_STORE__ICIALLUIS(val)  __MCR15(0, c7, c1, 0, "memory", val)

/** [WO] Invalidate all entries from branch predictors Inner Shareable */
#define BPIALLIS                          BPIALLIS
#define __ARCH_REG_STORE__BPIALLIS(val)   __MCR15(0, c7, c1, 6, "memory", val)

/** Physical Address Register */
#define PAR                               PAR
#define __ARCH_REG_LOAD__PAR()            __MRC15(0, c7, c4, 0, )
#define __ARCH_REG_STORE__PAR(val)        __MCR15(0, c7, c4, 0, , val)

/** [WO] Invalidate all instruction caches to PoU */
#define ICIALLU                           ICIALLU
#define __ARCH_REG_STORE__ICIALLU(val)    __MCR15(0, c7, c5, 0, "memory", val)

/** [WO] Invalidate instruction cache line by MVA to PoU */
#define ICIMVAU                           ICIMVAU
#define __ARCH_REG_STORE__ICIMVAU(val)    __MCR15(0, c7, c5, 1, "memory", val)

/** [WO] Invalidate all entries from branch predictors */
#define BPIALL                            BPIALL
#define __ARCH_REG_STORE__BPIALL(val)     __MCR15(0, c7, c5, 6, "memory", val)

/** [WO] Invalidate data or unified cache line by MVA to PoC */
#define DCIMVAC                           DCIMVAC
#define __ARCH_REG_STORE__DCIMVAC(val)    __MCR15(0, c7, c6, 1, "memory", val)

/** [WO] Invalidate data or unified cache line by set/way */
#define DCISW                             DCISW
#define __ARCH_REG_STORE__DCISW(val)      __MCR15(0, c7, c6, 2, "memory", val)

/** [WO] Clean data or unified cache line by MVA to PoC */
#define DCCMVAC                           DCCMVAC
#define __ARCH_REG_STORE__DCCMVAC(val)    __MCR15(0, c7, c10, 1, "memory", val)

/** [WO] Clean data or unified cache line by set/way */
#define DCCSW                             DCCSW
#define __ARCH_REG_STORE__DCCSW(val)      __MCR15(0, c7, c10, 2, "memory", val)

/** [WO] Clean data or unified cache line by MVA to PoU */
#define DCCMVAU                           DCCMVAU
#define __ARCH_REG_STORE__DCCMVAU(val)    __MCR15(0, c7, c11, 1, "memory", val)

/** [WO] Clean and Invalidate data or unified cache line by MVA to PoC */
#define DCCIMVAC                          DCCIMVAC
#define __ARCH_REG_STORE__DCCIMVAC(val)   __MCR15(0, c7, c14, 1, "memory", val)

/** [WO] Clean and Invalidate data or unified cache line by set/way */
#define DCCISW                            DCCISW
#define __ARCH_REG_STORE__DCCISW(val)     __MCR15(0, c7, c14, 2, "memory", val)

/** TLB Lockdown Register */
#define TLBLR                             TLBLR
#define __ARCH_REG_LOAD__TLBLR()          __MRC15(0, c10, c0, 0, )
#define __ARCH_REG_STORE__TLBLR(val)      __MCR15(0, c10, c0, 0, , val)

/** Primary Region Remap Register */
#define PRRR                              PRRR
#define __ARCH_REG_LOAD__PRRR()           __MRC15(0, c10, c2, 0, )
#define __ARCH_REG_STORE__PRRR(val)       __MCR15(0, c10, c2, 0, , val)

/** Normal Memory Remap Register */
#define NRRR                              NRRR
#define __ARCH_REG_LOAD__NRRR()           __MRC15(0, c10, c2, 1, )
#define __ARCH_REG_STORE__NRRR(val)       __MCR15(0, c10, c2, 1, , val)

/** PLE ID Register */
#define PLEIDR                            PLEIDR
#define __ARCH_REG_LOAD__PLEIDR()         __MRC15(0, c11, c0, 0, )
#define __ARCH_REG_STORE__PLEIDR(val)     __MCR15(0, c11, c0, 0, , val)

/** PLE Activity Status Register */
#define PLEASR                            PLEASR
#define __ARCH_REG_LOAD__PLEASR()         __MRC15(0, c11, c0, 2, )
#define __ARCH_REG_STORE__PLEASR(val)     __MCR15(0, c11, c0, 2, , val)

/** PLE FIFO Status Register */
#define PLEFSR                            PLEFSR
#define __ARCH_REG_LOAD__PLEFSR()         __MRC15(0, c11, c0, 4, )
#define __ARCH_REG_STORE__PLEFSR(val)     __MCR15(0, c11, c0, 4, , val)

/** Preload Engine User Accessibility Register */
#define PLEUAR                            PLEUAR
#define __ARCH_REG_LOAD__PLEUAR()         __MRC15(0, c11, c1, 0, )
#define __ARCH_REG_STORE__PLEUAR(val)     __MCR15(0, c11, c1, 0, , val)

/** Preload Engine Parameters Control Register */
#define PLEPCR                            PLEPCR
#define __ARCH_REG_LOAD__PLEPCR()         __MRC15(0, c11, c1, 1, )
#define __ARCH_REG_STORE__PLEPCR(val)     __MCR15(0, c11, c1, 1, , val)

/** Vector Base Address Register */
#define VBAR                              VBAR
#define __ARCH_REG_LOAD__VBAR()           __MRC15(0, c12, c0, 0, "cc")
#define __ARCH_REG_STORE__VBAR(val)       __MCR15(0, c12, c0, 0, "cc", val)

/** Monitor Vector Base Address Register */
#define MVBAR                             MVBAR
#define __ARCH_REG_LOAD__MVBAR()          __MRC15(0, c12, c0, 1, "cc")
#define __ARCH_REG_STORE__MVBAR(val)      __MCR15(0, c12, c0, 1, "cc", val)

/** [RO] Interrupt Status Register */
#define ISR                               ISR
#define __ARCH_REG_LOAD__ISR()            __MRC15(0, c12, c1, 0, )

/** Virtualization Interrupt Register */
#define VIR                               VIR
#define __ARCH_REG_LOAD__VIR()            __MRC15(0, c12, c1, 1, )
#define __ARCH_REG_STORE__VIR(val)        __MCR15(0, c12, c1, 1, , val)

/** Hyp Vector Base Address Register */
#define HVBAR                             HVBAR
#define __ARCH_REG_LOAD__HVBAR()          __MRC15(4, c12, c0, 0, )
#define __ARCH_REG_STORE__HVBAR(val)      __MCR15(4, c12, c0, 0, , val)

/** FCSE Process ID Register */
#define FCSEIDR                           FCSEIDR
#define __ARCH_REG_LOAD__FCSEIDR()        __MRC15(0, c13, c0, 0, )
#define __ARCH_REG_STORE__FCSEIDR(val)    __MCR15(0, c13, c0, 0, , val)

/** Context ID Register */
#define CONTEXTIDR                        CONTEXTIDR
#define __ARCH_REG_LOAD__CONTEXTIDR()     __MRC15(0, c13, c0, 1, )
#define __ARCH_REG_STORE__CONTEXTIDR(val) __MCR15(0, c13, c0, 1, , val)

/** User Read/Write Thread ID Register */
#define TPIDRURW                          TPIDRURW
#define __ARCH_REG_LOAD__TPIDRURW()       __MRC15(0, c13, c0, 2, )
#define __ARCH_REG_STORE__TPIDRURW(val)   __MCR15(0, c13, c0, 2, , val)

/** User Read-Only Thread ID Register */
#define TPIDRURO                          TPIDRURO
#define __ARCH_REG_LOAD__TPIDRURO()       __MRC15(0, c13, c0, 3, )
#define __ARCH_REG_STORE__TPIDRURO(val)   __MCR15(0, c13, c0, 3, , val)

/** PL1 only Thread ID Register */
#define TPIDRPRW                          TPIDRPRW
#define __ARCH_REG_LOAD__TPIDRPRW()       __MRC15(0, c13, c0, 4, )
#define __ARCH_REG_STORE__TPIDRPRW(val)   __MCR15(0, c13, c0, 4, , val)

/** Hyp Software Thread ID Register */
#define HTPIDR                            HTPIDR
#define __ARCH_REG_LOAD__HTPIDR()         __MRC15(4, c13, c0, 2, )
#define __ARCH_REG_STORE__HTPIDR(val)     __MCR15(4, c13, c0, 2, , val)

/** [WO] Select Lockdown TLB Entry for read */
#define SLTLBER                           SLTLBER
#define __ARCH_REG_STORE__SLTLBER(val)    __MCR15(5, c15, c4, 2, , val)

/** [WO] Select Lockdown TLB Entry for write */
#define SLTLBEW                           SLTLBEW
#define __ARCH_REG_STORE__SLTLBEW(val)    __MCR15(5, c15, c4, 4, , val)

/** Main TLB VA Register */
#define MTLBVAR                           MTLBVAR
#define __ARCH_REG_LOAD__MTLBVAR()        __MRC15(5, c15, c5, 2, )
#define __ARCH_REG_STORE__MTLBVAR(val)    __MCR15(5, c15, c5, 2, , val)

/** Main TLB PA Register */
#define MTLBPAR                           MTLBPAR
#define __ARCH_REG_LOAD__MTLBPAR()        __MRC15(5, c15, c6, 2, )
#define __ARCH_REG_STORE__MTLBPAR(val)    __MCR15(5, c15, c6, 2, , val)

/** Main TLB Attribute Register */
#define MTLBATR                           MTLBATR
#define __ARCH_REG_LOAD__MTLBATR()        __MRC15(5, c15, c7, 2, )
#define __ARCH_REG_STORE__MTLBATR(val)    __MCR15(5, c15, c7, 2, , val)

#endif /* ARMLIB_REG_CP15_H_ */
