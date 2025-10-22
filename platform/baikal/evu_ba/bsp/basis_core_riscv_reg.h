/**
 * *****************************************************************************
 *  @file       basis_core_riscv_reg.h
 *  @author     Baikal electronics SDK team
 *  @brief      BASIS RISC-V control and status registers definitins
 *  @version    1.3
 *  @date       2025-06-30
 * *****************************************************************************
 *  @copyright Copyright (c) 2025 Baikal Electronics
 *
 *  This software is licensed under terms that can be found in the LICENSE file
 *  in the root directory of this software component.
 *  If no LICENSE file comes with this software, it is provided AS-IS.
 * *****************************************************************************
 */

#ifndef CORE_RISCV_REG_H
#define CORE_RISCV_REG_H

#include "basis_core_riscv_param.h"

/* Control and Status Registers */

/* Machine level CSRs */
#define CSR_MSTATUS                         0x300U  /*!< Machine status register */
#define CSR_MISA                            0x301U  /*!< Machine ISA and extensions */
#define CSR_MEDELEG                         0x302U  /*!< Machine exception delegation register */
#define CSR_MIDELEG                         0x303U  /*!< Machine interrupt delegation register */
#define CSR_MIE                             0x304U  /*!< Machine interrupt enable register */
#define CSR_MTVEC                           0x305U  /*!< Machine trap handler base address */
#define CSR_MCOUNTEREN                      0x306U  /*!< Machine counter enable */
#define CSR_MTVT                            0x307U  /*!< Machine trap handler vector table base address */
#define CSR_MSTATUSH                        0x310U  /*!< Machine additional status register, RV32 only */

#define CSR_MSCRATCH                        0x340U  /*!< Machine scratch register for trap handlers */
#define CSR_MEPC                            0x341U  /*!< Machine exception program counter */
#define CSR_MCAUSE                          0x342U  /*!< Machine trap cause */
#define CSR_MTVAL                           0x343U  /*!< Machine bad address or instruction */
#define CSR_MIP                             0x344U  /*!< Machine interrupt pending */

#define CSR_MNXTI                           0x345U  /*!< Machine interrupt handler address and enable modifier */
#define CSR_MINTSTATUS                      0x346U  /*!< Machine current interrupt levels */
#define CSR_MINTTHRESH                      0x347U  /*!< Machine interrupt level threshold */
#define CSR_MSCRATCHCSW                     0x348U  /*!< Machine conditional scratch swap on priv mode change */
#define CSR_MSCRATCHCSWL                    0x349U  /*!< Machine conditional scratch swap on level change */

#define CSR_MNMITVEC                        0x7C0U  /*!< Machine NMI trap handler address */

#define CSR_MCYCLE                          0xB00U  /*!< Machine cycle counter */
#define CSR_MINSTRET                        0xB02U  /*!< Machine instructions-retired counter */
#define CSR_MCYCLEH                         0xB80U  /*!< Upper 32 bits of mcycle, RV32 only */
#define CSR_MINSTRETH                       0xB82U  /*!< Upper 32 bits of minstret, RV32 only */

/* User level CSRs */
#define CSR_CYCLE                           0xC00U  /*!< Cycle counter */
#define CSR_TIME                            0xC01U  /*!< Timer counter */
#define CSR_INSTRET                         0xC02U  /*!< Instructions-retired counter */
#define CSR_CYCLEH                          0xC80U  /*!< Upper 32 bits of cycle, RV32 only */
#define CSR_TIMEH                           0xC81U  /*!< Upper 32 bits of time, RV32 only */
#define CSR_INSTRETH                        0xC82U  /*!< Upper 32 bits of instret, RV32 only */

#if defined(__RV32)

/* CSR_MSTATUS */

/** @brief CSR_MSTATUS[3]: Machine global interrupt enable */
#define CSR_MSTATUS_MIE_Pos                 (3U)
#define CSR_MSTATUS_MIE_Msk                 (0x1UL << CSR_MSTATUS_MIE_Pos)
/** @brief CSR_MSTATUS[7]: Machine interrupt enable at the time of the trap */
#define CSR_MSTATUS_MPIE_Pos                (7U)
#define CSR_MSTATUS_MPIE_Msk                (0x1UL << CSR_MSTATUS_MPIE_Pos)
/** @brief CSR_MSTATUS[12:11]: Machine privilege mode at the time of the trap */
#define CSR_MSTATUS_MPP_Pos                 (11U)
#define CSR_MSTATUS_MPP_Msk                 (0x3UL << CSR_MSTATUS_MPP_Pos)

/* CSR_MIE */

