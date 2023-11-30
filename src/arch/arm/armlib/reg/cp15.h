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

#define MIDR                              /* [RO] Main ID Register */
#define __ARCH_REG_LOAD__MIDR()           __MRC15(0, c0, c0, 0, )

#define CTR                               /* [RO] Cache Type Register */
#define __ARCH_REG_LOAD__CTR()            __MRC15(0, c0, c0, 1, )

#define TCMTR                             /* [RO] TCM Type Register */
#define __ARCH_REG_LOAD__TCMTR()          __MRC15(0, c0, c0, 2, )

#define TLBTR                             /* [RO] TLB Type Register */
#define __ARCH_REG_LOAD__TLBTR()          __MRC15(0, c0, c0, 3, )

#define MPIDR                             /* [RO] Multiprocessor Affinity Register */
#define __ARCH_REG_LOAD__MPIDR()          __MRC15(0, c0, c0, 5, )

#define REVIDR                            /* [RO] Revision ID Register */
#define __ARCH_REG_LOAD__REVIDR()         __MRC15(0, c0, c0, 6, )

#define PFR0                              /* [RO] Processor Feature Register 0 */
#define __ARCH_REG_LOAD__PFR0()           __MRC15(0, c0, c1, 0, )

#define PFR1                              /* [RO] Processor Feature Register 1 */
#define __ARCH_REG_LOAD__PFR1()           __MRC15(0, c0, c1, 1, )

#define DFR0                              /* [RO] Debug Feature Register 0 */
#define __ARCH_REG_LOAD__DFR0()           __MRC15(0, c0, c1, 2, )

#define AFR0                              /* [RO] Auxiliary Feature Register 0 */
#define __ARCH_REG_LOAD__AFR0()           __MRC15(0, c0, c1, 3, )

#define MMFR0                             /* [RO] Memory Model Feature Register 0 */
#define __ARCH_REG_LOAD__MMFR0()          __MRC15(0, c0, c1, 4, )

#define MMFR1                             /* [RO] Memory Model Feature Register 1 */
#define __ARCH_REG_LOAD__MMFR1()          __MRC15(0, c0, c1, 5, )

#define MMFR2                             /* [RO] Memory Model Feature Register 2 */
#define __ARCH_REG_LOAD__MMFR2()          __MRC15(0, c0, c1, 6, )

#define MMFR3                             /* [RO] Memory Model Feature Register 3 */
#define __ARCH_REG_LOAD__MMFR3()          __MRC15(0, c0, c1, 7, )

#define ISAR0                             /* [RO] Instruction Set Attribute Register 0 */
#define __ARCH_REG_LOAD__ISAR0()          __MRC15(0, c0, c2, 0, )

#define ISAR1                             /* [RO] Instruction Set Attribute Register 1 */
#define __ARCH_REG_LOAD__ISAR1()          __MRC15(0, c0, c2, 1, )

#define ISAR2                             /* [RO] Instruction Set Attribute Register 2 */
#define __ARCH_REG_LOAD__ISAR2()          __MRC15(0, c0, c2, 2, )

#define ISAR3                             /* [RO] Instruction Set Attribute Register 3 */
#define __ARCH_REG_LOAD__ISAR3()          __MRC15(0, c0, c2, 3, )

#define ISAR4                             /* [RO] Instruction Set Attribute Register 4 */
#define __ARCH_REG_LOAD__ISAR4()          __MRC15(0, c0, c2, 4, )

#define ISAR5                             /* [RO] Instruction Set Attribute Register 5 */
#define __ARCH_REG_LOAD__ISAR5()          __MRC15(0, c0, c2, 5, )

#define CCSIDR                            /* [RO] Cache Size ID Registers */
#define __ARCH_REG_LOAD__CCSIDR()         __MRC15(1, c0, c0, 0, )

#define CLIDR                             /* [RO] Cache Level ID Register */
#define __ARCH_REG_LOAD__CLIDR()          __MRC15(1, c0, c0, 1, )

