/**
 *******************************************************************************
 * @file    MDR32VF0xI_rst_clk.h
 * @author  Milandr Application Team
 * @version V0.2.0
 * @date    07/05/2025
 * @brief   This file contains all the functions prototypes for the RST_CLK
 *          firmware library.
 *******************************************************************************
 * <br><br>
 *
 * THE PRESENT FIRMWARE IS FOR GUIDANCE ONLY. IT AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING MILANDR'S PRODUCTS IN ORDER TO FACILITATE
 * THE USE AND SAVE TIME. MILANDR SHALL NOT BE HELD LIABLE FOR ANY
 * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES RESULTING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR A USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN THEIR PRODUCTS.
 *
 * <h2><center>&copy; COPYRIGHT 2025 Milandr</center></h2>
 *******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MDR32VF0xI_RST_CLK_H
#define MDR32VF0xI_RST_CLK_H

#include <stdint.h>

#include "mdr1206fi.h"

typedef enum {
    RESET = 0,
    SET   = 1
} FlagStatus,
    ITStatus,
    BitStatus;


typedef enum {
    DISABLE = 0,
    ENABLE  = 1
} FunctionalState;


typedef enum {
    ERROR   = 0,
    SUCCESS = 1
} ErrorStatus;

/**
 * @brief RST_CLK HSE (High Speed External) clock mode and source selection constants.
 */
typedef enum {
    RST_CLK_HSE_OFF    = ((uint32_t)0x0),                                         /*!< Switch off the HSE clock generator. */
    RST_CLK_HSE_ON     = RST_CLK_HS_CONTROL_HSE_ON,                               /*!< Switch on the HSE clock generator. */
    RST_CLK_HSE_BYPASS = (RST_CLK_HS_CONTROL_HSE_ON | RST_CLK_HS_CONTROL_HSE_BYP) /*!< Use an external clock source. */
} RST_CLK_HSE_Mode_TypeDef;

#define IS_RST_CLK_HSE(HSE) (((HSE) == RST_CLK_HSE_OFF) || \
                             ((HSE) == RST_CLK_HSE_ON) ||  \
                             ((HSE) == RST_CLK_HSE_BYPASS))

/**
 * @brief RST_CLK CPU_PLL clock source selection constants.
 */
typedef enum {
    RST_CLK_PLLCPU_CLK_SRC_HSE    = ((uint32_t)0),
    RST_CLK_PLLCPU_CLK_SRC_CPU_C1 = ((uint32_t)RST_CLK_PLL_CONTROL_PLL_CPU_SEL)
} RST_CLK_PLLCPU_ClockSource_TypeDef;

#define IS_RST_CLK_PLLCPU_CLOCK_SOURCE(CLK) (((CLK) == RST_CLK_PLLCPU_CLK_SRC_HSE) || \
                                             ((CLK) == RST_CLK_PLLCPU_CLK_SRC_CPU_C1))

/**
 * @brief RST_CLK CPU_C1 clock source selection constants.
 */
typedef enum {
    RST_CLK_CPU_C1_CLK_SRC_HSI       = ((uint32_t)RST_CLK_CPU_CLOCK_CPU_C1_SEL_HSI),       /*!< HSI as the CPU_C1 source. */
    RST_CLK_CPU_C1_CLK_SRC_HSI_DIV_2 = ((uint32_t)RST_CLK_CPU_CLOCK_CPU_C1_SEL_HSI_DIV_2), /*!< HSI/2 as the CPU_C1 source. */
    RST_CLK_CPU_C1_CLK_SRC_HSE       = ((uint32_t)RST_CLK_CPU_CLOCK_CPU_C1_SEL_HSE),       /*!< HSE as the CPU_C1 source. */
    RST_CLK_CPU_C1_CLK_SRC_HSE_DIV_2 = ((uint32_t)RST_CLK_CPU_CLOCK_CPU_C1_SEL_HSE_DIV_2)  /*!< HSE/2 as the CPU_C1 source. */
} RST_CLK_CPU_C1_ClockSource_TypeDef;

#define IS_RST_CLK_CPU_C1_CLOCK_SOURCE(CLK) (((CLK) == RST_CLK_CPU_C1_CLK_SRC_HSI) ||       \
                                             ((CLK) == RST_CLK_CPU_C1_CLK_SRC_HSI_DIV_2) || \
                                             ((CLK) == RST_CLK_CPU_C1_CLK_SRC_HSE) ||       \
                                             ((CLK) == RST_CLK_CPU_C1_CLK_SRC_HSE_DIV_2))

/**
 * @brief RST_CLK PLL_CPU_MUL multiplier constants.
 */
typedef enum {
    RST_CLK_PLLCPU_MUL_1 = ((uint32_t)0x0),
    RST_CLK_PLLCPU_MUL_2 = ((uint32_t)0x1),
    RST_CLK_PLLCPU_MUL_3 = ((uint32_t)0x2),
    RST_CLK_PLLCPU_MUL_4 = ((uint32_t)0x3),
    RST_CLK_PLLCPU_MUL_5 = ((uint32_t)0x4),
    RST_CLK_PLLCPU_MUL_6 = ((uint32_t)0x5),
    RST_CLK_PLLCPU_MUL_7 = ((uint32_t)0x6),
    RST_CLK_PLLCPU_MUL_8 = ((uint32_t)0x7)
} RST_CLK_PLLCPU_Multiplier_TypeDef;

#define IS_RST_CLK_PLLCPU_MUL(MUL) (((MUL) & ~((uint32_t)0x7)) == 0x0)

/**
 * @brief RST_CLK CPU_C2 clock source selection constants.
 */
typedef enum {
    RST_CLK_CPU_C2_CLK_SRC_CPU_C1 = (((uint32_t)0x0) << RST_CLK_CPU_CLOCK_CPU_C2_SEL_Pos), /*!< CPU_C1 as CPU_C2 source. */
    RST_CLK_CPU_C2_CLK_SRC_PLLCPU = (((uint32_t)0x1) << RST_CLK_CPU_CLOCK_CPU_C2_SEL_Pos)  /*!< CPU_PLL as CPU_C2 source. */
} RST_CLK_CPU_C2_ClockSource_TypeDef;

#define IS_RST_CLK_CPU_C2_CLOCK_SOURCE(CLK) (((CLK) == RST_CLK_CPU_C2_CLK_SRC_CPU_C1) || \
                                             ((CLK) == RST_CLK_CPU_C2_CLK_SRC_PLLCPU))

/**
 * @brief RST_CLK CPU_C3 divider constants.
 */
typedef enum {
    RST_CLK_CPU_C3_PRESCALER_DIV_1   = (((uint32_t)0x0) << RST_CLK_CPU_CLOCK_CPU_C3_SEL_Pos),
    RST_CLK_CPU_C3_PRESCALER_DIV_2   = (((uint32_t)0x8) << RST_CLK_CPU_CLOCK_CPU_C3_SEL_Pos),
    RST_CLK_CPU_C3_PRESCALER_DIV_4   = (((uint32_t)0x9) << RST_CLK_CPU_CLOCK_CPU_C3_SEL_Pos),
    RST_CLK_CPU_C3_PRESCALER_DIV_8   = (((uint32_t)0xA) << RST_CLK_CPU_CLOCK_CPU_C3_SEL_Pos),
    RST_CLK_CPU_C3_PRESCALER_DIV_16  = (((uint32_t)0xB) << RST_CLK_CPU_CLOCK_CPU_C3_SEL_Pos),
    RST_CLK_CPU_C3_PRESCALER_DIV_32  = (((uint32_t)0xC) << RST_CLK_CPU_CLOCK_CPU_C3_SEL_Pos),
    RST_CLK_CPU_C3_PRESCALER_DIV_64  = (((uint32_t)0xD) << RST_CLK_CPU_CLOCK_CPU_C3_SEL_Pos),
    RST_CLK_CPU_C3_PRESCALER_DIV_128 = (((uint32_t)0xE) << RST_CLK_CPU_CLOCK_CPU_C3_SEL_Pos),
    RST_CLK_CPU_C3_PRESCALER_DIV_256 = (((uint32_t)0xF) << RST_CLK_CPU_CLOCK_CPU_C3_SEL_Pos)
} RST_CLK_CPU_C3_Prescaler_TypeDef;