/** @brief CSR_MIE[1]: Supervisor software interrupt enable */
#define CSR_MIE_SSIE_Pos                    (1U)
#define CSR_MIE_SSIE_Msk                    (0x1UL << CSR_MIE_SSIE_Pos)
/** @brief CSR_MIE[3]: Machine software interrupt enable */
#define CSR_MIE_MSIE_Pos                    (3U)
#define CSR_MIE_MSIE_Msk                    (0x1UL << CSR_MIE_MSIE_Pos)
/** @brief CSR_MIE[5]: Supervisor timer interrupt enable */
#define CSR_MIE_STIE_Pos                    (5U)
#define CSR_MIE_STIE_Msk                    (0x1UL << CSR_MIE_STIE_Pos)
/** @brief CSR_MIE[7]: Machine timer interrupt enable */
#define CSR_MIE_MTIE_Pos                    (7U)
#define CSR_MIE_MTIE_Msk                    (0x1UL << CSR_MIE_MTIE_Pos)
/** @brief CSR_MIE[9]: Supervisor external interrupt enable */
#define CSR_MIE_SEIE_Pos                    (9U)
#define CSR_MIE_SEIE_Msk                    (0x1UL << CSR_MIE_SEIE_Pos)
/** @brief CSR_MIE[11]: Machine external interrupt enable */
#define CSR_MIE_MEIE_Pos                    (11U)
#define CSR_MIE_MEIE_Msk                    (0x1UL << CSR_MIE_MEIE_Pos)

/* CSR_MTVEC - CLINT mode */

/** @brief CSR_MTVEC[1:0]: Interrupt handling mode */
#define CSR_MTVEC_CLINT_MODE_Pos            (0U)
#define CSR_MTVEC_CLINT_MODE_Msk            (0x3UL << CSR_MTVEC_CLINT_MODE_Pos)
/** @brief CSR_MTVEC[31:2]: Trap vector base address */
#define CSR_MTVEC_CLINT_BASE_Pos            (2U)
#define CSR_MTVEC_CLINT_BASE_Msk            (0x3FFFFFFFUL << CSR_MTVEC_CLINT_BASE_Pos)

/* CSR_MTVEC - CLIC mode */

/** @brief CSR_MTVEC[5:0]: Interrupt handling mode */
#define CSR_MTVEC_CLIC_MODE_Pos             (0U)
#define CSR_MTVEC_CLIC_MODE_Msk             (0x3FUL << CSR_MTVEC_CLIC_MODE_Pos)
/** @brief CSR_MTVEC[31:6]: Trap base address */
#define CSR_MTVEC_CLIC_BASE_Pos             (6U)
#define CSR_MTVEC_CLIC_BASE_Msk             (0x03FFFFFFUL << CSR_MTVEC_CLIC_BASE_Pos)

/* Interrupt handling mode definitions valid for both CLINT and CLIC mode */
#define CSR_MTVEC_MODE_CLINT_DIRECT         0x00000000UL    /*!< CLINT mode - Direct */
#define CSR_MTVEC_MODE_CLINT_VECTORED       0x00000001UL    /*!< CLINT mode - Vectored */
#define CSR_MTVEC_MODE_CLIC                 0x00000003UL    /*!< CLIC mode */

/* CSR_MTVT */

/** @brief CSR_MTVEC[31:6]: Trap vector base address */
#define CSR_MTVT_BASE_Pos                   (6U)
#define CSR_MTVT_BASE_Msk                   (0x3FFFFFFUL << CSR_MTVT_BASE_Pos)

/* CSR_MCAUSE */

/** @brief CSR_MCAUSE[30:0]: Exception code */
#define CSR_MCAUSE_CODE_Pos                 (0U)
#define CSR_MCAUSE_CODE_Msk                 (0x7FFFFFFFUL << CSR_MCAUSE_CODE_Pos)
/** @brief CSR_MCAUSE[31]: Interrupt flag */
#define CSR_MCAUSE_INT_Pos                  (31U)
#define CSR_MCAUSE_INT_Msk                  (0x1UL << CSR_MCAUSE_INT_Pos)

#define CSR_MCAUSE_INT_EXCEPTION            (0x0UL << CSR_MCAUSE_INT_Pos)   /*!< The trap was caused by an exception */
#define CSR_MCAUSE_INT_INTERRUPT            (0x1UL << CSR_MCAUSE_INT_Pos)   /*!< The trap was caused by an interrupt */

/* CSR_MIP */