#define AIDR                              /* [RO] Auxiliary ID Register */
#define __ARCH_REG_LOAD__AIDR()           __MRC15(1, c0, c0, 7, )

#define CSSELR                            /* Cache Size Selection Register */
#define __ARCH_REG_LOAD__CSSELR()         __MRC15(2, c0, c0, 0, )
#define __ARCH_REG_STORE__CSSELR(val)     __MCR15(2, c0, c0, 0, , val)

#define VPIDR                             /* Virtualization Processor ID Register */
#define __ARCH_REG_LOAD__VPIDR()          __MRC15(4, c0, c0, 0, )
#define __ARCH_REG_STORE__VPIDR(val)      __MCR15(4, c0, c0, 0, , val)

#define VPIDR                             /* Virtualization Processor ID Register */
#define __ARCH_REG_LOAD__VPIDR()          __MRC15(4, c0, c0, 0, )
#define __ARCH_REG_STORE__VPIDR(val)      __MCR15(4, c0, c0, 0, , val)

#define SCTLR                             /* System Control Register */
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

#define ACTLR                             /* Auxiliary Control Register */
#define __ARCH_REG_LOAD__ACTLR()          __MRC15(0, c1, c0, 1, "cc")
#define __ARCH_REG_STORE__ACTLR(val)      __MCR15(0, c1, c0, 1, "cc", val)

#define ACTLR_L2EN                        (1U << 1) /* L2 prefetch enabled */
#define ACTLR_L1EN                        (1U << 2) /* L1 prefetch enabled */

#define CPACR                             /* Coprocessor Access Control Register */
#define __ARCH_REG_LOAD__CPACR()          __MRC15(0, c1, c0, 2, "cc")
#define __ARCH_REG_STORE__CPACR(val)      __MCR15(0, c1, c0, 2, "cc", val)

#define SCR                               /* Secure Configuration Register */
#define __ARCH_REG_LOAD__SCR()            __MRC15(0, c1, c1, 0, "cc")
#define __ARCH_REG_STORE__SCR(val)        __MCR15(0, c1, c1, 0, "cc", val)

#define SDER                              /* Secure Debug Enable Register */
#define __ARCH_REG_LOAD__SDER()           __MRC15(0, c1, c1, 1, "cc")
#define __ARCH_REG_STORE__SDER(val)       __MCR15(0, c1, c1, 1, "cc", val)

#define NSACR                             /* Non-Secure Access Control Register */
#define __ARCH_REG_LOAD__NSACR()          __MRC15(0, c1, c1, 2, "cc")
#define __ARCH_REG_STORE__NSACR(val)      __MCR15(0, c1, c1, 2, "cc", val)

#define VCR                               /* Virtualization Control Register */
#define __ARCH_REG_LOAD__VCR()            __MRC15(0, c1, c1, 3, )
#define __ARCH_REG_STORE__VCR(val)        __MCR15(0, c1, c1, 3, , val)

#define HSCTLR                            /* Hyp System Control Register */
#define __ARCH_REG_LOAD__HSCTLR()         __MRC15(4, c1, c0, 0, )
#define __ARCH_REG_STORE__HSCTLR(val)     __MCR15(4, c1, c0, 0, , val)

#define HACTLR                            /* Hyp Auxiliary Control Register */
#define __ARCH_REG_LOAD__HACTLR()         __MRC15(4, c1, c0, 1, )
#define __ARCH_REG_STORE__HACTLR(val)     __MCR15(4, c1, c0, 1, , val)

#define HCR                               /* Hyp Configuration Register */
#define __ARCH_REG_LOAD__HCR()            __MRC15(4, c1, c1, 0, )
#define __ARCH_REG_STORE__HCR(val)        __MCR15(4, c1, c1, 0, , val)

#define HDCR                              /* Hyp Debug Configuration Register */
#define __ARCH_REG_LOAD__HDCR()           __MRC15(4, c1, c1, 1, )
#define __ARCH_REG_STORE__HDCR(val)       __MCR15(4, c1, c1, 1, , val)