#define IS_RST_CLK_CPU_C3_DIV(DIV) (((DIV) == RST_CLK_CPU_C3_PRESCALER_DIV_1) ||   \
                                    ((DIV) == RST_CLK_CPU_C3_PRESCALER_DIV_2) ||   \
                                    ((DIV) == RST_CLK_CPU_C3_PRESCALER_DIV_4) ||   \
                                    ((DIV) == RST_CLK_CPU_C3_PRESCALER_DIV_8) ||   \
                                    ((DIV) == RST_CLK_CPU_C3_PRESCALER_DIV_16) ||  \
                                    ((DIV) == RST_CLK_CPU_C3_PRESCALER_DIV_32) ||  \
                                    ((DIV) == RST_CLK_CPU_C3_PRESCALER_DIV_64) ||  \
                                    ((DIV) == RST_CLK_CPU_C3_PRESCALER_DIV_128) || \
                                    ((DIV) == RST_CLK_CPU_C3_PRESCALER_DIV_256))

/**
 * @brief RST_CLK HCLK clock source selection constants.
 */
typedef enum {
    RST_CLK_CPU_HCLK_CLK_SRC_HSI    = ((uint32_t)RST_CLK_CPU_CLOCK_HCLK_SEL_HSI),
    RST_CLK_CPU_HCLK_CLK_SRC_CPU_C3 = ((uint32_t)RST_CLK_CPU_CLOCK_HCLK_SEL_CPU_C3),
    RST_CLK_CPU_HCLK_CLK_SRC_LSE    = ((uint32_t)RST_CLK_CPU_CLOCK_HCLK_SEL_LSE),
    RST_CLK_CPU_HCLK_CLK_SRC_LSI    = ((uint32_t)RST_CLK_CPU_CLOCK_HCLK_SEL_LSI)
} RST_CLK_HCLK_ClockSource_TypeDef;

#define IS_RST_CLK_HCLK_CLOCK_SOURCE(CLK) (((CLK) == RST_CLK_CPU_HCLK_CLK_SRC_HSI) ||    \
                                           ((CLK) == RST_CLK_CPU_HCLK_CLK_SRC_CPU_C3) || \
                                           ((CLK) == RST_CLK_CPU_HCLK_CLK_SRC_LSE) ||    \
                                           ((CLK) == RST_CLK_CPU_HCLK_CLK_SRC_LSI))

/**
 * @brief RST_CLK ADC_CLK clock source selection constants.
 */
typedef enum {
    RST_CLK_ADC_CLK_SRC_CPU_C1     = ((uint32_t)RST_CLK_ADC_CLOCK_ADC_C1_SEL_CPU_C1),
    RST_CLK_ADC_CLK_SRC_PER1_C1    = ((uint32_t)RST_CLK_ADC_CLOCK_ADC_C1_SEL_PER1_C1),
    RST_CLK_ADC_CLK_SRC_PLLCPU     = ((uint32_t)RST_CLK_ADC_CLOCK_ADC_C1_SEL_PLLCPU),
    RST_CLK_ADC_CLK_SRC_HSE_C1_CLK = ((uint32_t)RST_CLK_ADC_CLOCK_ADC_C1_SEL_HSE_C1_CLK)
} RST_CLK_ADC_ClockSource_TypeDef;

#define IS_RST_CLK_ADC_CLOCK_SOURCE(CLK) (((CLK) == RST_CLK_ADC_CLK_SRC_CPU_C1) ||  \
                                          ((CLK) == RST_CLK_ADC_CLK_SRC_PER1_C1) || \
                                          ((CLK) == RST_CLK_ADC_CLK_SRC_PLLCPU) ||  \
                                          ((CLK) == RST_CLK_ADC_CLK_SRC_HSE_C1_CLK))

/**
 * @brief RST_CLK ADC_C3_SEL divider constants.
 */
typedef enum {
    RST_CLK_ADC_PRESCALER_DIV_1   = (((uint32_t)0x0) << RST_CLK_ADC_CLOCK_ADC_C3_SEL_Pos),
    RST_CLK_ADC_PRESCALER_DIV_2   = (((uint32_t)0x8) << RST_CLK_ADC_CLOCK_ADC_C3_SEL_Pos),
    RST_CLK_ADC_PRESCALER_DIV_4   = (((uint32_t)0x9) << RST_CLK_ADC_CLOCK_ADC_C3_SEL_Pos),
    RST_CLK_ADC_PRESCALER_DIV_8   = (((uint32_t)0xA) << RST_CLK_ADC_CLOCK_ADC_C3_SEL_Pos),
    RST_CLK_ADC_PRESCALER_DIV_16  = (((uint32_t)0xB) << RST_CLK_ADC_CLOCK_ADC_C3_SEL_Pos),
    RST_CLK_ADC_PRESCALER_DIV_32  = (((uint32_t)0xC) << RST_CLK_ADC_CLOCK_ADC_C3_SEL_Pos),
    RST_CLK_ADC_PRESCALER_DIV_64  = (((uint32_t)0xD) << RST_CLK_ADC_CLOCK_ADC_C3_SEL_Pos),
    RST_CLK_ADC_PRESCALER_DIV_128 = (((uint32_t)0xE) << RST_CLK_ADC_CLOCK_ADC_C3_SEL_Pos),
    RST_CLK_ADC_PRESCALER_DIV_256 = (((uint32_t)0xF) << RST_CLK_ADC_CLOCK_ADC_C3_SEL_Pos)
} RST_CLK_ADC_C3_Prescaler_TypeDef;

#define IS_RST_CLK_ADC_C3_DIV(DIV) (((DIV) == RST_CLK_ADC_PRESCALER_DIV_1) ||   \
                                    ((DIV) == RST_CLK_ADC_PRESCALER_DIV_2) ||   \
                                    ((DIV) == RST_CLK_ADC_PRESCALER_DIV_4) ||   \
                                    ((DIV) == RST_CLK_ADC_PRESCALER_DIV_8) ||   \
                                    ((DIV) == RST_CLK_ADC_PRESCALER_DIV_16) ||  \
                                    ((DIV) == RST_CLK_ADC_PRESCALER_DIV_32) ||  \
                                    ((DIV) == RST_CLK_ADC_PRESCALER_DIV_64) ||  \
                                    ((DIV) == RST_CLK_ADC_PRESCALER_DIV_128) || \
                                    ((DIV) == RST_CLK_ADC_PRESCALER_DIV_256))

/**
 * @brief RST_CLK ADCUI_CLK clock source selection constants.
 */
typedef enum {
    RST_CLK_ADCUI_CLK_SRC_CPU_C1     = ((uint32_t)RST_CLK_ADC_CLOCK_ADCUI_C1_SEL_CPU_C1),
    RST_CLK_ADCUI_CLK_SRC_PER1_C1    = ((uint32_t)RST_CLK_ADC_CLOCK_ADCUI_C1_SEL_PER1_C1),
    RST_CLK_ADCUI_CLK_SRC_PLLCPU     = ((uint32_t)RST_CLK_ADC_CLOCK_ADCUI_C1_SEL_PLLCPU),
    RST_CLK_ADCUI_CLK_SRC_HSE_C1_CLK = ((uint32_t)RST_CLK_ADC_CLOCK_ADCUI_C1_SEL_HSE_C1_CLK)
} RST_CLK_ADCUI_ClockSource_TypeDef;

#define IS_RST_CLK_ADCUI_CLOCK_SOURCE(CLK) (((CLK) == RST_CLK_ADCUI_CLK_SRC_CPU_C1) ||  \
                                            ((CLK) == RST_CLK_ADCUI_CLK_SRC_PER1_C1) || \
                                            ((CLK) == RST_CLK_ADCUI_CLK_SRC_PLLCPU) ||  \
                                            ((CLK) == RST_CLK_ADCUI_CLK_SRC_HSE_C1_CLK))

/**
 * @brief RST_CLK ADCUI_C3_SEL divider constants.
 */