/** @brief CSR_MIP[1]: Supervisor software interrupt pending */
#define CSR_MIP_SSIP_Pos                    (1U)
#define CSR_MIP_SSIP_Msk                    (0x1UL << CSR_MIP_SSIP_Pos)
/** @brief CSR_MIP[3]: Machine software interrupt pending */
#define CSR_MIP_MSIP_Pos                    (3U)
#define CSR_MIP_MSIP_Msk                    (0x1UL << CSR_MIP_MSIP_Pos)
/** @brief CSR_MIP[5]: Supervisor timer interrupt pending */
#define CSR_MIP_STIP_Pos                    (5U)
#define CSR_MIP_STIP_Msk                    (0x1UL << CSR_MIP_STIP_Pos)
/** @brief CSR_MIP[7]: Machine timer interrupt pending */
#define CSR_MIP_MTIP_Pos                    (7U)
#define CSR_MIP_MTIP_Msk                    (0x1UL << CSR_MIP_MTIP_Pos)
/** @brief CSR_MIP[9]: Supervisor external interrupt pending */
#define CSR_MIP_SEIP_Pos                    (9U)
#define CSR_MIP_SEIP_Msk                    (0x1UL << CSR_MIP_SEIP_Pos)
/** @brief CSR_MIP[11]: Machine external interrupt pending */
#define CSR_MIP_MEIP_Pos                    (11U)
#define CSR_MIP_MEIP_Msk                    (0x1UL << CSR_MIP_MEIP_Pos)

/* CSR_MINTTHRESH */

/** @brief CSR_MINTTHRESH[7:0]: Interrupt level threshold */
#define CSR_MINTTHRESH_TH_Pos               (0U)
#define CSR_MINTTHRESH_TH_Msk               (0xFFUL << CSR_MINTTHRESH_TH_Pos)

#endif /* __RV32 */

/* CLINT Registers */
#define CLINT_BASE                          0x02000000UL                        /*!< CLINT base address */

#define CLINT_MSIP0                         CLINT_BASE                          /*!< Machine software interrupt register address */
#define CLINT_MTIMECMP0                     (CLINT_BASE + 0x00004000UL)         /*!< Machine timer compare register address */
#define CLINT_MTIMECMP0L                    (CLINT_BASE + 0x00004000UL)         /*!< Lower 32 bits of MTIMECMP0 */
#define CLINT_MTIMECMP0H                    (CLINT_BASE + 0x00004004UL)         /*!< Upper 32 bits of MTIMECMP0 */
#define CLINT_MTIME                         (CLINT_BASE + 0x0000BFF8UL)         /*!< Machine timer counter register address */
#define CLINT_MTIMEL                        (CLINT_BASE + 0x0000BFF8UL)         /*!< Lower 32 bits of MTIME */
#define CLINT_MTIMEH                        (CLINT_BASE + 0x0000BFFCUL)         /*!< Upper 32 bits of MTIME */

/* CLINT_MSIP0 */

/** @brief CLINT_MSIP0[0]: Machine software interrupt pending */
#define CLINT_MSIP0_IP_Pos                  (0U)
#define CLINT_MSIP0_IP_Msk                  (0x1UL << CLINT_MSIP0_IP_Pos)

/* CLIC Registers */
#define CLIC_BASE                           0x0D000000UL                        /*!< CLIC base address */

#define CLIC_CFG                            CLIC_BASE                           /*!< Configuration register address */
#define CLIC_INFO                           (CLIC_BASE + 0x00000004UL)          /*!< Information register address */
#define CLIC_INTTRIG                        (CLIC_BASE + 0x00000040UL)          /*!< Interrupt trigger registers base address */
#define CLIC_INTIP                          (CLIC_BASE + 0x00001000UL)          /*!< Interrupt pending registers base address */
#define CLIC_INTIE                          (CLIC_BASE + 0x00001001UL)          /*!< Interrupt enable registers base address */
#define CLIC_INTATTR                        (CLIC_BASE + 0x00001002UL)          /*!< Interrupt attribute registers base address */
#define CLIC_INTCTL                         (CLIC_BASE + 0x00001003UL)          /*!< Interrupt input control registers base address */

/* CLIC_CFG */

/** @brief CLIC_CFG[0]: Selective interrupt hardware vectoring feature implemented flag */
#define CLIC_CFG_NVBITS_Pos                 (0U)
#define CLIC_CFG_NVBITS_Msk                 (0x1U << CLIC_CFG_NVBITS_Pos)
/** @brief CLIC_CFG[4:1]: Machine bits assigned to interrupt level */
#define CLIC_CFG_NLBITS_Pos                 (1U)
#define CLIC_CFG_NLBITS_Msk                 (0xFU << CLIC_CFG_NLBITS_Pos)
/** @brief CLIC_CFG[6:5]: Bits assigned to interrupt privilege mode */
#define CLIC_CFG_NMBITS_Pos                 (5U)
#define CLIC_CFG_NMBITS_Msk                 (0x3U << CLIC_CFG_NMBITS_Pos)

/* CLIC_INFO */