#define HCPTR                             /* Hyp Coprocessor Trap Register */
#define __ARCH_REG_LOAD__HCPTR()          __MRC15(4, c1, c1, 2, )
#define __ARCH_REG_STORE__HCPTR(val)      __MCR15(4, c1, c1, 2, , val)

#define HSTR                              /* Hyp System Trap Register */
#define __ARCH_REG_LOAD__HSTR()           __MRC15(4, c1, c1, 3, )
#define __ARCH_REG_STORE__HSTR(val)       __MCR15(4, c1, c1, 3, , val)

#define HACR                              /* Hyp Auxiliary Configuration Register */
#define __ARCH_REG_LOAD__HACR()           __MRC15(4, c1, c1, 7, )
#define __ARCH_REG_STORE__HACR(val)       __MCR15(4, c1, c1, 7, , val)

#define TTBR0                             /* Translation Table Base Register 0 */
#define __ARCH_REG_LOAD__TTBR0()          __MRC15(0, c2, c0, 0, )
#define __ARCH_REG_STORE__TTBR0(val)      __MCR15(0, c2, c0, 0, , val)

#define TTBR0_ADDR_MASK                   0xFFFFFF00

#define TTBR1                             /* Translation Table Base Register 1 */
#define __ARCH_REG_LOAD__TTBR1()          __MRC15(0, c2, c0, 1, )
#define __ARCH_REG_STORE__TTBR1(val)      __MCR15(0, c2, c0, 1, , val)

#define TTBCR                             /* Translation Table Base Control Register */
#define __ARCH_REG_LOAD__TTBCR()          __MRC15(0, c2, c0, 2, )
#define __ARCH_REG_STORE__TTBCR(val)      __MCR15(0, c2, c0, 2, , val)

#define TTBCR_PAE_SUPPORT                 0x80000000

#define HTCR                              /* Hyp Translation Control Register */
#define __ARCH_REG_LOAD__HTCR()           __MRC15(4, c2, c0, 2, )
#define __ARCH_REG_STORE__HTCR(val)       __MCR15(4, c2, c0, 2, , val)

#define VTCR                              /* Virtualization Translation Control Register */
#define __ARCH_REG_LOAD__VTCR()           __MRC15(4, c2, c1, 2, )
#define __ARCH_REG_STORE__VTCR(val)       __MCR15(4, c2, c1, 2, , val)

#define DACR                              /* Domain Access Control Register */
#define __ARCH_REG_LOAD__DACR()           __MRC15(0, c3, c0, 0, )
#define __ARCH_REG_STORE__DACR(val)       __MCR15(0, c3, c0, 0, , val)

#define DFSR                              /* Data Fault Status Register */
#define __ARCH_REG_LOAD__DFSR()           __MRC15(0, c5, c0, 0, )
#define __ARCH_REG_STORE__DFSR(val)       __MCR15(0, c5, c0, 0, , val)

#define IFSR                              /* Instruction Fault Status Register */
#define __ARCH_REG_LOAD__IFSR()           __MRC15(0, c5, c0, 1, )
#define __ARCH_REG_STORE__IFSR(val)       __MCR15(0, c5, c0, 1, , val)

#define ADFSR                             /* Auxiliary Data Fault Status Register */
#define __ARCH_REG_LOAD__ADFSR()          __MRC15(0, c5, c1, 0, )
#define __ARCH_REG_STORE__ADFSR(val)      __MCR15(0, c5, c1, 0, , val)

#define AIFSR                             /* Auxiliary Instruction Fault Status Register */
#define __ARCH_REG_LOAD__AIFSR()          __MRC15(0, c5, c1, 1, )
#define __ARCH_REG_STORE__AIFSR(val)      __MCR15(0, c5, c1, 1, , val)

#define HADFSR                            /* Hyp Auxiliary Data Fault Syndrome Register */
#define __ARCH_REG_LOAD__HADFSR()         __MRC15(4, c5, c1, 0, )
#define __ARCH_REG_STORE__HADFSR(val)     __MCR15(4, c5, c1, 0, , val)