typedef enum {
    RST_CLK_ADCUI_PRESCALER_DIV_1   = (((uint32_t)0x0) << RST_CLK_ADC_CLOCK_ADCUI_C3_SEL_Pos),
    RST_CLK_ADCUI_PRESCALER_DIV_2   = (((uint32_t)0x8) << RST_CLK_ADC_CLOCK_ADCUI_C3_SEL_Pos),
    RST_CLK_ADCUI_PRESCALER_DIV_4   = (((uint32_t)0x9) << RST_CLK_ADC_CLOCK_ADCUI_C3_SEL_Pos),
    RST_CLK_ADCUI_PRESCALER_DIV_8   = (((uint32_t)0xA) << RST_CLK_ADC_CLOCK_ADCUI_C3_SEL_Pos),
    RST_CLK_ADCUI_PRESCALER_DIV_16  = (((uint32_t)0xB) << RST_CLK_ADC_CLOCK_ADCUI_C3_SEL_Pos),
    RST_CLK_ADCUI_PRESCALER_DIV_32  = (((uint32_t)0xC) << RST_CLK_ADC_CLOCK_ADCUI_C3_SEL_Pos),
    RST_CLK_ADCUI_PRESCALER_DIV_64  = (((uint32_t)0xD) << RST_CLK_ADC_CLOCK_ADCUI_C3_SEL_Pos),
    RST_CLK_ADCUI_PRESCALER_DIV_128 = (((uint32_t)0xE) << RST_CLK_ADC_CLOCK_ADCUI_C3_SEL_Pos),
    RST_CLK_ADCUI_PRESCALER_DIV_256 = (((uint32_t)0xF) << RST_CLK_ADC_CLOCK_ADCUI_C3_SEL_Pos)
} RST_CLK_ADCUI_C3_Prescaler_TypeDef;

#define IS_RST_CLK_ADCUI_C3_DIV(DIV) (((DIV) == RST_CLK_ADCUI_PRESCALER_DIV_1) ||   \
                                      ((DIV) == RST_CLK_ADCUI_PRESCALER_DIV_2) ||   \
                                      ((DIV) == RST_CLK_ADCUI_PRESCALER_DIV_4) ||   \
                                      ((DIV) == RST_CLK_ADCUI_PRESCALER_DIV_8) ||   \
                                      ((DIV) == RST_CLK_ADCUI_PRESCALER_DIV_16) ||  \
                                      ((DIV) == RST_CLK_ADCUI_PRESCALER_DIV_32) ||  \
                                      ((DIV) == RST_CLK_ADCUI_PRESCALER_DIV_64) ||  \
                                      ((DIV) == RST_CLK_ADCUI_PRESCALER_DIV_128) || \
                                      ((DIV) == RST_CLK_ADCUI_PRESCALER_DIV_256))

/**
 * @brief RST_CLK peripheral modules clock constants.
 */
typedef enum {
    RST_CLK_PCLK_SSP1    = RST_CLK_PER2_CLOCK_PCLK_EN_SSP1,
    RST_CLK_PCLK_UART1   = RST_CLK_PER2_CLOCK_PCLK_EN_UART1,
    RST_CLK_PCLK_UART2   = RST_CLK_PER2_CLOCK_PCLK_EN_UART2,
    RST_CLK_PCLK_FLASH   = RST_CLK_PER2_CLOCK_PCLK_EN_FLASH,
    RST_CLK_PCLK_RST_CLK = RST_CLK_PER2_CLOCK_PCLK_EN_RST_CLK,
    RST_CLK_PCLK_DMA     = RST_CLK_PER2_CLOCK_PCLK_EN_DMA,
    RST_CLK_PCLK_I2C     = RST_CLK_PER2_CLOCK_PCLK_EN_I2C,
    RST_CLK_PCLK_UART3   = RST_CLK_PER2_CLOCK_PCLK_EN_UART3,
    RST_CLK_PCLK_ADC     = RST_CLK_PER2_CLOCK_PCLK_EN_ADC,
    RST_CLK_PCLK_WWDG    = RST_CLK_PER2_CLOCK_PCLK_EN_WWDG,
    RST_CLK_PCLK_IWDG    = RST_CLK_PER2_CLOCK_PCLK_EN_IWDG,
    RST_CLK_PCLK_POWER   = RST_CLK_PER2_CLOCK_PCLK_EN_POWER,
    RST_CLK_PCLK_BKP     = RST_CLK_PER2_CLOCK_PCLK_EN_BKP,
    RST_CLK_PCLK_ADCUI   = RST_CLK_PER2_CLOCK_PCLK_EN_ADCUI,
    RST_CLK_PCLK_TIMER1  = RST_CLK_PER2_CLOCK_PCLK_EN_TIMER1,
    RST_CLK_PCLK_TIMER2  = RST_CLK_PER2_CLOCK_PCLK_EN_TIMER2,
    RST_CLK_PCLK_PORTA   = RST_CLK_PER2_CLOCK_PCLK_EN_PORTA,
    RST_CLK_PCLK_PORTB   = RST_CLK_PER2_CLOCK_PCLK_EN_PORTB,
    RST_CLK_PCLK_PORTC   = RST_CLK_PER2_CLOCK_PCLK_EN_PORTC,
    RST_CLK_PCLK_CRC     = RST_CLK_PER2_CLOCK_PCLK_EN_CRC,
#if defined(USE_MDR32F02)
    RST_CLK_PCLK_SENSORS     = RST_CLK_PER2_CLOCK_PCLK_EN_SENSORS,
    RST_CLK_PCLK_CLK_MEASURE = RST_CLK_PER2_CLOCK_PCLK_EN_CLR_MEASURE,
#endif
    RST_CLK_PCLK_RANDOM = RST_CLK_PER2_CLOCK_PCLK_EN_RANDOM,
    RST_CLK_PCLK_USART  = RST_CLK_PER2_CLOCK_PCLK_EN_ISO7816,
    RST_CLK_PCLK_SSP2   = RST_CLK_PER2_CLOCK_PCLK_EN_SSP2,
    RST_CLK_PCLK_SSP3   = RST_CLK_PER2_CLOCK_PCLK_EN_SSP3,
    RST_CLK_PCLK_TIMER3 = RST_CLK_PER2_CLOCK_PCLK_EN_TIMER3,
    RST_CLK_PCLK_TIMER4 = RST_CLK_PER2_CLOCK_PCLK_EN_TIMER4,
    RST_CLK_PCLK_UART4  = RST_CLK_PER2_CLOCK_PCLK_EN_UART4,
    RST_CLK_PCLK_PORTD  = RST_CLK_PER2_CLOCK_PCLK_EN_PORTD
} RST_CLK_PCLK_TypeDef;


#define IS_RST_CLK_PCLK(PCLK) (((PCLK) == RST_CLK_PCLK_SSP1) ||    \
                               ((PCLK) == RST_CLK_PCLK_UART1) ||   \
                               ((PCLK) == RST_CLK_PCLK_UART2) ||   \
                               ((PCLK) == RST_CLK_PCLK_FLASH) ||   \
                               ((PCLK) == RST_CLK_PCLK_RST_CLK) || \
                               ((PCLK) == RST_CLK_PCLK_DMA) ||     \
                               ((PCLK) == RST_CLK_PCLK_I2C) ||     \
                               ((PCLK) == RST_CLK_PCLK_UART3) ||   \
                               ((PCLK) == RST_CLK_PCLK_ADC) ||     \
                               ((PCLK) == RST_CLK_PCLK_WWDG) ||    \
                               ((PCLK) == RST_CLK_PCLK_IWDG) ||    \
                               ((PCLK) == RST_CLK_PCLK_POWER) ||   \
                               ((PCLK) == RST_CLK_PCLK_BKP) ||     \
                               ((PCLK) == RST_CLK_PCLK_ADCUI) ||   \
                               ((PCLK) == RST_CLK_PCLK_TIMER1) ||  \
                               ((PCLK) == RST_CLK_PCLK_TIMER2) ||  \
                               ((PCLK) == RST_CLK_PCLK_PORTA) ||   \
                               ((PCLK) == RST_CLK_PCLK_PORTB) ||   \
                               ((PCLK) == RST_CLK_PCLK_PORTC) ||   \
                               ((PCLK) == RST_CLK_PCLK_CRC) ||     \
                               ((PCLK) == RST_CLK_PCLK_RANDOM) ||  \
                               ((PCLK) == RST_CLK_PCLK_USART) ||   \
                               ((PCLK) == RST_CLK_PCLK_SSP2) ||    \
                               ((PCLK) == RST_CLK_PCLK_SSP3) ||    \
                               ((PCLK) == RST_CLK_PCLK_TIMER3) ||  \
                               ((PCLK) == RST_CLK_PCLK_TIMER4) ||  \
                               ((PCLK) == RST_CLK_PCLK_UART4) ||   \
                               ((PCLK) == RST_CLK_PCLK_PORTD))