/** @brief CLIC_INFO[12:0]: Number of maximum interrupt inputs supported */
#define CLIC_INFO_NUM_INT_Pos               (0U)
#define CLIC_INFO_NUM_INT_Msk               (0x1FFFUL << CLIC_INFO_NUM_INT_Pos)
/** @brief CLIC_INFO[20:13]: Version */
#define CLIC_INFO_VERSION_Pos               (13U)
#define CLIC_INFO_VERSION_Msk               (0xFFUL << CLIC_INFO_VERSION_Pos)
/** @brief CLIC_INFO[24:21]: Number of bits implemented in CLICINTCTL registers */
#define CLIC_INFO_CLICINTCTLBITS_Pos        (21U)
#define CLIC_INFO_CLICINTCTLBITS_Msk        (0xFUL << CLIC_INFO_CLICINTCTLBITS_Pos)
/** @brief CLIC_INFO[30:25]: Number of maximum interrupt triggers supported */
#define CLIC_INFO_NUM_TRIGGER_Pos           (25U)
#define CLIC_INFO_NUM_TRIGGER_Msk           (0x3FUL << CLIC_INFO_NUM_TRIGGER_Pos)

/* CLIC_INTTRIG */

/** @brief CLIC_INTTRIG[12:0]: Source for the interrupt trigger */
#define CLIC_INTTRIG_INT_NUM_Pos            (0U)
#define CLIC_INTTRIG_INT_NUM_Msk            (0x1FFFUL << CLIC_INTTRIG_INT_NUM_Pos)
/** @brief CLIC_INTTRIG[31]: Interrupt trigger enable */
#define CLIC_INTTRIG_ENABLE_Pos             (31U)
#define CLIC_INTTRIG_ENABLE_Msk             (0x1UL << CLIC_INTTRIG_ENABLE_Pos)

/* CLIC_INTIP */

/** @brief CLIC_INTIP[0]: Interrupt pending */
#define CLIC_INTIP_IP_Pos                   (0U)
#define CLIC_INTIP_IP_Msk                   (0x1U << CLIC_INTIP_IP_Pos)

/* CLIC_INTIE */

/** @brief CLIC_INTIE[0]: Interrupt enable */
#define CLIC_INTIE_IE_Pos                   (0U)
#define CLIC_INTIE_IE_Msk                   (0x1U << CLIC_INTIE_IE_Pos)

/* CLIC_INTATTR */

/** @brief CLIC_INTATTR[0]: Selective hardware vectoring */
#define CLIC_INTATTR_SHV_Pos                (0U)
#define CLIC_INTATTR_SHV_Msk                (0x1U << CLIC_INTATTR_SHV_Pos)

#define CLIC_INTATTR_SHV_DIRECT             (0x0U << CLIC_INTATTR_SHV_Pos)  /*!< Non-vectored interrupt */
#define CLIC_INTATTR_SHV_VECTORED           (0x1U << CLIC_INTATTR_SHV_Pos)  /*!< Vectored interrupt */

/** @brief CLIC_INTATTR[1]: Trigger type and for the interrupt input */
#define CLIC_INTATTR_TRIG_TYPE_Pos          (1U)
#define CLIC_INTATTR_TRIG_TYPE_Msk          (0x1U << CLIC_INTATTR_TRIG_TYPE_Pos)

#define CLIC_INTATTR_TRIG_TYPE_LEVEL        (0x0U << CLIC_INTATTR_TRIG_TYPE_Pos)    /*!< Level triggered interrupt */
#define CLIC_INTATTR_TRIG_TYPE_EDGE         (0x1U << CLIC_INTATTR_TRIG_TYPE_Pos)    /*!< Edge triggered interrupt */

/** @brief CLIC_INTATTR[2]: Trigger polarity for the interrupt input */
#define CLIC_INTATTR_TRIG_POL_Pos           (2U)
#define CLIC_INTATTR_TRIG_POL_Msk           (0x1U << CLIC_INTATTR_TRIG_POL_Pos)

#define CLIC_INTATTR_TRIG_POL_P             (0x0U << CLIC_INTATTR_TRIG_POL_Pos) /*!< Positive edge trigger */
#define CLIC_INTATTR_TRIG_POL_N             (0x1U << CLIC_INTATTR_TRIG_POL_Pos) /*!< Negative edge trigger */

/** @brief CLIC_INTATTR[7:6]: Privilege mode of the interrupt */
#define CLIC_INTATTR_MODE_Pos               (6U)
#define CLIC_INTATTR_MODE_Msk               (0x3U << CLIC_INTATTR_MODE_Pos)

#define CLIC_INTATTR_MODE_USER              (0x0U << CLIC_INTATTR_MODE_Pos) /*!< User mode */
#define CLIC_INTATTR_MODE_SUPERVISOR        (0x1U << CLIC_INTATTR_MODE_Pos) /*!< Supervisor mode */
#define CLIC_INTATTR_MODE_MACHINE           (0x3U << CLIC_INTATTR_MODE_Pos) /*!< Machine mode */

typedef unsigned long rv_csr_t;

#endif /* CORE_RISCV_REG_H */