#define HAIFSR                            /* Hyp Auxiliary Instruction Fault Syndrome Register */
#define __ARCH_REG_LOAD__HAIFSR()         __MRC15(4, c5, c1, 1, )
#define __ARCH_REG_STORE__HAIFSR(val)     __MCR15(4, c5, c1, 1, , val)

#define HSR                               /* Hyp Syndrome Register */
#define __ARCH_REG_LOAD__HSR()            __MRC15(4, c5, c2, 0, )
#define __ARCH_REG_STORE__HSR(val)        __MCR15(4, c5, c2, 0, , val)

#define DFAR                              /* Data Fault Address Register */
#define __ARCH_REG_LOAD__DFAR()           __MRC15(0, c6, c0, 0, )
#define __ARCH_REG_STORE__DFAR(val)       __MCR15(0, c6, c0, 0, , val)

#define IFAR                              /* Instruction Fault Address Register */
#define __ARCH_REG_LOAD__IFAR()           __MRC15(0, c6, c0, 2, )
#define __ARCH_REG_STORE__IFAR(val)       __MCR15(0, c6, c0, 2, , val)

#define HDFAR                             /* Hyp Data Fault Address Register */
#define __ARCH_REG_LOAD__HDFAR()          __MRC15(4, c6, c0, 0, )
#define __ARCH_REG_STORE__HDFAR(val)      __MCR15(4, c6, c0, 0, , val)

#define HIFAR                             /* Hyp Instruction Fault Address Register */
#define __ARCH_REG_LOAD__HIFAR()          __MRC15(4, c6, c0, 2, )
#define __ARCH_REG_STORE__HIFAR(val)      __MCR15(4, c6, c0, 2, , val)

#define HPFAR                             /* Hyp IPA Fault Address Register */
#define __ARCH_REG_LOAD__HPFAR()          __MRC15(4, c6, c0, 4, )
#define __ARCH_REG_STORE__HPFAR(val)      __MCR15(4, c6, c0, 4, , val)

#define HPFAR                             /* Hyp IPA Fault Address Register */
#define __ARCH_REG_LOAD__HPFAR()          __MRC15(4, c6, c0, 4, )
#define __ARCH_REG_STORE__HPFAR(val)      __MCR15(4, c6, c0, 4, , val)

#define WFI                               /* [WO] Wait For Interrupt */
#define __ARCH_REG_STORE__WFI(val)        __MCR15(0, c7, c0, 4, , val)

#define ICIALLUIS                         /* [WO] Invalidate all instruction caches Inner Shareable to PoU */
#define __ARCH_REG_STORE__ICIALLUIS(val)  __MCR15(0, c7, c1, 0, "memory", val)

#define BPIALLIS                          /* [WO] Invalidate all entries from branch predictors Inner Shareable */
#define __ARCH_REG_STORE__BPIALLIS(val)   __MCR15(0, c7, c1, 6, "memory", val)

#define PAR                               /* Physical Address Register */
#define __ARCH_REG_LOAD__PAR()            __MRC15(0, c7, c4, 0, )
#define __ARCH_REG_STORE__PAR(val)        __MCR15(0, c7, c4, 0, , val)

#define ICIALLU                           /* [WO] Invalidate all instruction caches to PoU */
#define __ARCH_REG_STORE__ICIALLU(val)    __MCR15(0, c7, c5, 0, "memory", val)

#define ICIMVAU                           /* [WO] Invalidate instruction cache line by MVA to PoU */
#define __ARCH_REG_STORE__ICIMVAU(val)    __MCR15(0, c7, c5, 1, "memory", val)

#define BPIALL                            /* [WO] Invalidate all entries from branch predictors */
#define __ARCH_REG_STORE__BPIALL(val)     __MCR15(0, c7, c5, 6, "memory", val)

#define DCIMVAC                           /* [WO] Invalidate data or unified cache line by MVA to PoC */
#define __ARCH_REG_STORE__DCIMVAC(val)    __MCR15(0, c7, c6, 1, "memory", val)