#define RST_CLK_PCLK_MASK (RST_CLK_PCLK_SSP1 | RST_CLK_PCLK_UART1 |    \
                           RST_CLK_PCLK_UART2 | RST_CLK_PCLK_FLASH |   \
                           RST_CLK_PCLK_RST_CLK | RST_CLK_PCLK_DMA |   \
                           RST_CLK_PCLK_I2C | RST_CLK_PCLK_UART3 |     \
                           RST_CLK_PCLK_ADC | RST_CLK_PCLK_WWDG |      \
                           RST_CLK_PCLK_IWDG | RST_CLK_PCLK_POWER |    \
                           RST_CLK_PCLK_BKP | RST_CLK_PCLK_ADCUI |     \
                           RST_CLK_PCLK_TIMER1 | RST_CLK_PCLK_TIMER2 | \
                           RST_CLK_PCLK_PORTA | RST_CLK_PCLK_PORTB |   \
                           RST_CLK_PCLK_PORTC | RST_CLK_PCLK_CRC |     \
                           RST_CLK_PCLK_RANDOM | RST_CLK_PCLK_USART |  \
                           RST_CLK_PCLK_SSP2 | RST_CLK_PCLK_SSP3 |     \
                           RST_CLK_PCLK_TIMER3 | RST_CLK_PCLK_TIMER4 | \
                           RST_CLK_PCLK_UART4 | RST_CLK_PCLK_PORTD)



#define IS_RST_CLK_PCLKS(PCLK) (((PCLK) & RST_CLK_PCLK_MASK) == (PCLK))

/**
 * @brief RST_CLK PER1_C1 clock source selection constants.
 */
typedef enum {
    RST_CLK_PER1_C1_CLK_SRC_LSI       = ((uint32_t)RST_CLK_PER1_CLOCK_PER1_C1_SEL_LSI),
    RST_CLK_PER1_C1_CLK_SRC_LSI_DIV_2 = ((uint32_t)RST_CLK_PER1_CLOCK_PER1_C1_SEL_LSI_DIV_2),
    RST_CLK_PER1_C1_CLK_SRC_LSE       = ((uint32_t)RST_CLK_PER1_CLOCK_PER1_C1_SEL_LSE),
    RST_CLK_PER1_C1_CLK_SRC_LSE_DIV_2 = ((uint32_t)RST_CLK_PER1_CLOCK_PER1_C1_SEL_LSE_DIV_2)
} RST_CLK_PER1_C1_ClockSource_TypeDef;

#define IS_RST_CLK_PER1_C1_CLOCK_SOURCE(CLK) (((CLK) == RST_CLK_PER1_C1_CLK_SRC_LSI) ||       \
                                              ((CLK) == RST_CLK_PER1_C1_CLK_SRC_LSI_DIV_2) || \
                                              ((CLK) == RST_CLK_PER1_C1_CLK_SRC_LSE) ||       \
                                              ((CLK) == RST_CLK_PER1_C1_CLK_SRC_LSE_DIV_2))

/**
 * @brief RST_CLK PER1_C2 clock source selection constants.
 */
typedef enum {
    RST_CLK_PER1_C2_CLK_SRC_CPU_C1     = ((uint32_t)RST_CLK_PER1_CLOCK_PER1_C2_SEL_CPU_C1),
    RST_CLK_PER1_C2_CLK_SRC_PER1_C1    = ((uint32_t)RST_CLK_PER1_CLOCK_PER1_C2_SEL_PER1_C1),
    RST_CLK_PER1_C2_CLK_SRC_PLLCPU     = ((uint32_t)RST_CLK_PER1_CLOCK_PER1_C2_SEL_PLLCPU),
    RST_CLK_PER1_C2_CLK_SRC_HSI_C1_CLK = ((uint32_t)RST_CLK_PER1_CLOCK_PER1_C2_SEL_HSI_C1_CLK)
} RST_CLK_PER1_C2_ClockSource_TypeDef;

#define IS_RST_CLK_PER1_C2_CLOCK_SOURCE(CLK) (((CLK) == RST_CLK_PER1_C2_CLK_SRC_CPU_C1) ||  \
                                              ((CLK) == RST_CLK_PER1_C2_CLK_SRC_PER1_C1) || \
                                              ((CLK) == RST_CLK_PER1_C2_CLK_SRC_PLLCPU) ||  \
                                              ((CLK) == RST_CLK_PER1_C2_CLK_SRC_HSI_C1_CLK))

/**
 * @brief RST_CLK PER1 peripheral selection constants.
 */
typedef enum {
    RST_CLK_PER1_C2_UART1 = ((uint32_t)0x1 << 0),
    RST_CLK_PER1_C2_UART2 = ((uint32_t)0x1 << 1),
    RST_CLK_PER1_C2_UART3 = ((uint32_t)0x1 << 2),
    RST_CLK_PER1_C2_UART4 = ((uint32_t)0x1 << 6),
    RST_CLK_PER1_C2_TIM1  = ((uint32_t)0x1 << 7),
    RST_CLK_PER1_C2_TIM2  = ((uint32_t)0x1 << 8),
    RST_CLK_PER1_C2_TIM3  = ((uint32_t)0x1 << 9),
    RST_CLK_PER1_C2_TIM4  = ((uint32_t)0x1 << 10),
    RST_CLK_PER1_C2_SSP1  = ((uint32_t)0x1 << 11),
    RST_CLK_PER1_C2_SSP2  = ((uint32_t)0x1 << 12),
    RST_CLK_PER1_C2_SSP3  = ((uint32_t)0x1 << 13)
} RST_CLK_PER1_C2_X_TypeDef;

#define IS_RST_CLK_PER1_CLK(PER1_CLK) (((PER1_CLK) == RST_CLK_PER1_C2_UART1) || \
                                       ((PER1_CLK) == RST_CLK_PER1_C2_UART2) || \
                                       ((PER1_CLK) == RST_CLK_PER1_C2_UART3) || \
                                       ((PER1_CLK) == RST_CLK_PER1_C2_UART4) || \
                                       ((PER1_CLK) == RST_CLK_PER1_C2_TIM1) ||  \
                                       ((PER1_CLK) == RST_CLK_PER1_C2_TIM2) ||  \
                                       ((PER1_CLK) == RST_CLK_PER1_C2_TIM3) ||  \
                                       ((PER1_CLK) == RST_CLK_PER1_C2_TIM4) ||  \
                                       ((PER1_CLK) == RST_CLK_PER1_C2_SSP1) ||  \
                                       ((PER1_CLK) == RST_CLK_PER1_C2_SSP2) ||  \
                                       ((PER1_CLK) == RST_CLK_PER1_C2_SSP3))

#define RST_CLK_PER1_CLK_UART_MASK (uint32_t)(RST_CLK_PER1_C2_UART1 | \
                                              RST_CLK_PER1_C2_UART2 | \
                                              RST_CLK_PER1_C2_UART3 | \
                                              RST_CLK_PER1_C2_UART4)

#define RST_CLK_PER1_CLK_TIM_MASK (uint32_t)(RST_CLK_PER1_C2_TIM1 | \
                                             RST_CLK_PER1_C2_TIM2 | \
                                             RST_CLK_PER1_C2_TIM3 | \
                                             RST_CLK_PER1_C2_TIM4)

#define RST_CLK_PER1_CLK_SSP_MASK (uint32_t)(RST_CLK_PER1_C2_SSP1 | \
                                             RST_CLK_PER1_C2_SSP2 | \
                                             RST_CLK_PER1_C2_SSP3)

#define IS_RST_CLK_PER1_CLKS(PER1_CLKS) (((PER1_CLKS) & ~(RST_CLK_PER1_CLK_UART_MASK | \
                                                          RST_CLK_PER1_CLK_TIM_MASK |  \
                                                          RST_CLK_PER1_CLK_SSP_MASK)) == 0x0)

/**
 * @brief RST_CLK PER1_C2 divider constants.
 */
typedef enum {
    RST_CLK_PER1_PRESCALER_DIV_1   = ((uint32_t)0x0),
    RST_CLK_PER1_PRESCALER_DIV_2   = ((uint32_t)0x1),
    RST_CLK_PER1_PRESCALER_DIV_4   = ((uint32_t)0x2),
    RST_CLK_PER1_PRESCALER_DIV_8   = ((uint32_t)0x3),
    RST_CLK_PER1_PRESCALER_DIV_16  = ((uint32_t)0x4),
    RST_CLK_PER1_PRESCALER_DIV_32  = ((uint32_t)0x5),
    RST_CLK_PER1_PRESCALER_DIV_64  = ((uint32_t)0x6),
    RST_CLK_PER1_PRESCALER_DIV_128 = ((uint32_t)0x7)
} RST_CLK_PER1_C2_Prescaler_TypeDef;

