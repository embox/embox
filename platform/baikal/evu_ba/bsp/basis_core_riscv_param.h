/**
 * *****************************************************************************
 *  @file       basis_core_riscv_param.h
 *  @author     Baikal electronics SDK team
 *  @brief      BASIS RISC-V constant definitions
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

#ifndef BASIS_CORE_RISCV_PARAM_H
#define BASIS_CORE_RISCV_PARAM_H

#define CORE_BR350S1

#if !defined (CORE_BR310S14) && !defined (CORE_BR350S1)
#error "Please select first the MCU core used in your application"
#endif

#if defined(CORE_BR310S14)

/* RISC-V ISA */
#define __RV32

/* Core-Local Interrupt Controller (CLIC) */
#define __RV_CLIC_EN                (0U)    /*!< CLIC is not implemented */
#define __RV_NUM_INTERRUPT          (32UL)  /*!< The number of interrupts (16 legacy included) implemented */
#define __RV_N_LOCAL_INT            (16UL)  /*!< The number of local interrupts implemented */
#define __RV_N_NMI                  (0UL)   /*!< The number of NMI interrupts implemented */
#define __RV_CLICINTCTLBITS         (4U)    /*!< The number of bits implemented in CLICINTCTL registers */

#endif /* CORE_BR310S14 */

#if defined(CORE_BR350S1)

/* RISC-V ISA */
#define __RV32

/* Core-Local Interrupt Controller (CLIC) */
#define __RV_CLIC_EN                (1U)    /*!< CLIC is implemented */
#define __RV_CLICANDBASIC           (0U)    /*!< Only CLIC mode is implemented */
#define __RV_NUM_INTERRUPT          (128UL) /*!< The number of interrupts (16 legacy included) implemented */
#define __RV_CLICMAXID              (127UL) /*!< ID of the last interrupt */
#define __RV_N_LOCAL_INT            (112UL) /*!< The number of local interrupts implemented */
#define __RV_N_NMI                  (1UL)   /*!< The number of NMI interrupts implemented */
#define __RV_CLICPRIVMODES          (2U)    /*!< M/U privilege modes for the interrupts are implemented */
#define __RV_CLICCFGMBITS           (1U)    /*!< The number of bits in NMBITS field of CLICCFG registers implemented */
#define __RV_CLICINTCTLBITS         (4U)    /*!< The number of bits implemented in CLICINTCTL registers */
#define __RV_CLICCFGLBITS           (4U)    /*!< The number of bits in NLBITS field of CLICCFG registers implemented */
#define __RV_CLICSELHVEC            (1U)    /*!< Hardware vector interrupts support is implemented */
#define __RV_CLICMTVECALIGN         (6U)    /*!< The address in XTVEC register is 64 bytes aligned */
#define __RV_CLICXNXTI              (1U)    /*!< XNXTI CSR is implemented */
#define __RV_CLICXCSW               (1U)    /*!< XSCRATCHCSW/XSCRATCHCSWL CSRs are implemented */

#endif /* CORE_BR350S1 */

#endif /* BASIS_CORE_RISCV_PARAM_H */
