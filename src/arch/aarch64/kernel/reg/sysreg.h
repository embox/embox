/**
 * @file
 * @brief
 *
 * @date 02.12.23
 * @author Aleksey Zhmulin
 */
#ifndef AARCH64_REG_SYSREG_H_
#define AARCH64_REG_SYSREG_H_

#include <stdint.h>

#define __MRS_SYS(reg)                                      \
	({                                                      \
		register uint64_t __val;                            \
		__asm__ __volatile__("mrs %0," #reg : "=r"(__val)); \
		__val;                                              \
	})

#define __MSR_SYS(reg, val)                                      \
	({                                                           \
		register uint64_t __val = val;                           \
		__asm__ __volatile__("msr " #reg ",%x0" : : "r"(__val)); \
		__asm__ __volatile__("isb");                             \
	})

/** Auxiliary Control Register (EL1) */
#define ACTLR_EL1                              ACTLR_EL1
#define __ARCH_REG_LOAD__ACTLR_EL1()           __MRS_SYS(ACTLR_EL1)
#define __ARCH_REG_STORE__ACTLR_EL1(val)       __MSR_SYS(ACTLR_EL1, val)

/** Auxiliary Control Register (EL2) */
#define ACTLR_EL2                              ACTLR_EL2
#define __ARCH_REG_LOAD__ACTLR_EL2()           __MRS_SYS(ACTLR_EL2)
#define __ARCH_REG_STORE__ACTLR_EL2(val)       __MSR_SYS(ACTLR_EL2, val)

/** Auxiliary Control Register (EL3) */
#define ACTLR_EL3                              ACTLR_EL3
#define __ARCH_REG_LOAD__ACTLR_EL3()           __MRS_SYS(ACTLR_EL3)
#define __ARCH_REG_STORE__ACTLR_EL3(val)       __MSR_SYS(ACTLR_EL3, val)

/** Auxiliary Fault Status Register 0 (EL1) */
#define AFSR0_EL1                              AFSR0_EL1
#define __ARCH_REG_LOAD__AFSR0_EL1()           __MRS_SYS(AFSR0_EL1)
#define __ARCH_REG_STORE__AFSR0_EL1(val)       __MSR_SYS(AFSR0_EL1, val)

/** Auxiliary Fault Status Register 0 (EL2) */
#define AFSR0_EL2                              AFSR0_EL2
#define __ARCH_REG_LOAD__AFSR0_EL2()           __MRS_SYS(AFSR0_EL1)
#define __ARCH_REG_STORE__AFSR0_EL2(val)       __MSR_SYS(AFSR0_EL1, val)

/** Auxiliary Fault Status Register 0 (EL3) */
#define AFSR0_EL3                              AFSR0_EL3
#define __ARCH_REG_LOAD__AFSR0_EL3()           __MRS_SYS(AFSR0_EL3)
#define __ARCH_REG_STORE__AFSR0_EL3(val)       __MSR_SYS(AFSR0_EL3, val)

/** Auxiliary Fault Status Register 1 (EL1) */
#define AFSR1_EL1                              AFSR1_EL1
#define __ARCH_REG_LOAD__AFSR1_EL1()           __MRS_SYS(AFSR1_EL1)
#define __ARCH_REG_STORE__AFSR1_EL1(val)       __MSR_SYS(AFSR1_EL1, val)

/** Auxiliary Fault Status Register 1 (EL2) */
#define AFSR1_EL2                              AFSR1_EL2
#define __ARCH_REG_LOAD__AFSR1_EL2()           __MRS_SYS(AFSR1_EL2)
#define __ARCH_REG_STORE__AFSR1_EL2(val)       __MSR_SYS(AFSR1_EL2, val)

/** Auxiliary Fault Status Register 1 (EL3) */
#define AFSR1_EL3                              AFSR1_EL3
#define __ARCH_REG_LOAD__AFSR1_EL3()           __MRS_SYS(AFSR1_EL3)
#define __ARCH_REG_STORE__AFSR1_EL3(val)       __MSR_SYS(AFSR1_EL3, val)

/** [RO] Auxiliary ID Register */
#define AIDR_EL1                               AIDR_EL1
#define __ARCH_REG_LOAD__AIDR_EL1()            __MRS_SYS(AIDR_EL1)

/** Auxiliary Memory Attribute Indirection Register (EL1) */
#define AMAIR_EL1                              AMAIR_EL1
#define __ARCH_REG_LOAD__AMAIR_EL1()           __MRS_SYS(AMAIR_EL1)
#define __ARCH_REG_STORE__AMAIR_EL1(val)       __MSR_SYS(AMAIR_EL1, val)

/** Auxiliary Memory Attribute Indirection Register (EL2) */
#define AMAIR_EL2                              AMAIR_EL2
#define __ARCH_REG_LOAD__AMAIR_EL2()           __MRS_SYS(AMAIR_EL2)
#define __ARCH_REG_STORE__AMAIR_EL2(val)       __MSR_SYS(AMAIR_EL2, val)

/** Auxiliary Memory Attribute Indirection Register (EL3) */
#define AMAIR_EL3                              AMAIR_EL3
#define __ARCH_REG_LOAD__AMAIR_EL3()           __MRS_SYS(AMAIR_EL3)
#define __ARCH_REG_STORE__AMAIR_EL3(val)       __MSR_SYS(AMAIR_EL3, val)

/** [RO] Current Cache Size ID Register */
#define CCSIDR_EL1                             CCSIDR_EL1
#define __ARCH_REG_LOAD__CCSIDR_EL1()          __MRS_SYS(CCSIDR_EL1)

/** [RO] Cache Level ID Register */
#define CLIDR_EL1                              CLIDR_EL1
#define __ARCH_REG_LOAD__CLIDR_EL1()           __MRS_SYS(CLIDR_EL1)

/** Context ID Register */
#define CONTEXTIDR_EL1                         CONTEXTIDR_EL1
#define __ARCH_REG_LOAD__CONTEXTIDR_EL1()      __MRS_SYS(CONTEXTIDR_EL1)
#define __ARCH_REG_STORE__CONTEXTIDR_EL1(val)  __MSR_SYS(CONTEXTIDR_EL1, val)

/** Architectural Feature Access Control Register */
#define CPACR_EL1                              CPACR_EL1
#define __ARCH_REG_LOAD__CPACR_EL1()           __MRS_SYS(CPACR_EL1)
#define __ARCH_REG_STORE__CPACR_EL1(val)       __MSR_SYS(CPACR_EL1, val)

/** Architectural Feature Trap Register (EL2) */
#define CPTR_EL2                               CPTR_EL2
#define __ARCH_REG_LOAD__CPTR_EL2()            __MRS_SYS(CPTR_EL2)
#define __ARCH_REG_STORE__CPTR_EL2(val)        __MSR_SYS(CPTR_EL2, val)

/** Architectural Feature Trap Register (EL3) */
#define CPTR_EL3                               CPTR_EL3
#define __ARCH_REG_LOAD__CPTR_EL3()            __MRS_SYS(CPTR_EL3)
#define __ARCH_REG_STORE__CPTR_EL3(val)        __MSR_SYS(CPTR_EL3, val)

/** Cache Size Selection Register */
#define CSSELR_EL1                             CSSELR_EL1
#define __ARCH_REG_LOAD__CSSELR_EL1()          __MRS_SYS(CSSELR_EL1)
#define __ARCH_REG_STORE__CSSELR_EL1(val)      __MSR_SYS(CSSELR_EL1, val)

/** [RO] Cache Type Register */
#define CTR_EL0                                CTR_EL0
#define __ARCH_REG_LOAD__CTR_EL0()             __MRS_SYS(CTR_EL0)

/** Domain Access Control Register */
#define DACR32_EL2                             DACR32_EL2
#define __ARCH_REG_LOAD__DACR32_EL2()          __MRS_SYS(DACR32_EL2)
#define __ARCH_REG_STORE__DACR32_EL2(val)      __MSR_SYS(DACR32_EL2, val)

/** [RO] Data Cache Zero ID register */
#define DCZID_EL0                              DCZID_EL0
#define __ARCH_REG_LOAD__DCZID_EL0()           __MRS_SYS(DCZID_EL0)

/** Exception Syndrome Register (EL1) */
#define ESR_EL1                                ESR_EL1
#define __ARCH_REG_LOAD__ESR_EL1()             __MRS_SYS(ESR_EL1)
#define __ARCH_REG_STORE__ESR_EL1(val)         __MSR_SYS(ESR_EL1, val)

/** Exception Syndrome Register (EL2) */
#define ESR_EL2                                ESR_EL2
#define __ARCH_REG_LOAD__ESR_EL2()             __MRS_SYS(ESR_EL2)
#define __ARCH_REG_STORE__ESR_EL2(val)         __MSR_SYS(ESR_EL2, val)

/** Exception Syndrome Register (EL3) */
#define ESR_EL3                                ESR_EL3
#define __ARCH_REG_LOAD__ESR_EL3()             __MRS_SYS(ESR_EL3)
#define __ARCH_REG_STORE__ESR_EL3(val)         __MSR_SYS(ESR_EL3, val)

/** Fault Address Register (EL1) */
#define FAR_EL1                                FAR_EL1
#define __ARCH_REG_LOAD__FAR_EL1()             __MRS_SYS(FAR_EL1)
#define __ARCH_REG_STORE__FAR_EL1(val)         __MSR_SYS(FAR_EL1, val)

/** Fault Address Register (EL2) */
#define FAR_EL2                                FAR_EL2
#define __ARCH_REG_LOAD__FAR_EL2()             __MRS_SYS(FAR_EL2)
#define __ARCH_REG_STORE__FAR_EL2(val)         __MSR_SYS(FAR_EL2, val)

/** Fault Address Register (EL3) */
#define FAR_EL3                                FAR_EL3
#define __ARCH_REG_LOAD__FAR_EL3()             __MRS_SYS(FAR_EL3)
#define __ARCH_REG_STORE__FAR_EL3(val)         __MSR_SYS(FAR_EL3, val)

/** Floating-point Exception Control register */
#define FPEXC32_EL2                            FPEXC32_EL2
#define __ARCH_REG_LOAD__FPEXC32_EL2()         __MRS_SYS(FPEXC32_EL2)
#define __ARCH_REG_STORE__FPEXC32_EL2(val)     __MSR_SYS(FPEXC32_EL2, val)

/** Hypervisor Auxiliary Control Register */
#define HACR_EL2                               HACR_EL2
#define __ARCH_REG_LOAD__HACR_EL2()            __MRS_SYS(HACR_EL2)
#define __ARCH_REG_STORE__HACR_EL2(val)        __MSR_SYS(HACR_EL2, val)

/** Hypervisor Configuration Register */
#define HCR_EL2                                HCR_EL2
#define __ARCH_REG_LOAD__HCR_EL2()             __MRS_SYS(HCR_EL2)
#define __ARCH_REG_STORE__HCR_EL2(val)         __MSR_SYS(HCR_EL2, val)

#define HCR_EL2_RW                             (1UL << 31) /* EL1 is AArch64 */
#define HCR_EL2_E2H                            (1UL << 34) /* [ARMv8.2-A] VHE enabled */

/** Hypervisor IPA Fault Address Register */
#define HPFAR_EL2                              HPFAR_EL2
#define __ARCH_REG_LOAD__HPFAR_EL2()           __MRS_SYS(HPFAR_EL2)
#define __ARCH_REG_STORE__HPFAR_EL2(val)       __MSR_SYS(HPFAR_EL2, val)

/** Hypervisor System Trap Register */
#define HSTR_EL2                               HSTR_EL2
#define __ARCH_REG_LOAD__HSTR_EL2()            __MRS_SYS(HSTR_EL2)
#define __ARCH_REG_STORE__HSTR_EL2(val)        __MSR_SYS(HSTR_EL2, val)

/** [RO] AArch64 Auxiliary Feature Register 0 */
#define ID_AA64AFR0_EL1                        ID_AA64AFR0_EL1
#define __ARCH_REG_LOAD__ID_AA64AFR0_EL1()     __MRS_SYS(ID_AA64AFR0_EL1)

/** [RO] AArch64 Auxiliary Feature Register 1 */
#define ID_AA64AFR1_EL1                        ID_AA64AFR1_EL1
#define __ARCH_REG_LOAD__ID_AA64AFR1_EL1()     __MRS_SYS(ID_AA64AFR1_EL1)

/** [RO] AArch64 Debug Feature Register 0 */
#define ID_AA64DFR0_EL1                        ID_AA64DFR0_EL1
#define __ARCH_REG_LOAD__ID_AA64DFR0_EL1()     __MRS_SYS(ID_AA64DFR0_EL1)

/** [RO] AArch64 Instruction Set Attribute Register 0 */
#define ID_AA64ISAR0_EL1                       ID_AA64ISAR0_EL1
#define __ARCH_REG_LOAD__ID_AA64ISAR0_EL1()    __MRS_SYS(ID_AA64ISAR0_EL1)

/** [RO] AArch64 Instruction Set Attribute Register 1 */
#define ID_AA64ISAR1_EL1                       ID_AA64ISAR1_EL1
#define __ARCH_REG_LOAD__ID_AA64ISAR1_EL1()    __MRS_SYS(ID_AA64ISAR1_EL1)

/** [RO] AArch64 Memory Model Feature Register 0 */
#define ID_AA64MMFR0_EL1                       ID_AA64MMFR0_EL1
#define __ARCH_REG_LOAD__ID_AA64MMFR0_EL1()    __MRS_SYS(ID_AA64MMFR0_EL1)

#define ID_AA64MMFR0_EL1_PAR                   /* Physical Address range supported */
#define ID_AA64MMFR0_EL1_PAR_MASK              0b1111UL
#define ID_AA64MMFR0_EL1_PAR_SHIFT             0
#define ID_AA64MMFR0_EL1_PAR_32b               0b0000 /* 32 bits, 4 GB */
#define ID_AA64MMFR0_EL1_PAR_36b               0b0001 /* 36 bits, 64 GB */
#define ID_AA64MMFR0_EL1_PAR_40b               0b0010 /* 40 bits, 1 TB */
#define ID_AA64MMFR0_EL1_PAR_42b               0b0011 /* 42 bits, 4 TB */
#define ID_AA64MMFR0_EL1_PAR_44b               0b0100 /* 44 bits, 16 TB */
#define ID_AA64MMFR0_EL1_PAR_48b               0b0101 /* 48 bits, 256 TB */

/** [RO] AArch64 Memory Model Feature Register 1 */
#define ID_AA64MMFR1_EL1                       ID_AA64MMFR1_EL1
#define __ARCH_REG_LOAD__ID_AA64MMFR1_EL1()    __MRS_SYS(ID_AA64MMFR1_EL1)

/** [RO] AArch64 Processor Feature Register 0 */
#define ID_AA64PFR0_EL1                        ID_AA64PFR0_EL1
#define __ARCH_REG_LOAD__ID_AA64PFR0_EL1()     __MRS_SYS(ID_AA64PFR0_EL1)

/** [RO] AArch64 Processor Feature Register 1 */
#define ID_AA64PFR1_EL1                        ID_AA64PFR1_EL1
#define __ARCH_REG_LOAD__ID_AA64PFR1_EL1()     __MRS_SYS(ID_AA64PFR1_EL1)

/** [RO] Interrupt Status Register */
#define ISR_EL1                                ISR_EL1
#define __ARCH_REG_LOAD__ISR_EL1()             __MRS_SYS(ISR_EL1)

/** Memory Attribute Indirection Register (EL1) */
#define MAIR_EL1                               MAIR_EL1
#define __ARCH_REG_LOAD__MAIR_EL1()            __MRS_SYS(MAIR_EL1)
#define __ARCH_REG_STORE__MAIR_EL1(val)        __MSR_SYS(MAIR_EL1, val)

/** Memory Attribute Indirection Register (EL2) */
#define MAIR_EL2                               MAIR_EL2
#define __ARCH_REG_LOAD__MAIR_EL2()            __MRS_SYS(MAIR_EL2)
#define __ARCH_REG_STORE__MAIR_EL2(val)        __MSR_SYS(MAIR_EL2, val)

/** Memory Attribute Indirection Register (EL3) */
#define MAIR_EL3                               MAIR_EL3
#define __ARCH_REG_LOAD__MAIR_EL3()            __MRS_SYS(MAIR_EL3)
#define __ARCH_REG_STORE__MAIR_EL3(val)        __MSR_SYS(MAIR_EL3, val)

/** [RO] Main ID Register */
#define MIDR_EL1                               MIDR_EL1
#define __ARCH_REG_LOAD__MIDR_EL1()            __MRS_SYS(MIDR_EL1)

/** [RO] Multiprocessor Affinity Register */
#define MPIDR_EL1                              MPIDR_EL1
#define __ARCH_REG_LOAD__MPIDR_EL1()           __MRS_SYS(MPIDR_EL1)

#define MPIDR_EL1_AFF0                         /* Affinity level 0 */
#define MPIDR_EL1_AFF0_MASK                    0xffUL
#define MPIDR_EL1_AFF0_SHIFT                   0

#define MPIDR_EL1_AFF1                         /* Affinity level 1 */
#define MPIDR_EL1_AFF1_MASK                    0xffUL
#define MPIDR_EL1_AFF1_SHIFT                   8

#define MPIDR_EL1_AFF2                         /* Affinity level 2 */
#define MPIDR_EL1_AFF2_MASK                    0xffUL
#define MPIDR_EL1_AFF2_SHIFT                   16

#define MPIDR_EL1_AFF3                         /* Affinity level 3 */
#define MPIDR_EL1_AFF3_MASK                    0xffUL
#define MPIDR_EL1_AFF3_SHIFT                   32

/** [RO] Media and VFP Feature Register 0 */
#define MVFR0_EL1                              MVFR0_EL1
#define __ARCH_REG_LOAD__MVFR0_EL1()           __MRS_SYS(MVFR0_EL1)

/** [RO] Media and VFP Feature Register 1 */
#define MVFR1_EL1                              MVFR1_EL1
#define __ARCH_REG_LOAD__MVFR1_EL1()           __MRS_SYS(MVFR1_EL1)

/** [RO] Media and VFP Feature Register 2 */
#define MVFR2_EL1                              MVFR2_EL1
#define __ARCH_REG_LOAD__MVFR1_EL2()           __MRS_SYS(MVFR2_EL1)

/** Physical Address Register */
#define PAR_EL1                                PAR_EL1
#define __ARCH_REG_LOAD__PAR_EL1()             __MRS_SYS(PAR_EL1)
#define __ARCH_REG_STORE__PAR_EL1(val)         __MSR_SYS(PAR_EL1, val)

/** [RO] Revision ID Register */
#define REVIDR_EL1                             REVIDR_EL1
#define __ARCH_REG_LOAD__REVIDR_EL1()          __MRS_SYS(REVIDR_EL1)

/** Reset Management Register (if EL2 and EL3 not implemented) */
#define RMR_EL1                                RMR_EL1
#define __ARCH_REG_LOAD__RMR_EL1()             __MRS_SYS(RMR_EL1)
#define __ARCH_REG_STORE__RMR_EL1(val)         __MSR_SYS(RMR_EL1, val)

/** Reset Management Register (if EL3 not implemented) */
#define RMR_EL2                                RMR_EL2
#define __ARCH_REG_LOAD__RMR_EL2()             __MRS_SYS(RMR_EL2)
#define __ARCH_REG_STORE__RMR_EL2(val)         __MSR_SYS(RMR_EL2, val)

/** Reset Management Register (if EL3 implemented) */
#define RMR_EL3                                RMR_EL3
#define __ARCH_REG_LOAD__RMR_EL3()             __MRS_SYS(RMR_EL3)
#define __ARCH_REG_STORE__RMR_EL3(val)         __MSR_SYS(RMR_EL3, val)

/** [RO] Reset Vector Base Address Register (if EL2 and EL3 not implemented) */
#define RVBAR_EL1                              RVBAR_EL1
#define __ARCH_REG_LOAD__RVBAR_EL1()           __MRS_SYS(RVBAR_EL1)

/** [RO] Reset Vector Base Address Register (if EL3 not implemented) */
#define RVBAR_EL2                              RVBAR_EL2
#define __ARCH_REG_LOAD__RVBAR_EL2()           __MRS_SYS(RVBAR_EL2)

/** [RO] Reset Vector Base Address Register (if EL3 implemented) */
#define RVBAR_EL3                              RVBAR_EL3
#define __ARCH_REG_LOAD__RVBAR_EL3()           __MRS_SYS(RVBAR_EL3)

/** Secure Configuration Register */
#define SCR_EL3                                SCR_EL3
#define __ARCH_REG_LOAD__SCR_EL3()             __MRS_SYS(SCR_EL3)
#define __ARCH_REG_STORE__SCR_EL3(val)         __MSR_SYS(SCR_EL3, val)

#define SCR_EL3_NS                             (1U << 0)  /* Non-secure bit */
#define SCR_EL3_RW                             (1U << 10) /* EL2 is AArch64 */

/** System Control Register (EL1) */
#define SCTLR_EL1                              SCTLR_EL1
#define __ARCH_REG_LOAD__SCTLR_EL1()           __MRS_SYS(SCTLR_EL1)
#define __ARCH_REG_STORE__SCTLR_EL1(val)       __MSR_SYS(SCTLR_EL1, val)

#define SCTLR_EL1_RES1                         0x30d00800U /* Read-As-One */

/** System Control Register (EL2) */
#define SCTLR_EL2                              SCTLR_EL2
#define __ARCH_REG_LOAD__SCTLR_EL2()           __MRS_SYS(SCTLR_EL2)
#define __ARCH_REG_STORE__SCTLR_EL2(val)       __MSR_SYS(SCTLR_EL2, val)

#define SCTLR_EL2_RES1                         0x30c50830U /* Read-As-One */

/** System Control Register (EL3) */
#define SCTLR_EL3                              SCTLR_EL3
#define __ARCH_REG_LOAD__SCTLR_EL3()           __MRS_SYS(SCTLR_EL3)
#define __ARCH_REG_STORE__SCTLR_EL3(val)       __MSR_SYS(SCTLR_EL3, val)

#define SCTLR_ELn_M                            (1U << 0) /* MMU enable */
#define SCTLR_ELn_A                            (1U << 1) /* Alignment check enable */
#define SCTLR_ELn_C                            (1U << 2) /* Data cache enable */
#define SCTLR_ELn_SA                           (1U << 3) /* Stack alignment check enable */
#define SCTLR_ELn_I                            (1U << 12) /* Instruction cache enable */

/** Translation Control Register (EL1) */
#define TCR_EL1                                TCR_EL1
#define __ARCH_REG_LOAD__TCR_EL1()             __MRS_SYS(TCR_EL1)
#define __ARCH_REG_STORE__TCR_EL1(val)         __MSR_SYS(TCR_EL1, val)

#define TCR_EL1_TG1                            /* TTBR1_EL1 Granule Size */
#define TCR_EL1_TG1_MASK                       0b11UL
#define TCR_EL1_TG1_SHIFT                      30
#define TCR_EL1_TG1_4KB                        0b10
#define TCR_EL1_TG1_16KB                       0b01
#define TCR_EL1_TG1_64KB                       0b11

#define TCR_EL1_IPS                            /* Intermediate Physical Address Size */
#define TCR_EL1_IPS_MASK                       0b111UL
#define TCR_EL1_IPS_SHIFT                      32
#define TCR_EL1_IPS_32b                        0b000 /* 32 bits, 4 GB */
#define TCR_EL1_IPS_36b                        0b001 /* 36 bits, 64 GB */
#define TCR_EL1_IPS_40b                        0b010 /* 40 bits, 1 TB */
#define TCR_EL1_IPS_42b                        0b011 /* 42 bits, 4 TB */
#define TCR_EL1_IPS_44b                        0b100 /* 44 bits, 16 TB */
#define TCR_EL1_IPS_48b                        0b101 /* 48 bits, 256 TB */

/** Translation Control Register (EL2) */
#define TCR_EL2                                TCR_EL2
#define __ARCH_REG_LOAD__TCR_EL2()             __MRS_SYS(TCR_EL2)
#define __ARCH_REG_STORE__TCR_EL2(val)         __MSR_SYS(TCR_EL2, val)

/** Translation Control Register (EL3) */
#define TCR_EL3                                TCR_EL3
#define __ARCH_REG_LOAD__TCR_EL3()             __MRS_SYS(TCR_EL3)
#define __ARCH_REG_STORE__TCR_EL3(val)         __MSR_SYS(TCR_EL3, val)

#define TCR_ELn_T0SZ                           /* The size offset of the memory region addressed by TTBR0 (2^(64-T0SZ)) */
#define TCR_ELn_T0SZ_MASK                      0b111111UL
#define TCR_ELn_T0SZ_SHIFT                     0

#define TCR_ELn_TG0                            /* Granule Size for the corresponding TTBR0 register */
#define TCR_ELn_TG0_MASK                       0b11UL
#define TCR_ELn_TG0_SHIFT                      14
#define TCR_ELn_TG0_4KB                        0b00
#define TCR_ELn_TG0_16KB                       0b10
#define TCR_ELn_TG0_64KB                       0b01

/** Thread Pointer / ID Register (EL0) */
#define TPIDR_EL0                              TPIDR_EL0
#define __ARCH_REG_LOAD__TPIDR_EL0()           __MRS_SYS(TPIDR_EL0)
#define __ARCH_REG_STORE__TPIDR_EL0(val)       __MSR_SYS(TPIDR_EL0, val)

/** Thread Pointer / ID Register (EL1) */
#define TPIDR_EL1                              TPIDR_EL1
#define __ARCH_REG_LOAD__TPIDR_EL1()           __MRS_SYS(TPIDR_EL1)
#define __ARCH_REG_STORE__TPIDR_EL1(val)       __MSR_SYS(TPIDR_EL1, val)

/** Thread Pointer / ID Register (EL2) */
#define TPIDR_EL2                              TPIDR_EL2
#define __ARCH_REG_LOAD__TPIDR_EL2()           __MRS_SYS(TPIDR_EL2)
#define __ARCH_REG_STORE__TPIDR_EL2(val)       __MSR_SYS(TPIDR_EL2, val)

/** Thread Pointer / ID Register (EL3) */
#define TPIDR_EL3                              TPIDR_EL3
#define __ARCH_REG_LOAD__TPIDR_EL3()           __MRS_SYS(TPIDR_EL3)
#define __ARCH_REG_STORE__TPIDR_EL3(val)       __MSR_SYS(TPIDR_EL3, val)

/** TPIDRRO_EL0, Thread Pointer / ID Register, Read-Only (EL0) */
#define TPIDRRO_EL0                            TPIDRRO_EL0
#define __ARCH_REG_LOAD__TPIDRRO_EL0()         __MRS_SYS(TPIDRRO_EL0)
#define __ARCH_REG_STORE__TPIDRRO_EL0(val)     __MSR_SYS(TPIDRRO_EL0, val)

/** Translation Table Base Register 0 (EL1) */
#define TTBR0_EL1                              TTBR0_EL1
#define __ARCH_REG_LOAD__TTBR0_EL1()           __MRS_SYS(TTBR0_EL1)
#define __ARCH_REG_STORE__TTBR0_EL1(val)       __MSR_SYS(TTBR0_EL1, val)

/** Translation Table Base Register 0 (EL2) */
#define TTBR0_EL2                              TTBR0_EL2
#define __ARCH_REG_LOAD__TTBR0_EL2()           __MRS_SYS(TTBR0_EL2)
#define __ARCH_REG_STORE__TTBR0_EL2(val)       __MSR_SYS(TTBR0_EL2, val)

/** Translation Table Base Register 0 (EL3) */
#define TTBR0_EL3                              TTBR0_EL3
#define __ARCH_REG_LOAD__TTBR0_EL3()           __MRS_SYS(TTBR0_EL3)
#define __ARCH_REG_STORE__TTBR0_EL3(val)       __MSR_SYS(TTBR0_EL3, val)

/** Translation Table Base Register 1 */
#define TTBR1_EL1                              TTBR1_EL1
#define __ARCH_REG_LOAD__TTBR1_EL1()           __MRS_SYS(TTBR1_EL1)
#define __ARCH_REG_STORE__TTBR1_EL1(val)       __MSR_SYS(TTBR1_EL1, val)

/** Vector Base Address Register (EL1) */
#define VBAR_EL1                               VBAR_EL1
#define __ARCH_REG_LOAD__VBAR_EL1()            __MRS_SYS(VBAR_EL1)
#define __ARCH_REG_STORE__VBAR_EL1(val)        __MSR_SYS(VBAR_EL1, val)

/** Vector Base Address Register (EL2) */
#define VBAR_EL2                               VBAR_EL2
#define __ARCH_REG_LOAD__VBAR_EL2()            __MRS_SYS(VBAR_EL2)
#define __ARCH_REG_STORE__VBAR_EL2(val)        __MSR_SYS(VBAR_EL2, val)

/** Vector Base Address Register (EL3) */
#define VBAR_EL3                               VBAR_EL3
#define __ARCH_REG_LOAD__VBAR_EL3()            __MRS_SYS(VBAR_EL3)
#define __ARCH_REG_STORE__VBAR_EL3(val)        __MSR_SYS(VBAR_EL3, val)

/** Virtualization Multiprocessor ID Register */
#define VMPIDR_EL2                             VMPIDR_EL2
#define __ARCH_REG_LOAD__VMPIDR_EL2()          __MRS_SYS(VMPIDR_EL2)
#define __ARCH_REG_STORE__VMPIDR_EL2(val)      __MSR_SYS(VMPIDR_EL2, val)

/** Virtualization Processor ID Register */
#define VPIDR_EL2                              VPIDR_EL2
#define __ARCH_REG_LOAD__VPIDR_EL2()           __MRS_SYS(VPIDR_EL2)
#define __ARCH_REG_STORE__VPIDR_EL2(val)       __MSR_SYS(VPIDR_EL2, val)

/** Virtualization Translation Control Register */
#define VTCR_EL2                               VTCR_EL2
#define __ARCH_REG_LOAD__VTCR_EL2()            __MRS_SYS(VTCR_EL2)
#define __ARCH_REG_STORE__VTCR_EL2(val)        __MSR_SYS(VTCR_EL2, val)

/** Virtualization Translation Table Base Register */
#define VTTBR_EL2                              VTTBR_EL2
#define __ARCH_REG_LOAD__VTTBR_EL2()           __MRS_SYS(VTTBR_EL2)
#define __ARCH_REG_STORE__VTTBR_EL2(val)       __MSR_SYS(VTTBR_EL2, val)

/** Counter-timer Frequency register */
#define CNTFRQ_EL0                             CNTFRQ_EL0
#define __ARCH_REG_LOAD__CNTFRQ_EL0()          __MRS_SYS(CNTFRQ_EL0)
#define __ARCH_REG_STORE__CNTFRQ_EL0(val)      __MSR_SYS(CNTFRQ_EL0, val)

/** Counter-timer Hypervisor Control register */
#define CNTHCTL_EL2                            CNTHCTL_EL2
#define __ARCH_REG_LOAD__CNTHCTL_EL2()         __MRS_SYS(CNTHCTL_EL2)
#define __ARCH_REG_STORE__CNTHCTL_EL2(val)     __MSR_SYS(CNTHCTL_EL2, val)

#define CNTHCTL_EL2_EL1PCTEN                   (1U << 0)
#define CNTHCTL_EL2_EL1PCEN                    (1U << 1)

/** Counter-timer Hypervisor Physical Timer Control register */
#define CNTHP_CTL_EL2                          CNTHP_CTL_EL2
#define __ARCH_REG_LOAD__CNTHP_CTL_EL2()       __MRS_SYS(CNTHP_CTL_EL2)
#define __ARCH_REG_STORE__CNTHP_CTL_EL2(val)   __MSR_SYS(CNTHP_CTL_EL2, val)

/** Counter-timer Hypervisor Physical Timer CompareValue register */
#define CNTHP_CVAL_EL2                         CNTHP_CVAL_EL2
#define __ARCH_REG_LOAD__CNTHP_CVAL_EL2()      __MRS_SYS(CNTHP_CVAL_EL2)
#define __ARCH_REG_STORE__CNTHP_CVAL_EL2(val)  __MSR_SYS(CNTHP_CVAL_EL2, val)

/** Counter-timer Hypervisor Physical Timer TimerValue register */
#define CNTHP_TVAL_EL2                         CNTHP_TVAL_EL2
#define __ARCH_REG_LOAD__CNTHP_TVAL_EL2()      __MRS_SYS(CNTHP_TVAL_EL2)
#define __ARCH_REG_STORE__CNTHP_TVAL_EL2(val)  __MSR_SYS(CNTHP_TVAL_EL2, val)

/** Counter-timer Kernel Control register */
#define CNTKCTL_EL1                            CNTKCTL_EL1
#define __ARCH_REG_LOAD__CNTKCTL_EL1()         __MRS_SYS(CNTKCTL_EL1)
#define __ARCH_REG_STORE__CNTKCTL_EL1(val)     __MSR_SYS(CNTKCTL_EL1, val)

/** Counter-timer Physical Timer Control register */
#define CNTP_CTL_EL0                           CNTP_CTL_EL0
#define __ARCH_REG_LOAD__CNTP_CTL_EL0()        __MRS_SYS(CNTP_CTL_EL0)
#define __ARCH_REG_STORE__CNTP_CTL_EL0(val)    __MSR_SYS(CNTP_CTL_EL0, val)

#define CNTP_CTL_EL0_EN                        (1U << 0) /* Enables the timer */
#define CNTP_CTL_EL0_IMASK                     (1U << 1) /* Timer interrupt mask bit */
#define CNTP_CTL_EL0_ISTAT                     (1U << 2) /* The status of the timer interrupt */

/** Counter-timer Physical Timer CompareValue register */
#define CNTP_CVAL_EL0                          CNTP_CVAL_EL0
#define __ARCH_REG_LOAD__CNTP_CVAL_EL0()       __MRS_SYS(CNTP_CVAL_EL0)
#define __ARCH_REG_STORE__CNTP_CVAL_EL0(val)   __MSR_SYS(CNTP_CVAL_EL0, val)

/** Counter-timer Physical Timer TimerValue register */
#define CNTP_TVAL_EL0                          CNTP_TVAL_EL0
#define __ARCH_REG_LOAD__CNTP_TVAL_EL0()       __MRS_SYS(CNTP_TVAL_EL0)
#define __ARCH_REG_STORE__CNTP_TVAL_EL0(val)   __MSR_SYS(CNTP_TVAL_EL0, val)

/** [RO] Counter-timer Physical Count register */
#define CNTPCT_EL0                             CNTPCT_EL0
#define __ARCH_REG_LOAD__CNTPCT_EL0()          __MRS_SYS(CNTPCT_EL0)

/** Counter-timer Physical Secure Timer Control register */
#define CNTPS_CTL_EL1                          CNTPS_CTL_EL1
#define __ARCH_REG_LOAD__CNTPS_CTL_EL1()       __MRS_SYS(CNTPS_CTL_EL1)
#define __ARCH_REG_STORE__CNTPS_CTL_EL1(val)   __MSR_SYS(CNTPS_CTL_EL1, val)

/** Counter-timer Physical Secure Timer CompareValue register */
#define CNTPS_CVAL_EL1                         CNTPS_CVAL_EL1
#define __ARCH_REG_LOAD__CNTPS_CVAL_EL1()      __MRS_SYS(CNTPS_CVAL_EL1)
#define __ARCH_REG_STORE__CNTPS_CVAL_EL1(val)  __MSR_SYS(CNTPS_CVAL_EL1, val)

/** Counter-timer Physical Secure Timer TimerValue register */
#define CNTPS_TVAL_EL1                         CNTPS_TVAL_EL1
#define __ARCH_REG_LOAD__CNTPS_TVAL_EL1()      __MRS_SYS(CNTPS_TVAL_EL1)
#define __ARCH_REG_STORE__CNTPS_TVAL_EL1(val)  __MSR_SYS(CNTPS_TVAL_EL1, val)

/** Counter-timer Virtual Timer Control register */
#define CNTV_CTL_EL0                           CNTV_CTL_EL0
#define __ARCH_REG_LOAD__CNTV_CTL_EL0()        __MRS_SYS(CNTV_CTL_EL0)
#define __ARCH_REG_STORE__CNTV_CTL_EL0(val)    __MSR_SYS(CNTV_CTL_EL0, val)

/** Counter-timer Virtual Timer CompareValue register */
#define CNTV_CVAL_EL0                          CNTV_CVAL_EL0
#define __ARCH_REG_LOAD__CNTV_CVAL_EL0()       __MRS_SYS(CNTV_CVAL_EL0)
#define __ARCH_REG_STORE__CNTV_CVAL_EL0(val)   __MSR_SYS(CNTV_CVAL_EL0, val)

/** Counter-timer Virtual Timer TimerValue register */
#define CNTV_TVAL_EL0                          CNTV_TVAL_EL0
#define __ARCH_REG_LOAD__CNTV_TVAL_EL0()       __MRS_SYS(CNTV_TVAL_EL0)
#define __ARCH_REG_STORE__CNTV_TVAL_EL0(val)   __MSR_SYS(CNTV_TVAL_EL0, val)

/** [RO] Counter-timer Virtual Count register */
#define CNTVCT_EL0                             CNTVCT_EL0
#define __ARCH_REG_LOAD__CNTVCT_EL0()          __MRS_SYS(CNTVCT_EL0)

/** Counter-timer Virtual Offset register */
#define CNTVOFF_EL2                            CNTVOFF_EL2
#define __ARCH_REG_LOAD__CNTVOFF_EL2()         __MRS_SYS(CNTVOFF_EL2)
#define __ARCH_REG_STORE__CNTVOFF_EL2(val)     __MSR_SYS(CNTVOFF_EL2, val)

/** Interrupt Controller Active Priorities Register (0,0) */
#define ICC_AP0R0_EL1                          ICC_AP0R0_EL1
#define __ARCH_REG_LOAD__ICC_AP0R0_EL1()       __MRS_SYS(S3_0_C12_C8_4)
#define __ARCH_REG_STORE__ICC_AP0R0_EL1(val)   __MSR_SYS(S3_0_C12_C8_4, val)

/** Interrupt Controller Active Priorities Register (0,1) */
#define ICC_AP0R1_EL1                          ICC_AP0R1_EL1
#define __ARCH_REG_LOAD__ICC_AP0R1_EL1()       __MRS_SYS(S3_0_C12_C8_5)
#define __ARCH_REG_STORE__ICC_AP0R1_EL1(val)   __MSR_SYS(S3_0_C12_C8_5, val)

/** Interrupt Controller Active Priorities Register (0,2) */
#define ICC_AP0R2_EL1                          ICC_AP0R2_EL1
#define __ARCH_REG_LOAD__ICC_AP0R2_EL1()       __MRS_SYS(S3_0_C12_C8_6)
#define __ARCH_REG_STORE__ICC_AP0R2_EL1(val)   __MSR_SYS(S3_0_C12_C8_6, val)

/** Interrupt Controller Active Priorities Register (0,3) */
#define ICC_AP0R3_EL1                          ICC_AP0R3_EL1
#define __ARCH_REG_LOAD__ICC_AP0R3_EL1()       __MRS_SYS(S3_0_C12_C8_7)
#define __ARCH_REG_STORE__ICC_AP0R3_EL1(val)   __MSR_SYS(S3_0_C12_C8_7, val)

/** Interrupt Controller Active Priorities Register (1,0) */
#define ICC_AP1R0_EL1                          ICC_AP1R0_EL1
#define __ARCH_REG_LOAD__ICC_AP1R0_EL1()       __MRS_SYS(S3_0_C12_C9_0)
#define __ARCH_REG_STORE__ICC_AP1R0_EL1(val)   __MSR_SYS(S3_0_C12_C9_0, val)

/** Interrupt Controller Active Priorities Register (1,1) */
#define ICC_AP1R1_EL1                          ICC_AP1R1_EL1
#define __ARCH_REG_LOAD__ICC_AP1R1_EL1()       __MRS_SYS(S3_0_C12_C9_1)
#define __ARCH_REG_STORE__ICC_AP1R1_EL1(val)   __MSR_SYS(S3_0_C12_C9_1, val)

/** Interrupt Controller Active Priorities Register (1,2) */
#define ICC_AP1R2_EL1                          ICC_AP1R2_EL1
#define __ARCH_REG_LOAD__ICC_AP1R2_EL1()       __MRS_SYS(S3_0_C12_C9_2)
#define __ARCH_REG_STORE__ICC_AP1R2_EL1(val)   __MSR_SYS(S3_0_C12_C9_2, val)

/** Interrupt Controller Active Priorities Register (1,3) */
#define ICC_AP1R3_EL1                          ICC_AP1R3_EL1
#define __ARCH_REG_LOAD__ICC_AP1R3_EL1()       __MRS_SYS(S3_0_C12_C9_3)
#define __ARCH_REG_STORE__ICC_AP1R3_EL1(val)   __MSR_SYS(S3_0_C12_C9_3, val)

/** Interrupt Controller Alias Software Generated Interrupt group 1 Register */
#define ICC_ASGI1R_EL1                         ICC_ASGI1R_EL1
#define __ARCH_REG_STORE__ICC_ASGI1R_EL1(val)  __MSR_SYS(S3_0_C12_C11_6, val)

/** Interrupt Controller Binary Point Register 0 */
#define ICC_BPR0_EL1                           ICC_BPR0_EL1
#define __ARCH_REG_LOAD__ICC_BPR0_EL1()        __MRS_SYS(S3_0_C12_C8_3)
#define __ARCH_REG_STORE__ICC_BPR0_EL1(val)    __MSR_SYS(S3_0_C12_C8_3, val)

/** Interrupt Controller Binary Point Register 1 */
#define ICC_BPR1_EL1                           ICC_BPR1_EL1
#define __ARCH_REG_LOAD__ICC_BPR1_EL1()        __MRS_SYS(S3_0_C12_C12_3)
#define __ARCH_REG_STORE__ICC_BPR1_EL1(val)    __MSR_SYS(S3_0_C12_C12_3, val)

/** Interrupt Controller Control Register (EL1) */
#define ICC_CTLR_EL1                           ICC_CTLR_EL1
#define __ARCH_REG_LOAD__ICC_CTLR_EL1()        __MRS_SYS(S3_0_C12_C12_4)
#define __ARCH_REG_STORE__ICC_CTLR_EL1(val)    __MSR_SYS(S3_0_C12_C12_4, val)

#define ICC_CTLR_EL1_EOImode                   (1U << 1) /* EOI mode */

/** Interrupt Controller Control Register (EL3) */
#define ICC_CTLR_EL3                           ICC_CTLR_EL3
#define __ARCH_REG_LOAD__ICC_CTLR_EL3()        __MRS_SYS(S3_6_C12_C12_4)
#define __ARCH_REG_STORE__ICC_CTLR_EL3(val)    __MSR_SYS(S3_6_C12_C12_4, val)

/** [WO] Interrupt Controller Deactivate Interrupt Register */
#define ICC_DIR_EL1                            ICC_DIR_EL1
#define __ARCH_REG_STORE__ICC_DIR_EL1(val)     __MSR_SYS(S3_0_C12_C11_1, val)

/** [WO] Interrupt Controller End Of Interrupt Register 0 */
#define ICC_EOIR0_EL1                          ICC_EOIR0_EL1
#define __ARCH_REG_STORE__ICC_EOIR0_EL1(val)   __MSR_SYS(S3_0_C12_C8_1, val)

/** [WO] Interrupt Controller End Of Interrupt Register 1 */
#define ICC_EOIR1_EL1                          ICC_EOIR1_EL1
#define __ARCH_REG_STORE__ICC_EOIR1_EL1(val)   __MSR_SYS(S3_0_C12_C12_1, val)

/** [RO] Interrupt Controller Highest Priority Pending Interrupt Register 0 */
#define ICC_HPPIR0_EL1                         ICC_HPPIR0_EL1
#define __ARCH_REG_LOAD__ICC_HPPIR0_EL1()      __MRS_SYS(S3_0_C12_C8_2)

/** [RO] Interrupt Controller Highest Priority Pending Interrupt Register 1 */
#define ICC_HPPIR1_EL1                         ICC_HPPIR1_EL1
#define __ARCH_REG_LOAD__ICC_HPPIR1_EL1()      __MRS_SYS(S3_0_C12_C12_2)

/** [RO] Interrupt Controller Interrupt Acknowledge Register 0 */
#define ICC_IAR0_EL1                           ICC_IAR0_EL1
#define __ARCH_REG_LOAD__ICC_IAR0_EL1()        __MRS_SYS(S3_0_C12_C8_0)

/** [RO] Interrupt Controller Interrupt Acknowledge Register 1 */
#define ICC_IAR1_EL1                           ICC_IAR1_EL1
#define __ARCH_REG_LOAD__ICC_IAR1_EL1()        __MRS_SYS(S3_0_C12_C12_0)

#define ICC_IAR1_EL1_INTID                     /* The ID of the signaled interrupt */
#define ICC_IAR1_EL1_INTID_MASK                0xffffffU
#define ICC_IAR1_EL1_INTID_SHIFT               0

/** Interrupt Controller Interrupt Group 0 Enable register */
#define ICC_IGRPEN0_EL1                        ICC_IGRPEN0_EL1
#define __ARCH_REG_LOAD__ICC_IGRPEN0_EL1()     __MRS_SYS(S3_0_C12_C12_6)
#define __ARCH_REG_STORE__ICC_IGRPEN0_EL1(val) __MSR_SYS(S3_0_C12_C12_6, val)

/** Interrupt Controller Interrupt Group 1 Enable register */
#define ICC_IGRPEN1_EL1                        ICC_IGRPEN1_EL1
#define __ARCH_REG_LOAD__ICC_IGRPEN1_EL1()     __MRS_SYS(S3_0_C12_C12_7)
#define __ARCH_REG_STORE__ICC_IGRPEN1_EL1(val) __MSR_SYS(S3_0_C12_C12_7, val)

#define ICC_IGRPEN1_EL1_EN                     (1U << 0) /* Enables Group 1 interrupts */

/** Interrupt Controller Interrupt Group 1 Enable register (EL3) */
#define ICC_IGRPEN1_EL3                        ICC_IGRPEN1_EL3
#define __ARCH_REG_LOAD__ICC_IGRPEN1_EL3()     __MRS_SYS(S3_6_C12_C12_7)
#define __ARCH_REG_STORE__ICC_IGRPEN1_EL3(val) __MSR_SYS(S3_6_C12_C12_7, val)

#define ICC_IGRPEN1_EL3_EN1NS                  (1U << 0) /* Enable for NS state */
#define ICC_IGRPEN1_EL3_EN1S                   (1U << 1) /* Enable for S state */

/** Interrupt Controller Interrupt Priority Mask Register */
#define ICC_PMR_EL1                            ICC_PMR_EL1
#define __ARCH_REG_LOAD__ICC_PMR_EL1()         __MRS_SYS(S3_0_C4_C6_0)
#define __ARCH_REG_STORE__ICC_PMR_EL1(val)     __MSR_SYS(S3_0_C4_C6_0, val)

/** [RO] Interrupt Controller Running Priority Register */
#define ICC_RPR_EL1                            ICC_RPR_EL1
#define __ARCH_REG_LOAD__ICC_RPR_EL1()         __MRS_SYS(S3_0_C12_C11_3)

/** Interrupt Controller System Error Interrupt Enable register */
#define ICC_SEIEN_EL1                          ICC_SEIEN_EL1
#define __ARCH_REG_LOAD__ICC_SEIEN_EL1()       __MRS_SYS(S3_0_C12_C11_0)
#define __ARCH_REG_STORE__ICC_SEIEN_EL1(val)   __MSR_SYS(S3_0_C12_C11_0, val)

/** [WO] Interrupt Controller Software Generated Interrupt group 0 Register */
#define ICC_SGI0R_EL1                          ICC_SGI0R_EL1
#define __ARCH_REG_STORE__ICC_SGI0R_EL1(val)   __MSR_SYS(S3_0_C12_C11_7, val)

/** [WO] Interrupt Controller Software Generated Interrupt group 1 Register */
#define ICC_SGI1R_EL1                          ICC_SGI1R_EL1
#define __ARCH_REG_STORE__ICC_SGI1R_EL1(val)   __MSR_SYS(S3_0_C12_C11_5, val)

/** Interrupt Controller System Register Enable register (EL1) */
#define ICC_SRE_EL1                            ICC_SRE_EL1
#define __ARCH_REG_LOAD__ICC_SRE_EL1()         __MRS_SYS(S3_0_C12_C12_5)
#define __ARCH_REG_STORE__ICC_SRE_EL1(val)     __MSR_SYS(S3_0_C12_C12_5, val)

/** Interrupt Controller System Register Enable register (EL2) */
#define ICC_SRE_EL2                            ICC_SRE_EL2
#define __ARCH_REG_LOAD__ICC_SRE_EL2()         __MRS_SYS(S3_4_C12_C9_5)
#define __ARCH_REG_STORE__ICC_SRE_EL2(val)     __MSR_SYS(S3_4_C12_C9_5, val)

/** Interrupt Controller System Register Enable register (EL3) */
#define ICC_SRE_EL3                            ICC_SRE_EL3
#define __ARCH_REG_LOAD__ICC_SRE_EL3()         __MRS_SYS(S3_6_C12_C12_5)
#define __ARCH_REG_STORE__ICC_SRE_EL3(val)     __MSR_SYS(S3_6_C12_C12_5, val)

#define ICC_SRE_SRE                            (1U << 0) /* System Register Enable */
#define ICC_SRE_DFB                            (1U << 1) /* Disable FIQ bypass */
#define ICC_SRE_DIB                            (1U << 2) /* Disable IRQ bypass */
#define ICC_SRE_EN                             (1U << 3) /* Enable lower exception level access */

/** Interrupt Controller Hyp Active Priorities Register (0,0) */
#define ICH_AP0R0_EL2                          ICH_AP0R0_EL2
#define __ARCH_REG_LOAD__ICH_AP0R0_EL2()       __MRS_SYS(S3_4_C12_C8_0)
#define __ARCH_REG_STORE__ICH_AP0R0_EL2(val)   __MSR_SYS(S3_4_C12_C8_0, val)

/** Interrupt Controller Hyp Active Priorities Register (0,1) */
#define ICH_AP0R1_EL2                          ICH_AP0R1_EL2
#define __ARCH_REG_LOAD__ICH_AP0R1_EL2()       __MRS_SYS(S3_4_C12_C8_1)
#define __ARCH_REG_STORE__ICH_AP0R1_EL2(val)   __MSR_SYS(S3_4_C12_C8_1, val)

/** Interrupt Controller Hyp Active Priorities Register (0,2) */
#define ICH_AP0R2_EL2                          ICH_AP0R2_EL2
#define __ARCH_REG_LOAD__ICH_AP0R2_EL2()       __MRS_SYS(S3_4_C12_C8_2)
#define __ARCH_REG_STORE__ICH_AP0R2_EL2(val)   __MSR_SYS(S3_4_C12_C8_2, val)

/** Interrupt Controller Hyp Active Priorities Register (0,3) */
#define ICH_AP0R3_EL2                          ICH_AP0R3_EL2
#define __ARCH_REG_LOAD__ICH_AP0R3_EL2()       __MRS_SYS(S3_4_C12_C8_3)
#define __ARCH_REG_STORE__ICH_AP0R3_EL2(val)   __MSR_SYS(S3_4_C12_C8_3, val)

/** Interrupt Controller Hyp Active Priorities Register (1,0) */
#define ICH_AP1R0_EL2                          ICH_AP1R0_EL2
#define __ARCH_REG_LOAD__ICH_AP1R0_EL2()       __MRS_SYS(S3_4_C12_C9_0)
#define __ARCH_REG_STORE__ICH_AP1R0_EL2(val)   __MSR_SYS(S3_4_C12_C9_0, val)

/** Interrupt Controller Hyp Active Priorities Register (1,1) */
#define ICH_AP1R1_EL2                          ICH_AP1R1_EL2
#define __ARCH_REG_LOAD__ICH_AP1R1_EL2()       __MRS_SYS(S3_4_C12_C9_1)
#define __ARCH_REG_STORE__ICH_AP1R1_EL2(val)   __MSR_SYS(S3_4_C12_C9_1, val)

/** Interrupt Controller Hyp Active Priorities Register (1,2) */
#define ICH_AP1R2_EL2                          ICH_AP1R2_EL2
#define __ARCH_REG_LOAD__ICH_AP1R2_EL2()       __MRS_SYS(S3_4_C12_C9_2)
#define __ARCH_REG_STORE__ICH_AP1R2_EL2(val)   __MSR_SYS(S3_4_C12_C9_2, val)

/** Interrupt Controller Hyp Active Priorities Register (1,3) */
#define ICH_AP1R3_EL2                          ICH_AP1R3_EL2
#define __ARCH_REG_LOAD__ICH_AP1R3_EL2()       __MRS_SYS(S3_4_C12_C9_3)
#define __ARCH_REG_STORE__ICH_AP1R3_EL2(val)   __MSR_SYS(S3_4_C12_C9_3, val)

/** [RO] Interrupt Controller End of Interrupt Status Register */
#define ICH_EISR_EL2                           ICH_EISR_EL2
#define __ARCH_REG_LOAD__ICH_EISR_EL2()        __MRS_SYS(S3_4_C12_C11_3)

/** [RO] Interrupt Controller Empty List Register Status Register */
#define ICH_ELSR_EL2                           ICH_ELSR_EL2
#define __ARCH_REG_LOAD__ICH_ELSR_EL2()        __MRS_SYS(S3_4_C12_C11_5)

/** Interrupt Controller Hyp Control Register */
#define ICH_HCR_EL2                            ICH_HCR_EL2
#define __ARCH_REG_LOAD__ICH_HCR_EL2()         __MRS_SYS(S3_4_C12_C11_0)
#define __ARCH_REG_STORE__ICH_HCR_EL2(val)     __MSR_SYS(S3_4_C12_C11_0, val)

/** [RO] Interrupt Controller Maintenance Interrupt State Register */
#define ICH_MISR_EL2                           ICH_MISR_EL2
#define __ARCH_REG_LOAD__ICH_MISR_EL2()        __MRS_SYS(S3_4_C12_C11_2)

/** Interrupt Controller Virtual Machine Control Register */
#define ICH_VMCR_EL2                           ICH_VMCR_EL2
#define __ARCH_REG_LOAD__ICH_VMCR_EL2()        __MRS_SYS(S3_4_C12_C11_7)
#define __ARCH_REG_STORE__ICH_VMCR_EL2(val)    __MSR_SYS(S3_4_C12_C11_7, val)

/** Interrupt Controller Virtual System Error Interrupt Register */
#define ICH_VSEIR_EL2                          ICH_VSEIR_EL2
#define __ARCH_REG_LOAD__ICH_VSEIR_EL2()       __MRS_SYS(S3_4_C12_C9_4)
#define __ARCH_REG_STORE__ICH_VSEIR_EL2(val)   __MSR_SYS(S3_4_C12_C9_4, val)

/** [RO] Interrupt Controller VGIC Type Register */
#define ICH_VTR_EL2                            ICH_VTR_EL2
#define __ARCH_REG_LOAD__ICH_VTR_EL2()         __MRS_SYS(S3_4_C12_C11_1)

#endif /* AARCH64_REG_SYSREG_H_ */