#define IS_RST_CLK_PER1_C2_DIV(DIV) (((DIV) == RST_CLK_PER1_PRESCALER_DIV_1) ||  \
                                     ((DIV) == RST_CLK_PER1_PRESCALER_DIV_2) ||  \
                                     ((DIV) == RST_CLK_PER1_PRESCALER_DIV_4) ||  \
                                     ((DIV) == RST_CLK_PER1_PRESCALER_DIV_8) ||  \
                                     ((DIV) == RST_CLK_PER1_PRESCALER_DIV_16) || \
                                     ((DIV) == RST_CLK_PER1_PRESCALER_DIV_32) || \
                                     ((DIV) == RST_CLK_PER1_PRESCALER_DIV_64) || \
                                     ((DIV) == RST_CLK_PER1_PRESCALER_DIV_128))

/**
 * @brief RST_CLK WWDG_C1 clock source selection constants.
 */
typedef enum {
    RST_CLK_WWDG_C1_CLK_SRC_LSI       = ((uint32_t)RST_CLK_PER1_CLOCK_WDG_C1_SEL_LSI),
    RST_CLK_WWDG_C1_CLK_SRC_LSI_DIV_2 = ((uint32_t)RST_CLK_PER1_CLOCK_WDG_C1_SEL_LSI_DIV_2),
    RST_CLK_WWDG_C1_CLK_SRC_LSE       = ((uint32_t)RST_CLK_PER1_CLOCK_WDG_C1_SEL_LSE),
    RST_CLK_WWDG_C1_CLK_SRC_LSE_DIV_2 = ((uint32_t)RST_CLK_PER1_CLOCK_WDG_C1_SEL_LSE_DIV_2)
} RST_CLK_WWDG_C1_ClockSource_TypeDef;

#define IS_RST_CLK_WWDG_C1_CLOCK_SOURCE(CLK) (((CLK) == RST_CLK_WWDG_C1_CLK_SRC_LSI) ||       \
                                              ((CLK) == RST_CLK_WWDG_C1_CLK_SRC_LSI_DIV_2) || \
                                              ((CLK) == RST_CLK_WWDG_C1_CLK_SRC_LSE) ||       \
                                              ((CLK) == RST_CLK_WWDG_C1_CLK_SRC_LSE_DIV_2))

/**
 * @brief RST_CLK WWDG_C2 clock source selection constants.
 */
typedef enum {
    RST_CLK_WWDG_C2_CLK_SRC_CPU_C1     = ((uint32_t)RST_CLK_PER1_CLOCK_WDG_C2_SEL_CPU_C1),
    RST_CLK_WWDG_C2_CLK_SRC_WWDG_C1    = ((uint32_t)RST_CLK_PER1_CLOCK_WDG_C2_SEL_PER1_C1),
    RST_CLK_WWDG_C2_CLK_SRC_PLLCPU     = ((uint32_t)RST_CLK_PER1_CLOCK_WDG_C2_SEL_PLLCPU),
    RST_CLK_WWDG_C2_CLK_SRC_HSI_C1_CLK = ((uint32_t)RST_CLK_PER1_CLOCK_WDG_C2_SEL_HSI_C1_CLK)
} RST_CLK_WWDG_C2_ClockSource_TypeDef;

#define IS_RST_CLK_WWDG_C2_CLOCK_SOURCE(CLK) (((CLK) == RST_CLK_WWDG_C2_CLK_SRC_CPU_C1) ||  \
                                              ((CLK) == RST_CLK_WWDG_C2_CLK_SRC_WWDG_C1) || \
                                              ((CLK) == RST_CLK_WWDG_C2_CLK_SRC_PLLCPU) ||  \
                                              ((CLK) == RST_CLK_WWDG_C2_CLK_SRC_HSI_C1_CLK))

/**
 * @brief RST_CLK WWDG_BRG divider constants.
 */
typedef enum {
    RST_CLK_WWDG_PRESCALER_DIV_1   = (((uint32_t)0x0) << RST_CLK_PER1_CLOCK_WDG_BRG_Pos),
    RST_CLK_WWDG_PRESCALER_DIV_2   = (((uint32_t)0x1) << RST_CLK_PER1_CLOCK_WDG_BRG_Pos),
    RST_CLK_WWDG_PRESCALER_DIV_4   = (((uint32_t)0x2) << RST_CLK_PER1_CLOCK_WDG_BRG_Pos),
    RST_CLK_WWDG_PRESCALER_DIV_8   = (((uint32_t)0x3) << RST_CLK_PER1_CLOCK_WDG_BRG_Pos),
    RST_CLK_WWDG_PRESCALER_DIV_16  = (((uint32_t)0x4) << RST_CLK_PER1_CLOCK_WDG_BRG_Pos),
    RST_CLK_WWDG_PRESCALER_DIV_32  = (((uint32_t)0x5) << RST_CLK_PER1_CLOCK_WDG_BRG_Pos),
    RST_CLK_WWDG_PRESCALER_DIV_64  = (((uint32_t)0x6) << RST_CLK_PER1_CLOCK_WDG_BRG_Pos),
    RST_CLK_WWDG_PRESCALER_DIV_128 = (((uint32_t)0x7) << RST_CLK_PER1_CLOCK_WDG_BRG_Pos)
} RST_CLK_WWDG_C2_Prescaler_TypeDef;

#define IS_RST_CLK_WWDG_C2_DIV(DIV) (((DIV) == RST_CLK_WWDG_PRESCALER_DIV_1) ||  \
                                     ((DIV) == RST_CLK_WWDG_PRESCALER_DIV_2) ||  \
                                     ((DIV) == RST_CLK_WWDG_PRESCALER_DIV_4) ||  \
                                     ((DIV) == RST_CLK_WWDG_PRESCALER_DIV_8) ||  \
                                     ((DIV) == RST_CLK_WWDG_PRESCALER_DIV_16) || \
                                     ((DIV) == RST_CLK_WWDG_PRESCALER_DIV_32) || \
                                     ((DIV) == RST_CLK_WWDG_PRESCALER_DIV_64) || \
                                     ((DIV) == RST_CLK_WWDG_PRESCALER_DIV_128))

/**
 * @brief RST_CLK flag identifiers.
 */
typedef enum {
    RST_CLK_FLAG_HSERDY    = RST_CLK_CLOCK_STATUS_HSE_RDY,
    RST_CLK_FLAG_PLLCPURDY = RST_CLK_CLOCK_STATUS_PLL_CPU_RDY
} RST_CLK_Flags_TypeDef;

#define IS_RST_CLK_FLAG(FLAG) (((FLAG) == RST_CLK_FLAG_HSERDY) || \
                               ((FLAG) == RST_CLK_FLAG_PLLCPURDY))

/**
 * @brief RST_CLK HSI_C1_SEL divider constants for RTCHSI.
 */
typedef enum {
    RST_CLK_HSI_PRESCALER_DIV_1   = (((uint32_t)0x0) << RST_CLK_RTC_CLOCK_HSI_SEL_Pos),
    RST_CLK_HSI_PRESCALER_DIV_2   = (((uint32_t)0x8) << RST_CLK_RTC_CLOCK_HSI_SEL_Pos),
    RST_CLK_HSI_PRESCALER_DIV_4   = (((uint32_t)0x9) << RST_CLK_RTC_CLOCK_HSI_SEL_Pos),
    RST_CLK_HSI_PRESCALER_DIV_8   = (((uint32_t)0xA) << RST_CLK_RTC_CLOCK_HSI_SEL_Pos),
    RST_CLK_HSI_PRESCALER_DIV_16  = (((uint32_t)0xB) << RST_CLK_RTC_CLOCK_HSI_SEL_Pos),
    RST_CLK_HSI_PRESCALER_DIV_32  = (((uint32_t)0xC) << RST_CLK_RTC_CLOCK_HSI_SEL_Pos),
    RST_CLK_HSI_PRESCALER_DIV_64  = (((uint32_t)0xD) << RST_CLK_RTC_CLOCK_HSI_SEL_Pos),
    RST_CLK_HSI_PRESCALER_DIV_128 = (((uint32_t)0xE) << RST_CLK_RTC_CLOCK_HSI_SEL_Pos),
    RST_CLK_HSI_PRESCALER_DIV_256 = (((uint32_t)0xF) << RST_CLK_RTC_CLOCK_HSI_SEL_Pos)
} RST_CLK_HSI_C1_Prescaler_TypeDef;