#define DCISW                             /* [WO] Invalidate data or unified cache line by set/way */
#define __ARCH_REG_STORE__DCISW(val)      __MCR15(0, c7, c6, 2, "memory", val)

#define DCCMVAC                           /* [WO] Clean data or unified cache line by MVA to PoC */
#define __ARCH_REG_STORE__DCCMVAC(val)    __MCR15(0, c7, c10, 1, "memory", val)

#define DCCSW                             /* [WO] Clean data or unified cache line by set/way */
#define __ARCH_REG_STORE__DCCSW(val)      __MCR15(0, c7, c10, 2, "memory", val)

#define DCCMVAU                           /* [WO] Clean data or unified cache line by MVA to PoU */
#define __ARCH_REG_STORE__DCCMVAU(val)    __MCR15(0, c7, c11, 1, "memory", val)

#define DCCIMVAC                          /* [WO] Clean and Invalidate data or unified cache line by MVA to PoC */
#define __ARCH_REG_STORE__DCCIMVAC(val)   __MCR15(0, c7, c14, 1, "memory", val)

#define DCCISW                            /* [WO] Clean and Invalidate data or unified cache line by set/way */
#define __ARCH_REG_STORE__DCCISW(val)     __MCR15(0, c7, c14, 2, "memory", val)

#define TLBLR                             /* TLB Lockdown Register */
#define __ARCH_REG_LOAD__TLBLR()          __MRC15(0, c10, c0, 0, )
#define __ARCH_REG_STORE__TLBLR(val)      __MCR15(0, c10, c0, 0, , val)

#define PRRR                              /* Primary Region Remap Register */
#define __ARCH_REG_LOAD__PRRR()           __MRC15(0, c10, c2, 0, )
#define __ARCH_REG_STORE__PRRR(val)       __MCR15(0, c10, c2, 0, , val)

#define NRRR                              /* Normal Memory Remap Register */
#define __ARCH_REG_LOAD__NRRR()           __MRC15(0, c10, c2, 1, )
#define __ARCH_REG_STORE__NRRR(val)       __MCR15(0, c10, c2, 1, , val)

#define PLEIDR                            /* PLE ID Register */
#define __ARCH_REG_LOAD__PLEIDR()         __MRC15(0, c11, c0, 0, )
#define __ARCH_REG_STORE__PLEIDR(val)     __MCR15(0, c11, c0, 0, , val)

#define PLEASR                            /* PLE Activity Status Register */
#define __ARCH_REG_LOAD__PLEASR()         __MRC15(0, c11, c0, 2, )
#define __ARCH_REG_STORE__PLEASR(val)     __MCR15(0, c11, c0, 2, , val)

#define PLEFSR                            /* PLE FIFO Status Register */
#define __ARCH_REG_LOAD__PLEFSR()         __MRC15(0, c11, c0, 4, )
#define __ARCH_REG_STORE__PLEFSR(val)     __MCR15(0, c11, c0, 4, , val)

#define PLEUAR                            /* Preload Engine User Accessibility Register */
#define __ARCH_REG_LOAD__PLEUAR()         __MRC15(0, c11, c1, 0, )
#define __ARCH_REG_STORE__PLEUAR(val)     __MCR15(0, c11, c1, 0, , val)

#define PLEPCR                            /* Preload Engine Parameters Control Register */
#define __ARCH_REG_LOAD__PLEPCR()         __MRC15(0, c11, c1, 1, )
#define __ARCH_REG_STORE__PLEPCR(val)     __MCR15(0, c11, c1, 1, , val)

#define VBAR                              /* Vector Base Address Register */
#define __ARCH_REG_LOAD__VBAR()           __MRC15(0, c12, c0, 0, "cc")
#define __ARCH_REG_STORE__VBAR(val)       __MCR15(0, c12, c0, 0, "cc", val)

