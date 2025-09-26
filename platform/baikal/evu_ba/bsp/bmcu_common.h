/**
 * *****************************************************************************
 *  @file       bmcu_common.h
 *  @author     Baikal electronics SDK team
 *  @brief      BASIS Baikal MCU common definitions
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

#ifndef BMCU_COMMON_H
#define BMCU_COMMON_H

#define BMCU_U     1
#undef BMCU_UM

#if defined (BMCU_U)
#include "bmcu_u.h"
#endif
#if defined (BMCU_UM)
#include "bmcu_um.h"
#endif

#if !defined (BMCU_U) && !defined (BMCU_UM)
#error "Please select the target BMCU device."
#endif

/* bit operations */
#define REG32(addr)                  (*(volatile uint32_t *)(uint32_t)(addr))
#define REG16(addr)                  (*(volatile uint16_t *)(uint32_t)(addr))
#define REG8(addr)                   (*(volatile uint8_t *)(uint32_t)(addr))
#define BIT(x)                       ((uint32_t)((uint32_t)0x01U<<(x)))
#define BITS(end, start)             ((0xFFFFFFFFUL << (start)) & (0xFFFFFFFFUL >> (31U - (uint32_t)(end)))) 
#define GET_BITS(regval, end, start) (((regval) & BITS((end), (start))) >> (start))

/* PoC bit operations */
#define REG_READ32(REG, VAR)  (*(uint32_t *) &(VAR) = *(volatile uint32_t *) &(REG))
#define REG_WRITE32(REG, VAR) (*(volatile uint32_t *) &(REG) = *(uint32_t *) &(VAR))

/*
 * Usage example: Disable timer
 * 
 * TIMER_CONTROL_TypeDef ctrl;
 * REG_MODIFY32(TIMER0->CONTROL0, ctrl, ctrl.TIMER_ENABLE = 1; ctrl.TIMER_MODE = 1);
 *     TIM_CONTROL(TIMx, TIM_Cnannel) |= (TIM_CONTROL_ENABLE_Msk | TIM_CONTROL_INTERRUPT_MASK_Msk);
 */
#define REG_MODIFY32(REG, VAR, VAR_MODIFY_BLOCK)  \
do {                                              \
     REG_READ32(REG, VAR);                        \
     VAR_MODIFY_BLOCK;                            \
     REG_WRITE32(REG, VAR);                       \
} while(0)


/* Register bitwise operations */
#define SET_BIT(REG, BIT)                       ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)                     ((REG) &= ~(BIT))
#define READ_BIT(REG, BIT)                      ((REG) & (BIT))
#define WRITE_REG(REG, VAL)                     ((REG) = (VAL))
#define CLEAR_REG(REG)                          ((REG) = (0x0))
#define MODIFY_REG(REG, CLEARMASK, SETMASK)     WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))
#define READ_REG(REG)                           ((REG))
#define POSITION_VAL(VAL)                       (__CTZ(VAL))


/**
 * @brief Counts the number of leading zeros of a data value.
 * @param val The value to count leading zeros.
 * @return The number of leading zeros in the value.
 */
#define __CLZ(val)                  ((uint8_t)__builtin_clz(val))

/**
 * @brief Counts the number of trailing zeros of a data value.
 * @param val The value to count trailing zeros.
 * @return The number of trailing zeros in the value.
 */
#define __CTZ(val)                  ((uint8_t)__builtin_ctz(val))


#define CORE_BR350S1

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

#endif /* BMCU_COMMON_H */