#define IS_RST_CLK_HSI_CLK_DIV(DIV) (((DIV) == RST_CLK_HSI_PRESCALER_DIV_1) ||   \
                                     ((DIV) == RST_CLK_HSI_PRESCALER_DIV_2) ||   \
                                     ((DIV) == RST_CLK_HSI_PRESCALER_DIV_4) ||   \
                                     ((DIV) == RST_CLK_HSI_PRESCALER_DIV_8) ||   \
                                     ((DIV) == RST_CLK_HSI_PRESCALER_DIV_16) ||  \
                                     ((DIV) == RST_CLK_HSI_PRESCALER_DIV_32) ||  \
                                     ((DIV) == RST_CLK_HSI_PRESCALER_DIV_64) ||  \
                                     ((DIV) == RST_CLK_HSI_PRESCALER_DIV_128) || \
                                     ((DIV) == RST_CLK_HSI_PRESCALER_DIV_256))

/**
 * @brief RST_CLK HSE_C1_SEL divider constants for RTCHSE.
 */
typedef enum {
    RST_CLK_HSE_PRESCALER_DIV_1   = (((uint32_t)0x0) << RST_CLK_RTC_CLOCK_HSE_SEL_Pos),
    RST_CLK_HSE_PRESCALER_DIV_2   = (((uint32_t)0x8) << RST_CLK_RTC_CLOCK_HSE_SEL_Pos),
    RST_CLK_HSE_PRESCALER_DIV_4   = (((uint32_t)0x9) << RST_CLK_RTC_CLOCK_HSE_SEL_Pos),
    RST_CLK_HSE_PRESCALER_DIV_8   = (((uint32_t)0xA) << RST_CLK_RTC_CLOCK_HSE_SEL_Pos),
    RST_CLK_HSE_PRESCALER_DIV_16  = (((uint32_t)0xB) << RST_CLK_RTC_CLOCK_HSE_SEL_Pos),
    RST_CLK_HSE_PRESCALER_DIV_32  = (((uint32_t)0xC) << RST_CLK_RTC_CLOCK_HSE_SEL_Pos),
    RST_CLK_HSE_PRESCALER_DIV_64  = (((uint32_t)0xD) << RST_CLK_RTC_CLOCK_HSE_SEL_Pos),
    RST_CLK_HSE_PRESCALER_DIV_128 = (((uint32_t)0xE) << RST_CLK_RTC_CLOCK_HSE_SEL_Pos),
    RST_CLK_HSE_PRESCALER_DIV_256 = (((uint32_t)0xF) << RST_CLK_RTC_CLOCK_HSE_SEL_Pos)
} RST_CLK_HSE_C1_Prescaler_TypeDef;

#define IS_RST_CLK_HSE_CLK_DIV(DIV) (((DIV) == RST_CLK_HSE_PRESCALER_DIV_1) ||   \
                                     ((DIV) == RST_CLK_HSE_PRESCALER_DIV_2) ||   \
                                     ((DIV) == RST_CLK_HSE_PRESCALER_DIV_4) ||   \
                                     ((DIV) == RST_CLK_HSE_PRESCALER_DIV_8) ||   \
                                     ((DIV) == RST_CLK_HSE_PRESCALER_DIV_16) ||  \
                                     ((DIV) == RST_CLK_HSE_PRESCALER_DIV_32) ||  \
                                     ((DIV) == RST_CLK_HSE_PRESCALER_DIV_64) ||  \
                                     ((DIV) == RST_CLK_HSE_PRESCALER_DIV_128) || \
                                     ((DIV) == RST_CLK_HSE_PRESCALER_DIV_256))

#if defined(USE_MDR32F02_REV_2) || defined(USE_MDR1206FI) || defined(USE_MDR1206AFI) || defined(USE_MDR1206)
/**
 * @brief RST_CLK DMA_DONE_STICK channels.
 */
typedef enum {
    RST_CLK_DMA_DONE_STICK_CHANNEL_UART1_TX = RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK0,      /*!< UART1 TX channel. */
    RST_CLK_DMA_DONE_STICK_CHANNEL_UART1_RX = RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK1,      /*!< UART1 RX channel. */
    RST_CLK_DMA_DONE_STICK_CHANNEL_UART2_TX = RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK2,      /*!< UART2 TX channel. */
    RST_CLK_DMA_DONE_STICK_CHANNEL_UART2_RX = RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK3,      /*!< UART2 RX channel. */
    RST_CLK_DMA_DONE_STICK_CHANNEL_SSP1_TX  = RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK4,      /*!< SSP1 TX channel. */
    RST_CLK_DMA_DONE_STICK_CHANNEL_SSP1_RX  = RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK5,      /*!< SSP1 RX channel. */
    RST_CLK_DMA_DONE_STICK_CHANNEL_CRC      = RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK6,      /*!< CRC channel. */
    RST_CLK_DMA_DONE_STICK_CHANNEL_UART3_TX = RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK7,      /*!< UART3 TX channel. */
    RST_CLK_DMA_DONE_STICK_CHANNEL_UART3_RX = RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK8,      /*!< UART3 RX channel. */
    RST_CLK_DMA_DONE_STICK_CHANNEL_TIM3     = RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK9,      /*!< TIMER3 channel. */
    RST_CLK_DMA_DONE_STICK_CHANNEL_TIM1     = RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK10,     /*!< TIMER1 channel. */
    RST_CLK_DMA_DONE_STICK_CHANNEL_TIM2     = RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK11,     /*!< TIMER2 channel. */
    RST_CLK_DMA_DONE_STICK_CHANNEL_ADCUI_I0 = RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK12,     /*!< ADCUI I0 channel. */
    RST_CLK_DMA_DONE_STICK_CHANNEL_ADCUI_V0 = RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK13,     /*!< ADCUI V0 channel. */
    RST_CLK_DMA_DONE_STICK_CHANNEL_ADCUI_I1 = RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK14,     /*!< ADCUI I1 channel. */
    RST_CLK_DMA_DONE_STICK_CHANNEL_ADCUI_V1 = RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK15,     /*!< ADCUI V1 channel. */
    RST_CLK_DMA_DONE_STICK_CHANNEL_ADCUI_I2 = RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK16,     /*!< ADCUI I2 channel. */
    RST_CLK_DMA_DONE_STICK_CHANNEL_ADCUI_V2 = RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK17,     /*!< ADCUI V2 channel. */
    RST_CLK_DMA_DONE_STICK_CHANNEL_ADCUI_I3 = RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK18,     /*!< ADCUI I3 channel. */
    RST_CLK_DMA_DONE_STICK_CHANNEL_SSP2_TX  = RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK19,     /*!< SSP2 TX channel. */
    RST_CLK_DMA_DONE_STICK_CHANNEL_SSP2_RX  = RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK20,     /*!< SSP2 RX channel. */
    RST_CLK_DMA_DONE_STICK_CHANNEL_SSP3_TX  = RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK21,     /*!< SSP3 TX channel. */
    RST_CLK_DMA_DONE_STICK_CHANNEL_SSP3_RX  = RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK22,     /*!< SSP3 RX channel. */
    RST_CLK_DMA_DONE_STICK_CHANNEL_TIM4     = RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK23,     /*!< TIMER4 channel. */
    RST_CLK_DMA_DONE_STICK_CHANNEL_UART4_TX = RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK24,     /*!< UART4 TX channel. */
    RST_CLK_DMA_DONE_STICK_CHANNEL_UART4_RX = RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK25,     /*!< UART4 RX channel. */
    RST_CLK_DMA_DONE_STICK_CHANNEL_SW1      = RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK26,     /*!< Software channel 1. */
    RST_CLK_DMA_DONE_STICK_CHANNEL_SW2      = RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK27,     /*!< Software channel 2. */
    RST_CLK_DMA_DONE_STICK_CHANNEL_SW3      = RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK28,     /*!< Software channel 3. */
    RST_CLK_DMA_DONE_STICK_CHANNEL_SW4      = RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK29,     /*!< Software channel 4. */
    RST_CLK_DMA_DONE_STICK_CHANNEL_ADC      = RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK30,     /*!< ADC channel. */
    RST_CLK_DMA_DONE_STICK_CHANNEL_SW5      = (int)RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK31 /*!< Software channel 5. */
} RST_CLK_DMADoneStickChannels_TypeDef;