#define MVBAR                             /* Monitor Vector Base Address Register */
#define __ARCH_REG_LOAD__MVBAR()          __MRC15(0, c12, c0, 1, "cc")
#define __ARCH_REG_STORE__MVBAR(val)      __MCR15(0, c12, c0, 1, "cc", val)

#define ISR                               /* [RO] Interrupt Status Register */
#define __ARCH_REG_LOAD__ISR()            __MRC15(0, c12, c1, 0, )

#define VIR                               /* Virtualization Interrupt Register */
#define __ARCH_REG_LOAD__VIR()            __MRC15(0, c12, c1, 1, )
#define __ARCH_REG_STORE__VIR(val)        __MCR15(0, c12, c1, 1, , val)

#define HVBAR                             /* Hyp Vector Base Address Register */
#define __ARCH_REG_LOAD__HVBAR()          __MRC15(4, c12, c0, 0, )
#define __ARCH_REG_STORE__HVBAR(val)      __MCR15(4, c12, c0, 0, , val)

#define FCSEIDR                           /* FCSE Process ID Register */
#define __ARCH_REG_LOAD__FCSEIDR()        __MRC15(0, c13, c0, 0, )
#define __ARCH_REG_STORE__FCSEIDR(val)    __MCR15(0, c13, c0, 0, , val)

#define CONTEXTIDR                        /* Context ID Register */
#define __ARCH_REG_LOAD__CONTEXTIDR()     __MRC15(0, c13, c0, 1, )
#define __ARCH_REG_STORE__CONTEXTIDR(val) __MCR15(0, c13, c0, 1, , val)

#define TPIDRURW                          /* User Read/Write Thread ID Register */
#define __ARCH_REG_LOAD__TPIDRURW()       __MRC15(0, c13, c0, 2, )
#define __ARCH_REG_STORE__TPIDRURW(val)   __MCR15(0, c13, c0, 2, , val)

#define TPIDRURO                          /* User Read-Only Thread ID Register */
#define __ARCH_REG_LOAD__TPIDRURO()       __MRC15(0, c13, c0, 3, )
#define __ARCH_REG_STORE__TPIDRURO(val)   __MCR15(0, c13, c0, 3, , val)

#define TPIDRPRW                          /* PL1 only Thread ID Register */
#define __ARCH_REG_LOAD__TPIDRPRW()       __MRC15(0, c13, c0, 4, )
#define __ARCH_REG_STORE__TPIDRPRW(val)   __MCR15(0, c13, c0, 4, , val)

#define HTPIDR                            /* Hyp Software Thread ID Register */
#define __ARCH_REG_LOAD__HTPIDR()         __MRC15(4, c13, c0, 2, )
#define __ARCH_REG_STORE__HTPIDR(val)     __MCR15(4, c13, c0, 2, , val)

#define SLTLBER                           /* [WO] Select Lockdown TLB Entry for read */
#define __ARCH_REG_STORE__SLTLBER(val)    __MCR15(5, c15, c4, 2, , val)

#define SLTLBEW                           /* [WO] Select Lockdown TLB Entry for write */
#define __ARCH_REG_STORE__SLTLBEW(val)    __MCR15(5, c15, c4, 4, , val)

#define MTLBVAR                           /* Main TLB VA Register */
#define __ARCH_REG_LOAD__MTLBVAR()        __MRC15(5, c15, c5, 2, )
#define __ARCH_REG_STORE__MTLBVAR(val)    __MCR15(5, c15, c5, 2, , val)

#define MTLBPAR                           /* Main TLB PA Register */
#define __ARCH_REG_LOAD__MTLBPAR()        __MRC15(5, c15, c6, 2, )
#define __ARCH_REG_STORE__MTLBPAR(val)    __MCR15(5, c15, c6, 2, , val)

#define MTLBATR                           /* Main TLB Attribute Register */
#define __ARCH_REG_LOAD__MTLBATR()        __MRC15(5, c15, c7, 2, )
#define __ARCH_REG_STORE__MTLBATR(val)    __MCR15(5, c15, c7, 2, , val)

#endif /* ARMLIB_REG_CP15_H_ */