#define IS_RST_CLK_DMA_DONE_STICK_CHANNEL(CH) (((CH) == RST_CLK_DMA_DONE_STICK_CHANNEL_UART1_TX) || \
                                               ((CH) == RST_CLK_DMA_DONE_STICK_CHANNEL_UART1_RX) || \
                                               ((CH) == RST_CLK_DMA_DONE_STICK_CHANNEL_UART2_TX) || \
                                               ((CH) == RST_CLK_DMA_DONE_STICK_CHANNEL_UART2_RX) || \
                                               ((CH) == RST_CLK_DMA_DONE_STICK_CHANNEL_SSP1_TX) ||  \
                                               ((CH) == RST_CLK_DMA_DONE_STICK_CHANNEL_SSP1_RX) ||  \
                                               ((CH) == RST_CLK_DMA_DONE_STICK_CHANNEL_CRC) ||      \
                                               ((CH) == RST_CLK_DMA_DONE_STICK_CHANNEL_UART3_TX) || \
                                               ((CH) == RST_CLK_DMA_DONE_STICK_CHANNEL_UART3_RX) || \
                                               ((CH) == RST_CLK_DMA_DONE_STICK_CHANNEL_TIM3) ||     \
                                               ((CH) == RST_CLK_DMA_DONE_STICK_CHANNEL_TIM1) ||     \
                                               ((CH) == RST_CLK_DMA_DONE_STICK_CHANNEL_TIM2) ||     \
                                               ((CH) == RST_CLK_DMA_DONE_STICK_CHANNEL_ADCUI_I0) || \
                                               ((CH) == RST_CLK_DMA_DONE_STICK_CHANNEL_ADCUI_V0) || \
                                               ((CH) == RST_CLK_DMA_DONE_STICK_CHANNEL_ADCUI_I1) || \
                                               ((CH) == RST_CLK_DMA_DONE_STICK_CHANNEL_ADCUI_V1) || \
                                               ((CH) == RST_CLK_DMA_DONE_STICK_CHANNEL_ADCUI_I2) || \
                                               ((CH) == RST_CLK_DMA_DONE_STICK_CHANNEL_ADCUI_V2) || \
                                               ((CH) == RST_CLK_DMA_DONE_STICK_CHANNEL_ADCUI_I3) || \
                                               ((CH) == RST_CLK_DMA_DONE_STICK_CHANNEL_SSP2_TX) ||  \
                                               ((CH) == RST_CLK_DMA_DONE_STICK_CHANNEL_SSP2_RX) ||  \
                                               ((CH) == RST_CLK_DMA_DONE_STICK_CHANNEL_SSP3_TX) ||  \
                                               ((CH) == RST_CLK_DMA_DONE_STICK_CHANNEL_SSP3_RX) ||  \
                                               ((CH) == RST_CLK_DMA_DONE_STICK_CHANNEL_TIM4) ||     \
                                               ((CH) == RST_CLK_DMA_DONE_STICK_CHANNEL_UART4_TX) || \
                                               ((CH) == RST_CLK_DMA_DONE_STICK_CHANNEL_UART4_RX) || \
                                               ((CH) == RST_CLK_DMA_DONE_STICK_CHANNEL_SW1) ||      \
                                               ((CH) == RST_CLK_DMA_DONE_STICK_CHANNEL_SW2) ||      \
                                               ((CH) == RST_CLK_DMA_DONE_STICK_CHANNEL_SW3) ||      \
                                               ((CH) == RST_CLK_DMA_DONE_STICK_CHANNEL_SW4) ||      \
                                               ((CH) == RST_CLK_DMA_DONE_STICK_CHANNEL_ADC) ||      \
                                               ((CH) == RST_CLK_DMA_DONE_STICK_CHANNEL_SW5))
#endif

/**
 * @brief RST_CLK on-chip peripheral clock frequencies.
 */
typedef enum {
    RST_CLK_CLOCK_FREQ_CPU_CLK       = ((uint32_t)0x1 << 0),  /*!< CPU clock frequency. */
    RST_CLK_CLOCK_FREQ_UART1         = ((uint32_t)0x1 << 1),  /*!< UART1 clock frequency. */
    RST_CLK_CLOCK_FREQ_UART2         = ((uint32_t)0x1 << 2),  /*!< UART2 clock frequency. */
    RST_CLK_CLOCK_FREQ_UART3         = ((uint32_t)0x1 << 3),  /*!< UART3 clock frequency. */
    RST_CLK_CLOCK_FREQ_UART4         = ((uint32_t)0x1 << 4),  /*!< UART4 clock frequency. */
    RST_CLK_CLOCK_FREQ_TIMER1        = ((uint32_t)0x1 << 5),  /*!< TIMER1 clock frequency. */
    RST_CLK_CLOCK_FREQ_TIMER2        = ((uint32_t)0x1 << 6),  /*!< TIMER2 clock frequency. */
    RST_CLK_CLOCK_FREQ_TIMER3        = ((uint32_t)0x1 << 7),  /*!< TIMER3 clock frequency. */
    RST_CLK_CLOCK_FREQ_TIMER4        = ((uint32_t)0x1 << 8),  /*!< TIMER4 clock frequency. */
    RST_CLK_CLOCK_FREQ_SSP1          = ((uint32_t)0x1 << 9),  /*!< SSP1 clock frequency. */
    RST_CLK_CLOCK_FREQ_SSP2          = ((uint32_t)0x1 << 10), /*!< SSP2 clock frequency. */
    RST_CLK_CLOCK_FREQ_SSP3          = ((uint32_t)0x1 << 11), /*!< SSP3 clock frequency. */
    RST_CLK_CLOCK_FREQ_WWDG          = ((uint32_t)0x1 << 12), /*!< WWDG clock frequency. */
    RST_CLK_CLOCK_FREQ_ADC           = ((uint32_t)0x1 << 13), /*!< ADC clock frequency. */
    RST_CLK_CLOCK_FREQ_ADCUI         = ((uint32_t)0x1 << 14), /*!< ADCUI clock frequency. */
    RST_CLK_CLOCK_FREQ_RTCHSI        = ((uint32_t)0x1 << 15), /*!< RTCHSI clock frequency. */
    RST_CLK_CLOCK_FREQ_RTCHSE        = ((uint32_t)0x1 << 16), /*!< RTCHSE clock frequency. */
    RST_CLK_CLOCK_FREQ_MACHINE_TIMER = ((uint32_t)0x1 << 17), /*!< Machine timer clock frequency. */
    RST_CLK_CLOCK_FREQ_ALL           = ((uint32_t)0x3FFFF)    /*!< Select all clock frequencies. */
} RST_CLK_Frequencies_TypeDef;

#define IS_RST_CLK_CLOCK_FREQUENCY(CLK_FREQ) (((CLK_FREQ) & ~(uint32_t)(RST_CLK_CLOCK_FREQ_ALL)) == 0x0)

/**
 * @brief Structure type for peripheral clock frequencies in Hz.
 */
typedef struct {
    uint32_t CPU_CLK_Frequency;      /*!< CPU_CLK frequency. */
    uint32_t UART_Frequency[4];      /*!< UART frequencies. */
    uint32_t TIMER_Frequency[4];     /*!< TIMER frequencies . */
    uint32_t SSP_Frequency[3];       /*!< SSP frequencies . */
    uint32_t WWDG_Frequency;         /*!< WWDG frequency. */
    uint32_t ADC_Frequency;          /*!< ADC frequency. */
    uint32_t ADCUI_Frequency;        /*!< ADCUI frequency. */
    uint32_t RTCHSI_Frequency;       /*!< HSI_RTC frequency. */
    uint32_t RTCHSE_Frequency;       /*!< HSE_RTC frequency. */
    uint32_t MachineTimer_Frequency; /*!< Core machine timer frequency. */
} RST_CLK_Freq_TypeDef;

/**
 * @brief RST_CLK HCLK clock init struct definition.
 */
typedef struct {
    RST_CLK_CPU_C1_ClockSource_TypeDef RST_CLK_CPU_C1_Source;      /*!< CPU_C1 clock source selection.
                                                                        Not used if (RSC_CLK_PLLCPU_ClockSource == RST_CLK_PLLCPU_CLK_SRC_HSE)).
                                                                        This parameter can be a value of the @ref RST_CLK_CPU_C1_ClockSource_TypeDef. */
    RST_CLK_PLLCPU_ClockSource_TypeDef RST_CLK_PLLCPU_ClockSource; /*!< CPU_PLL clock source selection.
                                                                        Not used if (RST_CLK_CPU_C2_ClockSource == RST_CLK_CPU_C2_CLK_SRC_CPU_C1).
                                                                        This parameter can be a value of the @ref RST_CLK_PLLCPU_ClockSource_TypeDef. */
    RST_CLK_PLLCPU_Multiplier_TypeDef RST_CLK_PLLCPU_Multiplier;   /*!< CPU_PLL clock multiplier selection.
                                                                        Not used if (RST_CLK_CPU_C2_ClockSource == RST_CLK_CPU_C2_CLK_SRC_CPU_C1).
                                                                        This parameter can be a value of the @ref RST_CLK_PLLCPU_Multiplier_TypeDef. */
    RST_CLK_CPU_C2_ClockSource_TypeDef RST_CLK_CPU_C2_ClockSource; /*!< CPU_C2 clock source selection.
                                                                        This parameter can be a value of the @ref RST_CLK_CPU_C2_ClockSource_TypeDef. */
    RST_CLK_CPU_C3_Prescaler_TypeDef RST_CLK_CPU_C3_Prescaler;     /*!< CPU_C3 clock divider.
                                                                        This parameter can be a value of the @ref RST_CLK_CPU_C3_Prescaler_TypeDef. */
    RST_CLK_HCLK_ClockSource_TypeDef RST_CLK_HCLK_ClockSource;     /*!< HCLK clock source selection.
                                                                        All other fields are not used if (RST_CLK_HCLK_ClockSource != RST_CLK_CPU_HCLK_CLK_SRC_CPU_C3).
                                                                        This parameter can be a value of the @ref RST_CLK_HCLK_ClockSource_TypeDef. */
} RST_CLK_HCLK_InitTypeDef;

/** @} */ /* End of the group RST_CLK_Exported_Types */

/** @defgroup RST_CLK_Exported_Defines RST_CLK Exported Defines
 * @{
 */

#define IS_RST_CLK_DIV_SYS_TIM(DIV) (((DIV) >= 2) && ((DIV) <= 257))

/** @} */ /* End of the group RST_CLK_Exported_Defines */

/** @defgroup RST_CLK_Exported_Functions RST_CLK Exported Functions
 * @{
 */

void RST_CLK_DeInit(void);

ErrorStatus RST_CLK_HCLK_Init(const RST_CLK_HCLK_InitTypeDef* RST_CLK_HCLK_InitStruct);
void        RST_CLK_HCLK_StructInit(RST_CLK_HCLK_InitTypeDef* RST_CLK_HCLK_InitStruct);

void                     RST_CLK_HSE_Cmd(FunctionalState NewState);
void                     RST_CLK_HSE_Config(RST_CLK_HSE_Mode_TypeDef HSE_Mode);
RST_CLK_HSE_Mode_TypeDef RST_CLK_HSE_GetModeConfig(void);
ErrorStatus              RST_CLK_HSE_GetStatus(void);

void            RST_CLK_CPU_PLL_Config(RST_CLK_PLLCPU_ClockSource_TypeDef PLLCPU_Source, uint32_t PLLCPU_Multiplier);
void            RST_CLK_CPU_PLL_Cmd(FunctionalState NewState);
FunctionalState RST_CLK_CPU_PLL_GetCmdState(void);
ErrorStatus     RST_CLK_CPU_PLL_GetStatus(void);

void RST_CLK_CPU_C1_ClkSelection(RST_CLK_CPU_C1_ClockSource_TypeDef CPU_C1_Source);
void RST_CLK_CPU_C2_ClkSelection(RST_CLK_CPU_C2_ClockSource_TypeDef CPU_C2_Source);
void RST_CLK_CPU_C3_SetPrescaler(RST_CLK_CPU_C3_Prescaler_TypeDef CPU_CLK_DivValue);
void RST_CLK_HCLK_ClkSelection(RST_CLK_HCLK_ClockSource_TypeDef CPU_HCLK_Source);

void            RST_CLK_ADC_ClkSelection(RST_CLK_ADC_ClockSource_TypeDef ADC_CLK_Source);
void            RST_CLK_ADC_SetPrescaler(RST_CLK_ADC_C3_Prescaler_TypeDef ADC_CLK_DivValue);
void            RST_CLK_ADC_ClkCmd(FunctionalState NewState);
FunctionalState RST_CLK_ADC_GetClkCmdState(void);

void            RST_CLK_ADCUI_ClkDeInit(void);
void            RST_CLK_ADCUI_ClkSelection(RST_CLK_ADCUI_ClockSource_TypeDef ADCUI_CLK_C1_Source);
void            RST_CLK_ADCUI_SetPrescaler(RST_CLK_ADCUI_C3_Prescaler_TypeDef ADCUI_CLK_DivValue);
void            RST_CLK_ADCUI_ClkCmd(FunctionalState NewState);
FunctionalState RST_CLK_ADCUI_GetClkCmdState(void);

void            RST_CLK_HSI_C1_SetPrescaler(RST_CLK_HSI_C1_Prescaler_TypeDef HSI_CLK_DivValue);
void            RST_CLK_RTC_HSI_ClkCmd(FunctionalState NewState);
FunctionalState RST_CLK_RTC_HSI_GetClkCmdState(void);

void            RST_CLK_HSE_C1_SetPrescaler(RST_CLK_HSE_C1_Prescaler_TypeDef HSE_CLK_DivValue);
void            RST_CLK_RTC_HSE_ClkCmd(FunctionalState NewState);
FunctionalState RST_CLK_RTC_HSE_GetClkCmdState(void);

void RST_CLK_PER1_C1_ClkSelection(RST_CLK_PER1_C1_ClockSource_TypeDef PER1_C1_Source);
void RST_CLK_PER1_C2_ClkSelection(RST_CLK_PER1_C2_ClockSource_TypeDef PER1_C2_Source);

void            RST_CLK_PER1_C2_SetPrescaler(RST_CLK_PER1_C2_X_TypeDef PER1_CLK, RST_CLK_PER1_C2_Prescaler_TypeDef PER1_C2_DivValue);
void            RST_CLK_PER1_C2_Cmd(uint32_t PER1_CLK, FunctionalState NewState);
FunctionalState RST_CLK_PER1_C2_GetCmdState(RST_CLK_PER1_C2_X_TypeDef PER1_CLK);

void RST_CLK_WWDG_C1_ClkSelection(RST_CLK_WWDG_C1_ClockSource_TypeDef WWDG_C1_Source);
void RST_CLK_WWDG_C2_ClkSelection(RST_CLK_WWDG_C2_ClockSource_TypeDef WWDG_C2_Source);

void            RST_CLK_WWDG_C2_SetPrescaler(RST_CLK_WWDG_C2_Prescaler_TypeDef WWDG_C2_DivValue);
void            RST_CLK_WWDG_C2_Cmd(FunctionalState NewState);
FunctionalState RST_CLK_WWDG_C2_GetCmdState(void);

void            RST_CLK_DMACmd(FunctionalState NewState);
FunctionalState RST_CLK_GetDMACmdState(void);

void            RST_CLK_ResetKeyRAM_Cmd(FunctionalState NewState);
FunctionalState RST_CLK_GetResetKeyRAMCmdState(void);


FlagStatus RST_CLK_DMADone_GetFlagStatus(RST_CLK_DMADoneStickChannels_TypeDef Channel);
uint32_t   RST_CLK_DMADone_GetStatus(void);
void       RST_CLK_DMADone_ClearFlags(uint32_t Channels);

void     RST_CLK_SystemTimer_SetPrescaler(uint16_t SysTimDivValue);
uint16_t RST_CLK_SystemTimer_GetPrescaler(void);


void            RST_CLK_PCLKCmd(uint32_t RST_CLK_PCLK, FunctionalState NewState);
FunctionalState RST_CLK_GetPCLKCmdState(RST_CLK_PCLK_TypeDef RST_CLK_PCLK);

FlagStatus RST_CLK_GetFlagStatus(RST_CLK_Flags_TypeDef RST_CLK_Flag);

void RST_CLK_GetClocksFreq(RST_CLK_Freq_TypeDef* RST_CLK_Clocks, uint32_t Clocks);



#endif /* MDR32VF0xI_RST_CLK_H */

/*********************** (C) COPYRIGHT 2025 Milandr ****************************
 *
 * END OF FILE MDR32VF0xI_rst_clk.h */
