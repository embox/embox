/**
 *******************************************************************************
 * @file    MDR32VF0xI.h
 * @author  Milandr Application Team
 * @version V0.2.0
 * @date    23/05/2025
 * @brief   Device Peripheral Access Layer Header File for MDR32VF0xI.
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
#ifndef MDR32VF0xI_H
#define MDR32VF0xI_H

#include <stdint.h>

#undef USE_MDR32F02
#undef USE_MDR1206AFI

#define USE_MDR1206FI       1
#define USE_MDR1206         1

/* Clock generators frequencies if the SPL is not used. */
/** HSI clock value [Hz].
    Default: 8000000 (8MHz). */
#define HSI_FREQUENCY_Hz HSI_CLK_FREQUENCY_TYP_Hz
/** HSE clock value [Hz].
    Default: 8000000 (8MHz). */
#define HSE_FREQUENCY_Hz 8000000UL
/** LSI clock value [Hz].
    Default: 31250 (31.25kHz). */
#define LSI_FREQUENCY_Hz (HSI_FREQUENCY_Hz / 256)
/** LSE clock value [Hz].
    Default: 32768 (32.768kHz). */
#define LSE_FREQUENCY_Hz 32768UL

#define __O  volatile
#define __IO volatile
#define __I  volatile const

/**
 * @brief MDR1206 base addresses and sizes.
 */

#if defined(USE_MDR1206FI)
#define FLASH_BOOT_BASE (0x00020000UL)
#define FLASH_BOOT_SIZE (0x00004000UL)
#elif defined(USE_MDR1206AFI)
#define FLASH_BOOT_BASE (0x00020000UL)
#define FLASH_BOOT_SIZE (0x00003C00UL)
#elif defined(USE_MDR1206)
#define FLASH_BOOT_MDR1206FI_BASE  (0x00020000UL)
#define FLASH_BOOT_MDR1206FI_SIZE  (0x00004000UL)
#define FLASH_BOOT_MDR1206AFI_BASE (0x00020000UL)
#define FLASH_BOOT_MDR1206AFI_SIZE (0x00003C00UL)
#endif

#define FLASH_BASE    (0x10000000UL)
#define FLASH_SIZE    (0x00080000UL)

#define RAM_AHB_BASE  (0x20000000UL)
#define RAM_AHB_SIZE  (0x00004000UL)

#define RAM_TCMA_BASE (0x40000000UL)
#define RAM_TCMA_SIZE (0x00010000UL)

#define RAM_TCMB_BASE (0x40010000UL)
#define RAM_TCMB_SIZE (0x00008000UL)

#define PERIPH_BASE   (0x50000000UL)


/** @defgroup MDR32VF0xI_Peripheral_Memory_Map MDR32VF0xI Peripheral Memory Map
 * @{
 */

#define MDR_SSP1_BASE    (PERIPH_BASE + 0x00000000UL)
#define MDR_UART1_BASE   (PERIPH_BASE + 0x00008000UL)
#define MDR_UART2_BASE   (PERIPH_BASE + 0x00010000UL)
#define MDR_FLASH_BASE   (PERIPH_BASE + 0x00018000UL)
#define MDR_RST_CLK_BASE (PERIPH_BASE + 0x00020000UL)
#define MDR_DMA_BASE     (PERIPH_BASE + 0x00028000UL)
#define MDR_I2C_BASE     (PERIPH_BASE + 0x00030000UL)
#define MDR_UART3_BASE   (PERIPH_BASE + 0x00038000UL)
#define MDR_ADC_BASE     (PERIPH_BASE + 0x00040000UL)
#define MDR_WWDG_BASE    (PERIPH_BASE + 0x00048000UL)
#define MDR_IWDG_BASE    (PERIPH_BASE + 0x00050000UL)
#define MDR_POWER_BASE   (PERIPH_BASE + 0x00058000UL)
#define MDR_BKP_BASE     (PERIPH_BASE + 0x00060000UL)
#define MDR_ADCUI_BASE   (PERIPH_BASE + 0x00068000UL)
#define MDR_TIMER1_BASE  (PERIPH_BASE + 0x00070000UL)
#define MDR_TIMER2_BASE  (PERIPH_BASE + 0x00078000UL)
#define MDR_PORTA_BASE   (PERIPH_BASE + 0x00080000UL)
#define MDR_PORTB_BASE   (PERIPH_BASE + 0x00088000UL)
#define MDR_PORTC_BASE   (PERIPH_BASE + 0x00090000UL)
#define MDR_CRC_BASE     (PERIPH_BASE + 0x00098000UL)
#if defined(USE_MDR32F02)
#define MDR_SENSORS_BASE     (PERIPH_BASE + 0x000A8000UL)
#define MDR_CLK_MEASURE_BASE (PERIPH_BASE + 0x000B0000UL)
#endif
#define MDR_RANDOM_BASE (PERIPH_BASE + 0x000B8000UL)
#define MDR_USART_BASE  (PERIPH_BASE + 0x000C0000UL)
#define MDR_SSP2_BASE   (PERIPH_BASE + 0x000C8000UL)
#define MDR_SSP3_BASE   (PERIPH_BASE + 0x000D0000UL)
#define MDR_TIMER3_BASE (PERIPH_BASE + 0x000D8000UL)
#define MDR_TIMER4_BASE (PERIPH_BASE + 0x000E0000UL)
#define MDR_UART4_BASE  (PERIPH_BASE + 0x000E8000UL)
#define MDR_PORTD_BASE  (PERIPH_BASE + 0x000F0000UL)

#if defined(USE_MDR32F02)
#define MDR_L_BLOCK_BASE  (CRYPTO_REGION_BASE + 0x00004000UL)
#define MDR_S_BLOCK0_BASE (CRYPTO_REGION_BASE + 0x00008000UL)
#define MDR_S_BLOCK1_BASE (CRYPTO_REGION_BASE + 0x0000C000UL)
#define MDR_S_BLOCK2_BASE (CRYPTO_REGION_BASE + 0x00010000UL)
#define MDR_S_BLOCK3_BASE (CRYPTO_REGION_BASE + 0x00014000UL)
#define MDR_S_BLOCK4_BASE (CRYPTO_REGION_BASE + 0x00018000UL)
#define MDR_S_BLOCK5_BASE (CRYPTO_REGION_BASE + 0x0001C000UL)
#define MDR_S_BLOCK6_BASE (CRYPTO_REGION_BASE + 0x00020000UL)
#define MDR_S_BLOCK7_BASE (CRYPTO_REGION_BASE + 0x00024000UL)
#define MDR_P_BYTE_BASE   (CRYPTO_REGION_BASE + 0x00028000UL)
#define MDR_P_BIT0_BASE   (CRYPTO_REGION_BASE + 0x0002C000UL)
#define MDR_P_BIT1_BASE   (CRYPTO_REGION_BASE + 0x00030000UL)
#define MDR_P_BIT2_BASE   (CRYPTO_REGION_BASE + 0x00034000UL)
#define MDR_P_BIT3_BASE   (CRYPTO_REGION_BASE + 0x00038000UL)
#define MDR_OTP_REG_BASE  (CRYPTO_REGION_BASE + 0x10010000UL)
#endif

#if defined(USE_MDR1206FI)
#define MDR_FLASH_SPECIAL_FIELDS_BASE (FLASH_BOOT_BASE + 0x00003FE0UL)
#elif defined(USE_MDR1206AFI)
#define MDR_FLASH_SPECIAL_FIELDS_BASE (FLASH_BOOT_BASE + 0x00003BE0UL)
#elif defined(USE_MDR1206)
#define MDR_FLASH_SPECIAL_FIELDS_MDR1206FI_BASE  (FLASH_BOOT_MDR1206FI_BASE + 0x00003FE0UL)
#define MDR_FLASH_SPECIAL_FIELDS_MDR1206AFI_BASE (FLASH_BOOT_MDR1206AFI_BASE + 0x00003BE0UL)
#endif

/** @} */ /* End of group MDR32VF0xI_Peripheral_Memory_Map */

/** @} */ /* End of group MDR32VF0xI_Memory_Map */

/** @defgroup MDR32VF0xI_Hardware_Constants MDR32VF0xI Hardware Constants
 * @{
 */


#define CHIP_ID_MDR1206FI  217UL
#define CHIP_ID_MDR1206AFI 215UL


#if defined(USE_MDR32F02)
#define CLK_FREQUENCY_MAX_Hz 60000000UL
#elif defined(USE_MDR1206FI)
#define CLK_FREQUENCY_MAX_Hz 48000000UL
#elif defined(USE_MDR1206AFI)
#define CLK_FREQUENCY_MAX_Hz 42000000UL
#elif defined(USE_MDR1206)
#define CLK_MDR1206FI_FREQUENCY_MAX_Hz  48000000UL
#define CLK_MDR1206AFI_FREQUENCY_MAX_Hz 42000000UL
#endif

#if defined(USE_MDR32F02)
#define HSI_CLK_FREQUENCY_MIN_Hz 7900000UL
#define HSI_CLK_FREQUENCY_TYP_Hz 8000000UL
#define HSI_CLK_FREQUENCY_MAX_Hz 8100000UL
#elif defined(USE_MDR1206FI) || defined(USE_MDR1206AFI) || defined(USE_MDR1206)
#define HSI_CLK_FREQUENCY_MIN_Hz 7500000UL
#define HSI_CLK_FREQUENCY_TYP_Hz 8000000UL
#define HSI_CLK_FREQUENCY_MAX_Hz 8500000UL
#endif
#define HSIRDY_TIMEOUT_HARDWARE_TICK 3UL

#define HSE_OSC_CLK_FREQUENCY_MIN_Hz 8000000UL
#define HSE_OSC_CLK_FREQUENCY_MAX_Hz 16000000UL
#if defined(USE_MDR32F02) || defined(USE_MDR1206FI) || defined(USE_MDR1206AFI)
#define HSE_BYP_CLK_FREQUENCY_MAX_Hz CLK_FREQUENCY_MAX_Hz
#elif defined(USE_MDR1206)
#define HSE_BYP_CLK_MDR1206FI_FREQUENCY_MAX_Hz  CLK_MDR1206FI_FREQUENCY_MAX_Hz
#define HSE_BYP_CLK_MDR1206AFI_FREQUENCY_MAX_Hz CLK_MDR1206AFI_FREQUENCY_MAX_Hz
#endif
#define HSERDY_TIMEOUT_TYP_MS           5UL

#define LSI_CLK_FREQUENCY_MIN_Hz        (HSI_CLK_FREQUENCY_MIN_Hz / 256)
#define LSI_CLK_FREQUENCY_TYP_Hz        (HSI_CLK_FREQUENCY_TYP_Hz / 256)
#define LSI_CLK_FREQUENCY_MAX_Hz        (HSI_CLK_FREQUENCY_MAX_Hz / 256)
#define LSIRDY_TIMEOUT_HARDWARE_TICK    3UL

#define LSE_OSC_CLK_FREQUENCY_MIN_Hz    32000UL
#define LSE_OSC_CLK_FREQUENCY_MAX_Hz    33000UL
#define LSE_BYP_CLK_FREQUENCY_MAX_Hz    1000000UL
#define LSERDY_TIMEOUT_TYP_MS           1000UL

#define PLLCPU_CLK_IN_FREQUENCY_MIN_Hz  8000000UL
#define PLLCPU_CLK_IN_FREQUENCY_MAX_Hz  16000000UL

#define PLLCPU_CLK_OUT_FREQUENCY_MIN_Hz 8000000UL
#if defined(USE_MDR32F02) || defined(USE_MDR1206FI) || defined(USE_MDR1206AFI)
#define PLLCPU_CLK_OUT_FREQUENCY_MAX_Hz CLK_FREQUENCY_MAX_Hz
#elif defined(USE_MDR1206)
#define PLLCPU_CLK_OUT_MDR1206FI_FREQUENCY_MAX_Hz  CLK_MDR1206FI_FREQUENCY_MAX_Hz
#define PLLCPU_CLK_OUT_MDR1206AFI_FREQUENCY_MAX_Hz CLK_MDR1206AFI_FREQUENCY_MAX_Hz
#endif

#define PLLCPURDY_TIMEOUT_HARDWARE_US 34UL

#define ADC_CLK_FREQUENCY_MAX_Hz      28000000UL

#if defined(USE_MDR32F02)
#define ADCUI_CLK_FREQUENCY_MIN_Hz 900000UL
#define ADCUI_CLK_FREQUENCY_MAX_Hz 1100000UL
#elif defined(USE_MDR1206FI) || defined(USE_MDR1206AFI)
#define ADCUI_CLK_FREQUENCY_MIN_Hz 900000UL
#define ADCUI_CLK_FREQUENCY_MAX_Hz 4100000UL
#endif

#define VREF_INT_VOLTAGE_TYP_mV 2450UL

#if defined(USE_MDR1206FI)
#define TRIM_HSI_VALUE_TYP 0x1EUL
#define TRIM_BG_VALUE_TYP  0x14UL
#elif defined(USE_MDR1206AFI)
#define TRIM_HSI_VALUE_TYP 0x2BUL
#define TRIM_BG_VALUE_TYP  0x04UL
#elif defined(USE_MDR1206)
#define TRIM_HSI_MDR1206FI_VALUE_TYP  0x1EUL
#define TRIM_BG_MDR1206FI_VALUE_TYP   0x14UL
#define TRIM_HSI_MDR1206AFI_VALUE_TYP 0x2BUL
#define TRIM_BG_MDR1206AFI_VALUE_TYP  0x04UL
#endif

#define BKP_RTC_CLK_MUX_SWITCH_TICK 3UL

/** @} */ /* End of group MDR32VF0xI_Hardware_Constants */

/** @defgroup MDR32VF0xI_Peripheral_Units MDR32VF0xI Peripheral Units
 * @{
 */

/** @defgroup MDR32VF0xI_SSP MDR_SSP
 * @{
 */

/**
 * @brief MDR32VF0xI SSP control structure.
 */
typedef struct {
    __IO uint32_t CR0;   /*!<SSP Control0 Register */
    __IO uint32_t CR1;   /*!<SSP Control1 Register */
    __IO uint32_t DR;    /*!<SSP Data Register */
    __I uint32_t  SR;    /*!<SSP Status Register */
    __IO uint32_t CPSR;  /*!<SSP Clock Prescaler Register */
    __IO uint32_t IMSC;  /*!<SSP Interrupt Mask Register */
    __I uint32_t  RIS;   /*!<SSP Interrupt Pending Register */
    __I uint32_t  MIS;   /*!<SSP Masked Interrupt Pending Register */
    __O uint32_t  ICR;   /*!<SSP Interrupt Clear Register */
    __IO uint32_t DMACR; /*!<SSP DMA Control Register */
} MDR_SSP_TypeDef;

/** @defgroup MDR32VF0xI_SSP_Defines SSP Defines
 * @{
 */

/** @defgroup MDR32VF0xI_SSP_CR0 SSP CR0
 * @{
 */

#define SSP_CR0_DSS30_Pos     0
#define SSP_CR0_DSS30_Msk     (0xFUL << SSP_CR0_DSS30_Pos)
#define SSP_CR0_DSS_4BIT      (0x3UL << SSP_CR0_DSS30_Pos)
#define SSP_CR0_DSS_5BIT      (0x4UL << SSP_CR0_DSS30_Pos)
#define SSP_CR0_DSS_6BIT      (0x5UL << SSP_CR0_DSS30_Pos)
#define SSP_CR0_DSS_7BIT      (0x6UL << SSP_CR0_DSS30_Pos)
#define SSP_CR0_DSS_8BIT      (0x7UL << SSP_CR0_DSS30_Pos)
#define SSP_CR0_DSS_9BIT      (0x8UL << SSP_CR0_DSS30_Pos)
#define SSP_CR0_DSS_10BIT     (0x9UL << SSP_CR0_DSS30_Pos)
#define SSP_CR0_DSS_11BIT     (0xAUL << SSP_CR0_DSS30_Pos)
#define SSP_CR0_DSS_12BIT     (0xBUL << SSP_CR0_DSS30_Pos)
#define SSP_CR0_DSS_13BIT     (0xCUL << SSP_CR0_DSS30_Pos)
#define SSP_CR0_DSS_14BIT     (0xDUL << SSP_CR0_DSS30_Pos)
#define SSP_CR0_DSS_15BIT     (0xEUL << SSP_CR0_DSS30_Pos)
#define SSP_CR0_DSS_16BIT     (0xFUL << SSP_CR0_DSS30_Pos)

#define SSP_CR0_FRF_Pos       4
#define SSP_CR0_FRF_Msk       (0x3UL << SSP_CR0_FRF_Pos)
#define SSP_CR0_FRF_SPI       (0x0UL << SSP_CR0_FRF_Pos)
#define SSP_CR0_FRF_SSI       (0x1UL << SSP_CR0_FRF_Pos)
#define SSP_CR0_FRF_MICROWIRE (0x2UL << SSP_CR0_FRF_Pos)

#define SSP_CR0_SPO_Pos       6
#define SSP_CR0_SPO_Msk       (0x1UL << SSP_CR0_SPO_Pos)
#define SSP_CR0_SPO           SSP_CR0_SPO_Msk

#define SSP_CR0_SPH_Pos       7
#define SSP_CR0_SPH_Msk       (0x1UL << SSP_CR0_SPH_Pos)
#define SSP_CR0_SPH           SSP_CR0_SPH_Msk

#define SSP_CR0_SCR_Pos       8
#define SSP_CR0_SCR_Msk       (0xFFUL << SSP_CR0_SCR_Pos)

#if defined(USE_MDR32F02_REV_1X)

#define SSP_CR0_DSS_Msk SSP_CR0_DSS30_Msk

#elif defined(USE_MDR32F02_REV_2) || defined(USE_MDR1206FI) || defined(USE_MDR1206AFI) || defined(USE_MDR1206)

#define SSP_CR0_DSS4_Pos   16
#define SSP_CR0_DSS4_Msk   (0x1UL << SSP_CR0_DSS4_Pos)
#define SSP_CR0_DSS4       SSP_CR0_DSS4_Msk

#define SSP_CR0_DSS_Msk    (SSP_CR0_DSS30_Msk | SSP_CR0_DSS4_Msk)
#define SSP_CR0_DSS_17BIT  ((0x0UL << SSP_CR0_DSS30_Pos) | SSP_CR0_DSS4)
#define SSP_CR0_DSS_18BIT  ((0x1UL << SSP_CR0_DSS30_Pos) | SSP_CR0_DSS4)
#define SSP_CR0_DSS_19BIT  ((0x2UL << SSP_CR0_DSS30_Pos) | SSP_CR0_DSS4)
#define SSP_CR0_DSS_20BIT  ((0x3UL << SSP_CR0_DSS30_Pos) | SSP_CR0_DSS4)
#define SSP_CR0_DSS_21BIT  ((0x4UL << SSP_CR0_DSS30_Pos) | SSP_CR0_DSS4)
#define SSP_CR0_DSS_22BIT  ((0x5UL << SSP_CR0_DSS30_Pos) | SSP_CR0_DSS4)
#define SSP_CR0_DSS_23BIT  ((0x6UL << SSP_CR0_DSS30_Pos) | SSP_CR0_DSS4)
#define SSP_CR0_DSS_24BIT  ((0x7UL << SSP_CR0_DSS30_Pos) | SSP_CR0_DSS4)
#define SSP_CR0_DSS_25BIT  ((0x8UL << SSP_CR0_DSS30_Pos) | SSP_CR0_DSS4)
#define SSP_CR0_DSS_26BIT  ((0x9UL << SSP_CR0_DSS30_Pos) | SSP_CR0_DSS4)
#define SSP_CR0_DSS_27BIT  ((0xAUL << SSP_CR0_DSS30_Pos) | SSP_CR0_DSS4)
#define SSP_CR0_DSS_28BIT  ((0xBUL << SSP_CR0_DSS30_Pos) | SSP_CR0_DSS4)
#define SSP_CR0_DSS_29BIT  ((0xCUL << SSP_CR0_DSS30_Pos) | SSP_CR0_DSS4)
#define SSP_CR0_DSS_30BIT  ((0xDUL << SSP_CR0_DSS30_Pos) | SSP_CR0_DSS4)
#define SSP_CR0_DSS_31BIT  ((0xEUL << SSP_CR0_DSS30_Pos) | SSP_CR0_DSS4)
#define SSP_CR0_DSS_32BIT  ((0xFUL << SSP_CR0_DSS30_Pos) | SSP_CR0_DSS4)

#define SSP_CR0_SSPFRX_Pos 17
#define SSP_CR0_SSPFRX_Msk (0x1UL << SSP_CR0_SSPFRX_Pos)
#define SSP_CR0_SSPFRX     SSP_CR0_SSPFRX_Msk

#endif

/** @} */ /* End of group MDR32VF0xI_SSP_CR0 */

/** @defgroup MDR32VF0xI_SSP_CR1 SSP CR1
 * @{
 */

#define SSP_CR1_LBM_Pos 0
#define SSP_CR1_LBM_Msk (0x1UL << SSP_CR1_LBM_Pos)
#define SSP_CR1_LBM     SSP_CR1_LBM_Msk

#define SSP_CR1_SSE_Pos 1
#define SSP_CR1_SSE_Msk (0x1UL << SSP_CR1_SSE_Pos)
#define SSP_CR1_SSE     SSP_CR1_SSE_Msk

#define SSP_CR1_MS_Pos  2
#define SSP_CR1_MS_Msk  (0x1UL << SSP_CR1_MS_Pos)
#define SSP_CR1_MS      SSP_CR1_MS_Msk

#define SSP_CR1_SOD_Pos 3
#define SSP_CR1_SOD_Msk (0x1UL << SSP_CR1_SOD_Pos)
#define SSP_CR1_SOD     SSP_CR1_SOD_Msk

#if defined(USE_MDR32F02_REV_2) || defined(USE_MDR1206FI) || defined(USE_MDR1206AFI) || defined(USE_MDR1206)

#define SSP_CR1_RESTXFIFO_Pos 4
#define SSP_CR1_RESTXFIFO_Msk (0x1UL << SSP_CR1_RESTXFIFO_Pos)
#define SSP_CR1_RESTXFIFO     SSP_CR1_RESTXFIFO_Msk

#endif

/** @} */ /* End of group MDR32VF0xI_SSP_CR1 */

/** @defgroup MDR32VF0xI_SSP_DR SSP DR
 * @{
 */

#define SSP_DR_DATA_Pos 0
#if defined(USE_MDR32F02_REV_1X)
#define SSP_DR_DATA_Msk (0xFFFFUL << SSP_DR_DATA_Pos)
#else
#define SSP_DR_DATA_Msk (0xFFFFFFFFUL << SSP_DR_DATA_Pos)
#endif

/** @} */ /* End of group MDR32VF0xI_SSP_DR */

/** @defgroup MDR32VF0xI_SSP_SR SSP SR
 * @{
 */

#define SSP_SR_TFE_Pos 0
#define SSP_SR_TFE_Msk (0x1UL << SSP_SR_TFE_Pos)
#define SSP_SR_TFE     SSP_SR_TFE_Msk

#define SSP_SR_TNF_Pos 1
#define SSP_SR_TNF_Msk (0x1UL << SSP_SR_TNF_Pos)
#define SSP_SR_TNF     SSP_SR_TNF_Msk

#define SSP_SR_RNE_Pos 2
#define SSP_SR_RNE_Msk (0x1UL << SSP_SR_RNE_Pos)
#define SSP_SR_RNE     SSP_SR_RNE_Msk

#define SSP_SR_RFF_Pos 3
#define SSP_SR_RFF_Msk (0x1UL << SSP_SR_RFF_Pos)
#define SSP_SR_RFF     SSP_SR_RFF_Msk

#define SSP_SR_BSY_Pos 4
#define SSP_SR_BSY_Msk (0x1UL << SSP_SR_BSY_Pos)
#define SSP_SR_BSY     SSP_SR_BSY_Msk

/** @} */ /* End of group MDR32VF0xI_SSP_SR */

/** @defgroup MDR32VF0xI_SSP_CPSR SSP CPSR
 * @{
 */

#define SSP_CPSR_CPSDVSR_Pos 0
#define SSP_CPSR_CPSDVSR_Msk (0xFFUL << SSP_CPSR_CPSDVSR_Pos)

/** @} */ /* End of group MDR32VF0xI_SSP_CPSR */

/** @defgroup MDR32VF0xI_SSP_IMSC SSP IMSC
 * @{
 */

#define SSP_IMSC_RORIM_Pos 0
#define SSP_IMSC_RORIM_Msk (0x1UL << SSP_IMSC_RORIM_Pos)
#define SSP_IMSC_RORIM     SSP_IMSC_RORIM_Msk

#define SSP_IMSC_RTIM_Pos  1
#define SSP_IMSC_RTIM_Msk  (0x1UL << SSP_IMSC_RTIM_Pos)
#define SSP_IMSC_RTIM      SSP_IMSC_RTIM_Msk

#define SSP_IMSC_RXIM_Pos  2
#define SSP_IMSC_RXIM_Msk  (0x1UL << SSP_IMSC_RXIM_Pos)
#define SSP_IMSC_RXIM      SSP_IMSC_RXIM_Msk

#define SSP_IMSC_TXIM_Pos  3
#define SSP_IMSC_TXIM_Msk  (0x1UL << SSP_IMSC_TXIM_Pos)
#define SSP_IMSC_TXIM      SSP_IMSC_TXIM_Msk

#if defined(USE_MDR32F02_REV_2) || defined(USE_MDR1206FI) || defined(USE_MDR1206AFI) || defined(USE_MDR1206)

#define SSP_IMSC_RNEIM_Pos   4
#define SSP_IMSC_RNEIM_Msk   (0x1UL << SSP_IMSC_RNEIM_Pos)
#define SSP_IMSC_RNEIM       SSP_IMSC_RNEIM_Msk

#define SSP_IMSC_TFEIM_Pos   5
#define SSP_IMSC_TFEIM_Msk   (0x1UL << SSP_IMSC_TFEIM_Pos)
#define SSP_IMSC_TFEIM       SSP_IMSC_TFEIM_Msk

#define SSP_IMSC_TNBSYIM_Pos 6
#define SSP_IMSC_TNBSYIM_Msk (0x1UL << SSP_IMSC_TNBSYIM_Pos)
#define SSP_IMSC_TNBSYIM     SSP_IMSC_TNBSYIM_Msk

#endif

/** @} */ /* End of group MDR32VF0xI_SSP_IMSC */

/** @defgroup MDR32VF0xI_SSP_RIS SSP RIS
 * @{
 */

#define SSP_RIS_RORRIS_Pos 0
#define SSP_RIS_RORRIS_Msk (0x1UL << SSP_RIS_RORRIS_Pos)
#define SSP_RIS_RORRIS     SSP_RIS_RORRIS_Msk

#define SSP_RIS_RTRIS_Pos  1
#define SSP_RIS_RTRIS_Msk  (0x1UL << SSP_RIS_RTRIS_Pos)
#define SSP_RIS_RTRIS      SSP_RIS_RTRIS_Msk

#define SSP_RIS_RXRIS_Pos  2
#define SSP_RIS_RXRIS_Msk  (0x1UL << SSP_RIS_RXRIS_Pos)
#define SSP_RIS_RXRIS      SSP_RIS_RXRIS_Msk

#define SSP_RIS_TXRIS_Pos  3
#define SSP_RIS_TXRIS_Msk  (0x1UL << SSP_RIS_TXRIS_Pos)
#define SSP_RIS_TXRIS      SSP_RIS_TXRIS_Msk

#if defined(USE_MDR32F02_REV_2) || defined(USE_MDR1206FI) || defined(USE_MDR1206AFI) || defined(USE_MDR1206)

#define SSP_IMSC_RNERIS_Pos   4
#define SSP_IMSC_RNERIS_Msk   (0x1UL << SSP_IMSC_RNERIS_Pos)
#define SSP_IMSC_RNERIS       SSP_IMSC_RNERIS_Msk

#define SSP_IMSC_TFERIS_Pos   5
#define SSP_IMSC_TFERIS_Msk   (0x1UL << SSP_IMSC_TFERIS_Pos)
#define SSP_IMSC_TFERIS       SSP_IMSC_TFERIS_Msk

#define SSP_IMSC_TNBSYRIS_Pos 6
#define SSP_IMSC_TNBSYRIS_Msk (0x1UL << SSP_IMSC_TNBSYRIS_Pos)
#define SSP_IMSC_TNBSYRIS     SSP_IMSC_TNBSYRIS_Msk

#endif

/** @} */ /* End of group MDR32VF0xI_SSP_RIS */

/** @defgroup MDR32VF0xI_SSP_MIS SSP MIS
 * @{
 */

#define SSP_MIS_RORMIS_Pos 0
#define SSP_MIS_RORMIS_Msk (0x1UL << SSP_MIS_RORMIS_Pos)
#define SSP_MIS_RORMIS     SSP_MIS_RORMIS_Msk

#define SSP_MIS_RTMIS_Pos  1
#define SSP_MIS_RTMIS_Msk  (0x1UL << SSP_MIS_RTMIS_Pos)
#define SSP_MIS_RTMIS      SSP_MIS_RTMIS_Msk

#define SSP_MIS_RXMIS_Pos  2
#define SSP_MIS_RXMIS_Msk  (0x1UL << SSP_MIS_RXMIS_Pos)
#define SSP_MIS_RXMIS      SSP_MIS_RXMIS_Msk

#define SSP_MIS_TXMIS_Pos  3
#define SSP_MIS_TXMIS_Msk  (0x1UL << SSP_MIS_TXMIS_Pos)
#define SSP_MIS_TXMIS      SSP_MIS_TXMIS_Msk

#if defined(USE_MDR32F02_REV_2) || defined(USE_MDR1206FI) || defined(USE_MDR1206AFI) || defined(USE_MDR1206)

#define SSP_MIS_RNEMIS_Pos   4
#define SSP_MIS_RNEMIS_Msk   (0x1UL << SSP_MIS_RNEMIS_Pos)
#define SSP_MIS_RNERIS       SSP_MIS_RNEMIS_Msk

#define SSP_MIS_TFEMIS_Pos   5
#define SSP_MIS_TFEMIS_Msk   (0x1UL << SSP_MIS_TFEMIS_Pos)
#define SSP_MIS_TFERIS       SSP_MIS_TFEMIS_Msk

#define SSP_MIS_TNBSYMIS_Pos 6
#define SSP_MIS_TNBSYMIS_Msk (0x1UL << SSP_MIS_TNBSYMIS_Pos)
#define SSP_MIS_TNBSYRIS     SSP_MIS_TNBSYMIS_Msk

#endif

/** @} */ /* End of group MDR32VF0xI_SSP_MIS */

/** @defgroup MDR32VF0xI_SSP_ICR SSP ICR
 * @{
 */

#define SSP_ICR_RORIC_Pos 0
#define SSP_ICR_RORIC_Msk (0x1UL << SSP_ICR_RORIC_Pos)
#define SSP_ICR_RORIC     SSP_ICR_RORIC_Msk

#define SSP_ICR_RTIC_Pos  1
#define SSP_ICR_RTIC_Msk  (0x1UL << SSP_ICR_RTIC_Pos)
#define SSP_ICR_RTIC      SSP_ICR_RTIC_Msk

/** @} */ /* End of group MDR32VF0xI_SSP_ICR */

/** @defgroup MDR32VF0xI_SSP_DMACR SSP DMACR
 * @{
 */

#define SSP_DMACR_RXDMAE_Pos 0
#define SSP_DMACR_RXDMAE_Msk (0x1UL << SSP_DMACR_RXDMAE_Pos)
#define SSP_DMACR_RXDMAE     SSP_DMACR_RXDMAE_Msk

#define SSP_DMACR_TXDMAE_Pos 1
#define SSP_DMACR_TXDMAE_Msk (0x1UL << SSP_DMACR_TXDMAE_Pos)
#define SSP_DMACR_TXDMAE     SSP_DMACR_TXDMAE_Msk

/** @} */ /* End of group MDR32VF0xI_SSP_DMACR */

/** @} */ /* End of group MDR32VF0xI_SSP_Defines */

/** @} */ /* End of group MDR32VF0xI_SSP */

/** @defgroup MDR32VF0xI_UART MDR_UART
 * @{
 */

/**
 * @brief MDR32VF0xI UART control structure.
 */
typedef struct {
    __IO uint32_t DR;      /*!<UART Data Register */
    __IO uint32_t RSR_ECR; /*!<UART Receive Status Register / Error Clear Register */
    __I uint32_t  RESERVED0[4];
    __I uint32_t  FR; /*!<UART Flag Register */
    __I uint32_t  RESERVED1;
    __IO uint32_t ILPR;  /*!<UART IrDA Low-Power Counter Register */
    __IO uint32_t IBRD;  /*!<UART Integer Baud Rate Register */
    __IO uint32_t FBRD;  /*!<UART Fractional Baud Rate Register */
    __IO uint32_t LCR_H; /*!<UART Line Control Register */
    __IO uint32_t CR;    /*!<UART Control Register */
    __IO uint32_t IFLS;  /*!<UART Interrupt FIFO Level Select Register */
    __IO uint32_t IMSC;  /*!<UART Interrupt Mask Set/Clear Register */
    __I uint32_t  RIS;   /*!<UART Raw Interrupt Status Register */
    __I uint32_t  MIS;   /*!<UART Masked Interrupt Status Register */
    __O uint32_t  ICR;   /*!<UART Interrupt Clear Register */
    __IO uint32_t DMACR; /*!<UART DMA Control Register */
    __I uint32_t  RESERVED2[13];
    __IO uint32_t TCR; /*!<UART Test Control Register */
} MDR_UART_TypeDef;

/** @defgroup MDR32VF0xI_UART_Defines UART Defines
 * @{
 */

/** @defgroup MDR32VF0xI_UART_DR UART DR
 * @{
 */

#define UART_DR_DATA_Pos 0
#define UART_DR_DATA_Msk (0x1FFUL << UART_DR_DATA_Pos)

#define UART_DR_FE_Pos   9
#define UART_DR_FE_Msk   (0x1UL << UART_DR_FE_Pos)
#define UART_DR_FE       UART_DR_FE_Msk

#define UART_DR_PE_Pos   10
#define UART_DR_PE_Msk   (0x1UL << UART_DR_PE_Pos)
#define UART_DR_PE       UART_DR_PE_Msk

#define UART_DR_BE_Pos   11
#define UART_DR_BE_Msk   (0x1UL << UART_DR_BE_Pos)
#define UART_DR_BE       UART_DR_BE_Msk

#define UART_DR_OE_Pos   12
#define UART_DR_OE_Msk   (0x1UL << UART_DR_OE_Pos)
#define UART_DR_OE       UART_DR_OE_Msk

/** @} */ /* End of group MDR32VF0xI_UART_DR */

/** @defgroup MDR32VF0xI_UART_RSR_ECR UART RSR_ECR
 * @{
 */

#define UART_RSR_ECR_FE_Pos 0
#define UART_RSR_ECR_FE_Msk (0x1UL << UART_RSR_ECR_FE_Pos)
#define UART_RSR_ECR_FE     UART_RSR_ECR_FE_Msk

#define UART_RSR_ECR_PE_Pos 1
#define UART_RSR_ECR_PE_Msk (0x1UL << UART_RSR_ECR_PE_Pos)
#define UART_RSR_ECR_PE     UART_RSR_ECR_PE_Msk

#define UART_RSR_ECR_BE_Pos 2
#define UART_RSR_ECR_BE_Msk (0x1UL << UART_RSR_ECR_BE_Pos)
#define UART_RSR_ECR_BE     UART_RSR_ECR_BE_Msk

#define UART_RSR_ECR_OE_Pos 3
#define UART_RSR_ECR_OE_Msk (0x1UL << UART_RSR_ECR_OE_Pos)
#define UART_RSR_ECR_OE     UART_RSR_ECR_OE_Msk

/** @} */ /* End of group MDR32VF0xI_UART_RSR_ECR */

/** @defgroup MDR32VF0xI_UART_FR UART FR
 * @{
 */

#define UART_FR_CTS_Pos  0
#define UART_FR_CTS_Msk  (0x1UL << UART_FR_CTS_Pos)
#define UART_FR_CTS      UART_FR_CTS_Msk

#define UART_FR_DSR_Pos  1
#define UART_FR_DSR_Msk  (0x1UL << UART_FR_DSR_Pos)
#define UART_FR_DSR      UART_FR_DSR_Msk

#define UART_FR_DCD_Pos  2
#define UART_FR_DCD_Msk  (0x1UL << UART_FR_DCD_Pos)
#define UART_FR_DCD      UART_FR_DCD_Msk

#define UART_FR_BUSY_Pos 3
#define UART_FR_BUSY_Msk (0x1UL << UART_FR_BUSY_Pos)
#define UART_FR_BUSY     UART_FR_BUSY_Msk

#define UART_FR_RXFE_Pos 4
#define UART_FR_RXFE_Msk (0x1UL << UART_FR_RXFE_Pos)
#define UART_FR_RXFE     UART_FR_RXFE_Msk

#define UART_FR_TXFF_Pos 5
#define UART_FR_TXFF_Msk (0x1UL << UART_FR_TXFF_Pos)
#define UART_FR_TXFF     UART_FR_TXFF_Msk

#define UART_FR_RXFF_Pos 6
#define UART_FR_RXFF_Msk (0x1UL << UART_FR_RXFF_Pos)
#define UART_FR_RXFF     UART_FR_RXFF_Msk

#define UART_FR_TXFE_Pos 7
#define UART_FR_TXFE_Msk (0x1UL << UART_FR_TXFE_Pos)
#define UART_FR_TXFE     UART_FR_TXFE_Msk

#define UART_FR_RI_Pos   8
#define UART_FR_RI_Msk   (0x1UL << UART_FR_RI_Pos)
#define UART_FR_RI       UART_FR_RI_Msk

/** @} */ /* End of group MDR32VF0xI_UART_FR */

/** @defgroup MDR32VF0xI_UART_ILPR UART ILPR
 * @{
 */

#define UART_ILPR_ILPDVSR_Pos 0
#define UART_ILPR_ILPDVSR_Msk (0xFFUL << UART_ILPR_ILPDVSR_Pos)

/** @} */ /* End of group MDR32VF0xI_UART_ILPR */

/** @defgroup MDR32VF0xI_UART_IBRD UART IBRD
 * @{
 */

#define UART_IBRD_BAUDDIV_INT_Pos 0
#define UART_IBRD_BAUDDIV_INT_Msk (0xFFFFUL << UART_IBRD_BAUDDIV_INT_Pos)

/** @} */ /* End of group MDR32VF0xI_UART_IBRD */

/** @defgroup MDR32VF0xI_UART_FBRD UART FBRD
 * @{
 */

#define UART_FBRD_BAUDDIV_FRAC_Pos 0
#define UART_FBRD_BAUDDIV_FRAC_Msk (0x3FUL << UART_FBRD_BAUDDIV_FRAC_Pos)

/** @} */ /* End of group MDR32VF0xI_UART_FBRD */

/** @defgroup MDR32VF0xI_UART_LCR_H UART LCR_H
 * @{
 */

#define UART_LCR_H_BRK_Pos    0
#define UART_LCR_H_BRK_Msk    (0x1UL << UART_LCR_H_BRK_Pos)
#define UART_LCR_H_BRK        UART_LCR_H_BRK_Msk

#define UART_LCR_H_PEN_Pos    1
#define UART_LCR_H_PEN_Msk    (0x1UL << UART_LCR_H_PEN_Pos)
#define UART_LCR_H_PEN        UART_LCR_H_PEN_Msk

#define UART_LCR_H_EPS_Pos    2
#define UART_LCR_H_EPS_Msk    (0x1UL << UART_LCR_H_EPS_Pos)
#define UART_LCR_H_EPS        UART_LCR_H_EPS_Msk

#define UART_LCR_H_STP2_Pos   3
#define UART_LCR_H_STP2_Msk   (0x1UL << UART_LCR_H_STP2_Pos)
#define UART_LCR_H_STP2       UART_LCR_H_STP2_Msk

#define UART_LCR_H_FEN_Pos    4
#define UART_LCR_H_FEN_Msk    (0x1UL << UART_LCR_H_FEN_Pos)
#define UART_LCR_H_FEN        UART_LCR_H_FEN_Msk

#define UART_LCR_H_WLEN10_Pos 5
#define UART_LCR_H_WLEN10_Msk (0x3UL << UART_LCR_H_WLEN10_Pos)

#define UART_LCR_H_SPS_Pos    7
#define UART_LCR_H_SPS_Msk    (0x1UL << UART_LCR_H_SPS_Pos)
#define UART_LCR_H_SPS        UART_LCR_H_SPS_Msk

#define UART_LCR_H_WLEN2_Pos  8
#define UART_LCR_H_WLEN2_Msk  (0x1UL << UART_LCR_H_WLEN2_Pos)

#define UART_LCR_H_WLEN_Msk   (UART_LCR_H_WLEN10_Msk | UART_LCR_H_WLEN2_Msk)
#define UART_LCR_H_WLEN_5BIT  (0x0UL << UART_LCR_H_WLEN10_Pos)
#define UART_LCR_H_WLEN_6BIT  (0x1UL << UART_LCR_H_WLEN10_Pos)
#define UART_LCR_H_WLEN_7BIT  (0x2UL << UART_LCR_H_WLEN10_Pos)
#define UART_LCR_H_WLEN_8BIT  (0x3UL << UART_LCR_H_WLEN10_Pos)
#define UART_LCR_H_WLEN_9BIT  (0x1UL << UART_LCR_H_WLEN2_Pos)

/** @} */ /* End of group MDR32VF0xI_UART_LCR_H */

/** @defgroup MDR32VF0xI_UART_CR UART CR
 * @{
 */

#define UART_CR_UARTEN_Pos 0
#define UART_CR_UARTEN_Msk (0x1UL << UART_CR_UARTEN_Pos)
#define UART_CR_UARTEN     UART_CR_UARTEN_Msk

#define UART_CR_SIREN_Pos  1
#define UART_CR_SIREN_Msk  (0x1UL << UART_CR_SIREN_Pos)
#define UART_CR_SIREN      UART_CR_SIREN_Msk

#define UART_CR_SIRLP_Pos  2
#define UART_CR_SIRLP_Msk  (0x1UL << UART_CR_SIRLP_Pos)
#define UART_CR_SIRLP      UART_CR_SIRLP_Msk

#define UART_CR_LBE_Pos    7
#define UART_CR_LBE_Msk    (0x1UL << UART_CR_LBE_Pos)
#define UART_CR_LBE        UART_CR_LBE_Msk

#define UART_CR_TXE_Pos    8
#define UART_CR_TXE_Msk    (0x1UL << UART_CR_TXE_Pos)
#define UART_CR_TXE        UART_CR_TXE_Msk

#define UART_CR_RXE_Pos    9
#define UART_CR_RXE_Msk    (0x1UL << UART_CR_RXE_Pos)
#define UART_CR_RXE        UART_CR_RXE_Msk

#define UART_CR_DTR_Pos    10
#define UART_CR_DTR_Msk    (0x1UL << UART_CR_DTR_Pos)
#define UART_CR_DTR        UART_CR_DTR_Msk

#define UART_CR_RTS_Pos    11
#define UART_CR_RTS_Msk    (0x1UL << UART_CR_RTS_Pos)
#define UART_CR_RTS        UART_CR_RTS_Msk

#define UART_CR_OUT1_Pos   12
#define UART_CR_OUT1_Msk   (0x1UL << UART_CR_OUT1_Pos)
#define UART_CR_OUT1       UART_CR_OUT1_Msk

#define UART_CR_OUT2_Pos   13
#define UART_CR_OUT2_Msk   (0x1UL << UART_CR_OUT2_Pos)
#define UART_CR_OUT2       UART_CR_OUT2_Msk

#define UART_CR_RTSEN_Pos  14
#define UART_CR_RTSEN_Msk  (0x1UL << UART_CR_RTSEN_Pos)
#define UART_CR_RTSEN      UART_CR_RTSEN_Msk

#define UART_CR_CTSEN_Pos  15
#define UART_CR_CTSEN_Msk  (0x1UL << UART_CR_CTSEN_Pos)
#define UART_CR_CTSEN      UART_CR_CTSEN_Msk

/** @} */ /* End of group MDR32VF0xI_UART_CR */

/** @defgroup MDR32VF0xI_UART_IFLS UART IFLS
 * @{
 */

#define UART_IFLS_TXIFLSEL_Pos    0
#define UART_IFLS_TXIFLSEL_Msk    (0x7UL << UART_IFLS_TXIFLSEL_Pos)
#define UART_IFLS_TXIFLSEL_2WORD  (0x0UL << UART_IFLS_TXIFLSEL_Pos)
#define UART_IFLS_TXIFLSEL_4WORD  (0x1UL << UART_IFLS_TXIFLSEL_Pos)
#define UART_IFLS_TXIFLSEL_8WORD  (0x2UL << UART_IFLS_TXIFLSEL_Pos)
#define UART_IFLS_TXIFLSEL_12WORD (0x3UL << UART_IFLS_TXIFLSEL_Pos)
#define UART_IFLS_TXIFLSEL_14WORD (0x4UL << UART_IFLS_TXIFLSEL_Pos)

#define UART_IFLS_RXIFLSEL_Pos    3
#define UART_IFLS_RXIFLSEL_Msk    (0x7UL << UART_IFLS_RXIFLSEL_Pos)
#define UART_IFLS_RXIFLSEL_2WORD  (0x0UL << UART_IFLS_RXIFLSEL_Pos)
#define UART_IFLS_RXIFLSEL_4WORD  (0x1UL << UART_IFLS_RXIFLSEL_Pos)
#define UART_IFLS_RXIFLSEL_8WORD  (0x2UL << UART_IFLS_RXIFLSEL_Pos)
#define UART_IFLS_RXIFLSEL_12WORD (0x3UL << UART_IFLS_RXIFLSEL_Pos)
#define UART_IFLS_RXIFLSEL_14WORD (0x4UL << UART_IFLS_RXIFLSEL_Pos)

/** @} */ /* End of group MDR32VF0xI_UART_IFLS */

/** @defgroup MDR32VF0xI_UART_IMSC UART IMSC
 * @{
 */

#define UART_IMSC_RIMIM_Pos   0
#define UART_IMSC_RIMIM_Msk   (0x1UL << UART_IMSC_RIMIM_Pos)
#define UART_IMSC_RIMIM       UART_IMSC_RIMIM_Msk

#define UART_IMSC_CTSMIM_Pos  1
#define UART_IMSC_CTSMIM_Msk  (0x1UL << UART_IMSC_CTSMIM_Pos)
#define UART_IMSC_CTSMIM      UART_IMSC_CTSMIM_Msk

#define UART_IMSC_DCDMIM_Pos  2
#define UART_IMSC_DCDMIM_Msk  (0x1UL << UART_IMSC_DCDMIM_Pos)
#define UART_IMSC_DCDMIM      UART_IMSC_DCDMIM_Msk

#define UART_IMSC_DSRMIM_Pos  3
#define UART_IMSC_DSRMIM_Msk  (0x1UL << UART_IMSC_DSRMIM_Pos)
#define UART_IMSC_DSRMIM      UART_IMSC_DSRMIM_Msk

#define UART_IMSC_RXIM_Pos    4
#define UART_IMSC_RXIM_Msk    (0x1UL << UART_IMSC_RXIM_Pos)
#define UART_IMSC_RXIM        UART_IMSC_RXIM_Msk

#define UART_IMSC_TXIM_Pos    5
#define UART_IMSC_TXIM_Msk    (0x1UL << UART_IMSC_TXIM_Pos)
#define UART_IMSC_TXIM        UART_IMSC_TXIM_Msk

#define UART_IMSC_RTIM_Pos    6
#define UART_IMSC_RTIM_Msk    (0x1UL << UART_IMSC_RTIM_Pos)
#define UART_IMSC_RTIM        UART_IMSC_RTIM_Msk

#define UART_IMSC_FEIM_Pos    7
#define UART_IMSC_FEIM_Msk    (0x1UL << UART_IMSC_FEIM_Pos)
#define UART_IMSC_FEIM        UART_IMSC_FEIM_Msk

#define UART_IMSC_PEIM_Pos    8
#define UART_IMSC_PEIM_Msk    (0x1UL << UART_IMSC_PEIM_Pos)
#define UART_IMSC_PEIM        UART_IMSC_PEIM_Msk

#define UART_IMSC_BEIM_Pos    9
#define UART_IMSC_BEIM_Msk    (0x1UL << UART_IMSC_BEIM_Pos)
#define UART_IMSC_BEIM        UART_IMSC_BEIM_Msk

#define UART_IMSC_OEIM_Pos    10
#define UART_IMSC_OEIM_Msk    (0x1UL << UART_IMSC_OEIM_Pos)
#define UART_IMSC_OEIM        UART_IMSC_OEIM_Msk

#define UART_IMSC_RNEIM_Pos   11
#define UART_IMSC_RNEIM_Msk   (0x1UL << UART_IMSC_RNEIM_Pos)
#define UART_IMSC_RNEIM       UART_IMSC_RNEIM_Msk

#define UART_IMSC_TFEIM_Pos   12
#define UART_IMSC_TFEIM_Msk   (0x1UL << UART_IMSC_TFEIM_Pos)
#define UART_IMSC_TFEIM       UART_IMSC_TFEIM_Msk

#define UART_IMSC_TNBSYIM_Pos 13
#define UART_IMSC_TNBSYIM_Msk (0x1UL << UART_IMSC_TNBSYIM_Pos)
#define UART_IMSC_TNBSYIM     UART_IMSC_TNBSYIM_Msk

/** @} */ /* End of group MDR32VF0xI_UART_IMSC */

/** @defgroup MDR32VF0xI_UART_RIS UART RIS
 * @{
 */

#define UART_RIS_RIRMIS_Pos   0
#define UART_RIS_RIRMIS_Msk   (0x1UL << UART_RIS_RIRMIS_Pos)
#define UART_RIS_RIRMIS       UART_RIS_RIRMIS_Msk

#define UART_RIS_CTSRMIS_Pos  1
#define UART_RIS_CTSRMIS_Msk  (0x1UL << UART_RIS_CTSRMIS_Pos)
#define UART_RIS_CTSRMIS      UART_RIS_CTSRMIS_Msk

#define UART_RIS_DCDRMIS_Pos  2
#define UART_RIS_DCDRMIS_Msk  (0x1UL << UART_RIS_DCDRMIS_Pos)
#define UART_RIS_DCDRMIS      UART_RIS_DCDRMIS_Msk

#define UART_RIS_DSRRMIS_Pos  3
#define UART_RIS_DSRRMIS_Msk  (0x1UL << UART_RIS_DSRRMIS_Pos)
#define UART_RIS_DSRRMIS      UART_RIS_DSRRMIS_Msk

#define UART_RIS_RXRIS_Pos    4
#define UART_RIS_RXRIS_Msk    (0x1UL << UART_RIS_RXRIS_Pos)
#define UART_RIS_RXRIS        UART_RIS_RXRIS_Msk

#define UART_RIS_TXRIS_Pos    5
#define UART_RIS_TXRIS_Msk    (0x1UL << UART_RIS_TXRIS_Pos)
#define UART_RIS_TXRIS        UART_RIS_TXRIS_Msk

#define UART_RIS_RTRIS_Pos    6
#define UART_RIS_RTRIS_Msk    (0x1UL << UART_RIS_RTRIS_Pos)
#define UART_RIS_RTRIS        UART_RIS_RTRIS_Msk

#define UART_RIS_FERIS_Pos    7
#define UART_RIS_FERIS_Msk    (0x1UL << UART_RIS_FERIS_Pos)
#define UART_RIS_FERIS        UART_RIS_FERIS_Msk

#define UART_RIS_PERIS_Pos    8
#define UART_RIS_PERIS_Msk    (0x1UL << UART_RIS_PERIS_Pos)
#define UART_RIS_PERIS        UART_RIS_PERIS_Msk

#define UART_RIS_BERIS_Pos    9
#define UART_RIS_BERIS_Msk    (0x1UL << UART_RIS_BERIS_Pos)
#define UART_RIS_BERIS        UART_RIS_BERIS_Msk

#define UART_RIS_OERIS_Pos    10
#define UART_RIS_OERIS_Msk    (0x1UL << UART_RIS_OERIS_Pos)
#define UART_RIS_OERIS        UART_RIS_OERIS_Msk

#define UART_RIS_RNERIS_Pos   11
#define UART_RIS_RNERIS_Msk   (0x1UL << UART_RIS_RNERIS_Pos)
#define UART_RIS_RNERIS       UART_RIS_RNERIS_Msk

#define UART_RIS_TFERIS_Pos   12
#define UART_RIS_TFERIS_Msk   (0x1UL << UART_RIS_TFERIS_Pos)
#define UART_RIS_TFERIS       UART_RIS_TFERIS_Msk

#define UART_RIS_TNBSYRIS_Pos 13
#define UART_RIS_TNBSYRIS_Msk (0x1UL << UART_RIS_TNBSYRIS_Pos)
#define UART_RIS_TNBSYRIS     UART_RIS_TNBSYRIS_Msk

/** @} */ /* End of group MDR32VF0xI_UART_RIS */

/** @defgroup MDR32VF0xI_UART_MIS UART MIS
 * @{
 */

#define UART_MIS_RIMMIS_Pos   0
#define UART_MIS_RIMMIS_Msk   (0x1UL << UART_MIS_RIMMIS_Pos)
#define UART_MIS_RIMMIS       UART_MIS_RIMMIS_Msk

#define UART_MIS_CTSMMIS_Pos  1
#define UART_MIS_CTSMMIS_Msk  (0x1UL << UART_MIS_CTSMMIS_Pos)
#define UART_MIS_CTSMMIS      UART_MIS_CTSMMIS_Msk

#define UART_MIS_DCDMMIS_Pos  2
#define UART_MIS_DCDMMIS_Msk  (0x1UL << UART_MIS_DCDMMIS_Pos)
#define UART_MIS_DCDMMIS      UART_MIS_DCDMMIS_Msk

#define UART_MIS_DSRMMIS_Pos  3
#define UART_MIS_DSRMMIS_Msk  (0x1UL << UART_MIS_DSRMMIS_Pos)
#define UART_MIS_DSRMMIS      UART_MIS_DSRMMIS_Msk

#define UART_MIS_RXMIS_Pos    4
#define UART_MIS_RXMIS_Msk    (0x1UL << UART_MIS_RXMIS_Pos)
#define UART_MIS_RXMIS        UART_MIS_RXMIS_Msk

#define UART_MIS_TXMIS_Pos    5
#define UART_MIS_TXMIS_Msk    (0x1UL << UART_MIS_TXMIS_Pos)
#define UART_MIS_TXMIS        UART_MIS_TXMIS_Msk

#define UART_MIS_RTMIS_Pos    6
#define UART_MIS_RTMIS_Msk    (0x1UL << UART_MIS_RTMIS_Pos)
#define UART_MIS_RTMIS        UART_MIS_RTMIS_Msk

#define UART_MIS_FEMIS_Pos    7
#define UART_MIS_FEMIS_Msk    (0x1UL << UART_MIS_FEMIS_Pos)
#define UART_MIS_FEMIS        UART_MIS_FEMIS_Msk

#define UART_MIS_PEMIS_Pos    8
#define UART_MIS_PEMIS_Msk    (0x1UL << UART_MIS_PEMIS_Pos)
#define UART_MIS_PEMIS        UART_MIS_PEMIS_Msk

#define UART_MIS_BEMIS_Pos    9
#define UART_MIS_BEMIS_Msk    (0x1UL << UART_MIS_BEMIS_Pos)
#define UART_MIS_BEMIS        UART_MIS_BEMIS_Msk

#define UART_MIS_OEMIS_Pos    10
#define UART_MIS_OEMIS_Msk    (0x1UL << UART_MIS_OEMIS_Pos)
#define UART_MIS_OEMIS        UART_MIS_OEMIS_Msk

#define UART_MIS_RNEMIS_Pos   11
#define UART_MIS_RNEMIS_Msk   (0x1UL << UART_MIS_RNEMIS_Pos)
#define UART_MIS_RNEMIS       UART_MIS_RNEMIS_Msk

#define UART_MIS_TFEMIS_Pos   12
#define UART_MIS_TFEMIS_Msk   (0x1UL << UART_MIS_TFEMIS_Pos)
#define UART_MIS_TFEMIS       UART_MIS_TFEMIS_Msk

#define UART_MIS_TNBSYMIS_Pos 13
#define UART_MIS_TNBSYMIS_Msk (0x1UL << UART_MIS_TNBSYMIS_Pos)
#define UART_MIS_TNBSYMIS     UART_MIS_TNBSYMIS_Msk

/** @} */ /* End of group MDR32VF0xI_UART_MIS */

/** @defgroup MDR32VF0xI_UART_ICR UART ICR
 * @{
 */

#define UART_ICR_RIMIC_Pos  0
#define UART_ICR_RIMIC_Msk  (0x1UL << UART_ICR_RIMIC_Pos)
#define UART_ICR_RIMIC      UART_ICR_RIMIC_Msk

#define UART_ICR_CTSMIC_Pos 1
#define UART_ICR_CTSMIC_Msk (0x1UL << UART_ICR_CTSMIC_Pos)
#define UART_ICR_CTSMIC     UART_ICR_CTSMIC_Msk

#define UART_ICR_DCDMIC_Pos 2
#define UART_ICR_DCDMIC_Msk (0x1UL << UART_ICR_DCDMIC_Pos)
#define UART_ICR_DCDMIC     UART_ICR_DCDMIC_Msk

#define UART_ICR_DSRMIC_Pos 3
#define UART_ICR_DSRMIC_Msk (0x1UL << UART_ICR_DSRMIC_Pos)
#define UART_ICR_DSRMIC     UART_ICR_DSRMIC_Msk

#define UART_ICR_RXIC_Pos   4
#define UART_ICR_RXIC_Msk   (0x1UL << UART_ICR_RXIC_Pos)
#define UART_ICR_RXIC       UART_ICR_RXIC_Msk

#define UART_ICR_TXIC_Pos   5
#define UART_ICR_TXIC_Msk   (0x1UL << UART_ICR_TXIC_Pos)
#define UART_ICR_TXIC       UART_ICR_TXIC_Msk

#define UART_ICR_RTIC_Pos   6
#define UART_ICR_RTIC_Msk   (0x1UL << UART_ICR_RTIC_Pos)
#define UART_ICR_RTIC       UART_ICR_RTIC_Msk

#define UART_ICR_FEIC_Pos   7
#define UART_ICR_FEIC_Msk   (0x1UL << UART_ICR_FEIC_Pos)
#define UART_ICR_FEIC       UART_ICR_FEIC_Msk

#define UART_ICR_PEIC_Pos   8
#define UART_ICR_PEIC_Msk   (0x1UL << UART_ICR_PEIC_Pos)
#define UART_ICR_PEIC       UART_ICR_PEIC_Msk

#define UART_ICR_BEIC_Pos   9
#define UART_ICR_BEIC_Msk   (0x1UL << UART_ICR_BEIC_Pos)
#define UART_ICR_BEIC       UART_ICR_BEIC_Msk

#define UART_ICR_OEIC_Pos   10
#define UART_ICR_OEIC_Msk   (0x1UL << UART_ICR_OEIC_Pos)
#define UART_ICR_OEIC       UART_ICR_OEIC_Msk

/** @} */ /* End of group MDR32VF0xI_UART_ICR */

/** @defgroup MDR32VF0xI_UART_DMACR UART DMACR
 * @{
 */

#define UART_DMACR_RXDMAE_Pos   0
#define UART_DMACR_RXDMAE_Msk   (0x1UL << UART_DMACR_RXDMAE_Pos)
#define UART_DMACR_RXDMAE       UART_DMACR_RXDMAE_Msk

#define UART_DMACR_TXDMAE_Pos   1
#define UART_DMACR_TXDMAE_Msk   (0x1UL << UART_DMACR_TXDMAE_Pos)
#define UART_DMACR_TXDMAE       UART_DMACR_TXDMAE_Msk

#define UART_DMACR_DMAONERR_Pos 2
#define UART_DMACR_DMAONERR_Msk (0x1UL << UART_DMACR_DMAONERR_Pos)
#define UART_DMACR_DMAONERR     UART_DMACR_DMAONERR_Msk

/** @} */ /* End of group MDR32VF0xI_UART_DMACR */

/** @defgroup MDR32VF0xI_UART_TCR UART TCR
 * @{
 */

#define UART_TCR_ITEN_Pos     0
#define UART_TCR_ITEN_Msk     (0x1UL << UART_TCR_ITEN_Pos)
#define UART_TCR_ITEN         UART_TCR_ITEN_Msk

#define UART_TCR_TESTFIFO_Pos 1
#define UART_TCR_TESTFIFO_Msk (0x1UL << UART_TCR_TESTFIFO_Pos)
#define UART_TCR_TESTFIFO     UART_TCR_TESTFIFO_Msk

#define UART_TCR_SIRTEST_Pos  2
#define UART_TCR_SIRTEST_Msk  (0x1UL << UART_TCR_SIRTEST_Pos)
#define UART_TCR_SIRTEST      UART_TCR_SIRTEST_Msk

/** @} */ /* End of group MDR32VF0xI_UART_TCR */

/** @} */ /* End of group MDR32VF0xI_UART_Defines */

/** @} */ /* End of group MDR32VF0xI_UART */

/** @defgroup MDR32VF0xI_FLASH MDR_FLASH
 * @{
 */

/**
 * @brief MDR32VF0xI FLASH control structure.
 */
typedef struct {
    __IO uint32_t CMD; /*!<FLASH Command Register */
    __IO uint32_t ADR; /*!<FLASH Address Register */
    __IO uint32_t DI;  /*!<FLASH Write Data Register */
    __I uint32_t  DO;  /*!<FLASH Read Data Register */
    __IO uint32_t KEY; /*!<FLASH Key Register */
#if defined(USE_MDR1206AFI) || defined(USE_MDR1206)
    __IO uint32_t CTRL; /*!<FLASH Control and Test Register */
#endif
#if defined(USE_MDR1206FI) || defined(USE_MDR1206AFI) || defined(USE_MDR1206)
    __IO uint32_t CHIP_ID_CTRL; /*!<FLASH Chip ID and OTP Control Register */
    __IO uint32_t OTP_KEY;      /*!<FLASH OTP Key Register */
#endif
} MDR_FLASH_TypeDef;

/** @defgroup MDR32VF0xI_FLASH_Defines FLASH Defines
 * @{
 */

/** @defgroup MDR32VF0xI_FLASH_CMD FLASH CMD
 * @{
 */

#if defined(USE_MDR32F02) || defined(USE_MDR1206FI) || defined(USE_MDR1206AFI) || defined(USE_MDR1206)

#define FLASH_CMD_CON_Pos   0
#define FLASH_CMD_CON_Msk   (0x1UL << FLASH_CMD_CON_Pos)
#define FLASH_CMD_CON       FLASH_CMD_CON_Msk

#define FLASH_CMD_ERASE_Pos 10
#define FLASH_CMD_ERASE_Msk (0x1UL << FLASH_CMD_ERASE_Pos)
#define FLASH_CMD_ERASE     FLASH_CMD_ERASE_Msk

#define FLASH_CMD_PROG_Pos  12
#define FLASH_CMD_PROG_Msk  (0x1UL << FLASH_CMD_PROG_Pos)
#define FLASH_CMD_PROG      FLASH_CMD_PROG_Msk

#endif

#if defined(USE_MDR32F02) || defined(USE_MDR1206FI) || defined(USE_MDR1206)

#define FLASH_CMD_DELAY_Pos     3
#define FLASH_CMD_DELAY_Msk     (0x7UL << FLASH_CMD_DELAY_Pos)
#define FLASH_CMD_DELAY_CYCLE_0 (0x0UL << FLASH_CMD_DELAY_Pos)
#define FLASH_CMD_DELAY_CYCLE_1 (0x1UL << FLASH_CMD_DELAY_Pos)

#define FLASH_CMD_XE_Pos        6
#define FLASH_CMD_XE_Msk        (0x1UL << FLASH_CMD_XE_Pos)
#define FLASH_CMD_XE            FLASH_CMD_XE_Msk

#define FLASH_CMD_YE_Pos        7
#define FLASH_CMD_YE_Msk        (0x1UL << FLASH_CMD_YE_Pos)
#define FLASH_CMD_YE            FLASH_CMD_YE_Msk

#define FLASH_CMD_SE_Pos        8
#define FLASH_CMD_SE_Msk        (0x1UL << FLASH_CMD_SE_Pos)
#define FLASH_CMD_SE            FLASH_CMD_SE_Msk

#define FLASH_CMD_IFREN_Pos     9
#define FLASH_CMD_IFREN_Msk     (0x1UL << FLASH_CMD_IFREN_Pos)
#define FLASH_CMD_IFREN         FLASH_CMD_IFREN_Msk

#define FLASH_CMD_MAS1_Pos      11
#define FLASH_CMD_MAS1_Msk      (0x1UL << FLASH_CMD_MAS1_Pos)
#define FLASH_CMD_MAS1          FLASH_CMD_MAS1_Msk

#define FLASH_CMD_NVSTR_Pos     13
#define FLASH_CMD_NVSTR_Msk     (0x1UL << FLASH_CMD_NVSTR_Pos)
#define FLASH_CMD_NVSTR         FLASH_CMD_NVSTR_Msk

#define FLASH_CMD_TMR_Pos       14
#define FLASH_CMD_TMR_Msk       (0x1UL << FLASH_CMD_TMR_Pos)
#define FLASH_CMD_TMR           FLASH_CMD_TMR_Msk

#endif

#if defined(USE_MDR1206AFI) || defined(USE_MDR1206)

#define FLASH_CMD_CE_Pos    6
#define FLASH_CMD_CE_Msk    (0x1UL << FLASH_CMD_CE_Pos)
#define FLASH_CMD_CE        FLASH_CMD_CE_Msk

#define FLASH_CMD_WE_Pos    7
#define FLASH_CMD_WE_Msk    (0x1UL << FLASH_CMD_WE_Pos)
#define FLASH_CMD_WE        FLASH_CMD_WE_Msk

#define FLASH_CMD_RE_Pos    8
#define FLASH_CMD_RE_Msk    (0x1UL << FLASH_CMD_RE_Pos)
#define FLASH_CMD_RE        FLASH_CMD_RE_Msk

#define FLASH_CMD_NVR_Pos   9
#define FLASH_CMD_NVR_Msk   (0x1UL << FLASH_CMD_NVR_Pos)
#define FLASH_CMD_NVR       FLASH_CMD_NVR_Msk

#define FLASH_CMD_CHIP_Pos  11
#define FLASH_CMD_CHIP_Msk  (0x1UL << FLASH_CMD_CHIP_Pos)
#define FLASH_CMD_CHIP      FLASH_CMD_CHIP_Msk

#define FLASH_CMD_PROG2_Pos 13
#define FLASH_CMD_PROG2_Msk (0x1UL << FLASH_CMD_PROG2_Pos)
#define FLASH_CMD_PROG2     FLASH_CMD_PROG2_Msk

#define FLASH_CMD_TMEN_Pos  14
#define FLASH_CMD_TMEN_Msk  (0x1UL << FLASH_CMD_TMEN_Pos)
#define FLASH_CMD_TMEN      FLASH_CMD_TMEN_Msk

#endif

/** @} */ /* End of group MDR32VF0xI_FLASH_CMD */

/** @defgroup MDR32VF0xI_FLASH_ADR FLASH ADR
 * @{
 */

#if defined(USE_MDR32F02) || defined(USE_MDR1206FI) || defined(USE_MDR1206)

#define FLASH_ADR_YADR_Pos           2
#define FLASH_ADR_YADR_Msk           (0x7FUL << FLASH_ADR_YADR_Pos)

#define FLASH_ADR_XADR_Pos           9

#define FLASH_ADR_XADR_ROW_Pos       FLASH_ADR_XADR_Pos
#define FLASH_ADR_XADR_ROW_Msk       (0x7UL << FLASH_ADR_XADR_ROW_Pos)

#define FLASH_ADR_INFO_XADR_Pos      FLASH_ADR_XADR_Pos
#define FLASH_ADR_INFO_XADR_Msk      (0xFUL << FLASH_ADR_INFO_XADR_Pos)

#define FLASH_ADR_INFO_XADR_PAGE_Pos 12
#define FLASH_ADR_INFO_XADR_PAGE_Msk (0x1UL << FLASH_ADR_INFO_XADR_PAGE_Pos)

#define FLASH_ADR_MAIN_XADR_Pos      FLASH_ADR_XADR_Pos
#define FLASH_ADR_MAIN_XADR_Msk      (0x1FFUL << FLASH_ADR_MAIN_XADR_Pos)

#define FLASH_ADR_MAIN_XADR_PAGE_Pos 12
#define FLASH_ADR_MAIN_XADR_PAGE_Msk (0x3FUL << FLASH_ADR_MAIN_XADR_PAGE_Pos)

#endif

#if defined(USE_MDR1206FI) || defined(USE_MDR1206)

#define FLASH_ADR_INFO_BLOCK_Pos 13
#define FLASH_ADR_INFO_BLOCK_Msk (0x1UL << FLASH_ADR_INFO_BLOCK_Pos)
#define FLASH_ADR_INFO_BLOCK_0   (0x0UL << FLASH_ADR_INFO_BLOCK_Pos)
#define FLASH_ADR_INFO_BLOCK_1   (0x1UL << FLASH_ADR_INFO_BLOCK_Pos)

#define FLASH_ADR_MAIN_BLOCK_Pos 18
#define FLASH_ADR_MAIN_BLOCK_Msk (0x1UL << FLASH_ADR_MAIN_BLOCK_Pos)
#define FLASH_ADR_MAIN_BLOCK_0   (0x0UL << FLASH_ADR_MAIN_BLOCK_Pos)
#define FLASH_ADR_MAIN_BLOCK_1   (0x1UL << FLASH_ADR_MAIN_BLOCK_Pos)

#endif

#if defined(USE_MDR1206AFI) || defined(USE_MDR1206)

#define FLASH_ADR_WORD_Pos        2
#define FLASH_ADR_WORD_Msk        (0x1FUL << FLASH_ADR_WORD_Pos)

#define FLASH_ADR_ROW_Pos         7
#define FLASH_ADR_ROW_Msk         (0x3UL << FLASH_ADR_ROW_Pos)

#define FLASH_ADR_PAGE_Pos        9

#define FLASH_ADR_INFO_PAGE_Pos   FLASH_ADR_PAGE_Pos
#define FLASH_ADR_INFO_PAGE_Msk   (0xFUL << FLASH_ADR_INFO_PAGE_Pos)

#define FLASH_ADR_MAIN_PAGE_Pos   FLASH_ADR_PAGE_Pos
#define FLASH_ADR_MAIN_PAGE_Msk   (0x1FFUL << FLASH_ADR_MAIN_PAGE_Pos)

#define FLASH_ADR_INFO_SECTOR_Pos 13
#define FLASH_ADR_INFO_SECTOR_Msk (0x1UL << FLASH_ADR_INFO_SECTOR_Pos)
#define FLASH_ADR_INFO_SECTOR_A   (0x0UL << FLASH_ADR_INFO_SECTOR_Pos)
#define FLASH_ADR_INFO_SECTOR_B   (0x1UL << FLASH_ADR_INFO_SECTOR_Pos)

#define FLASH_ADR_MAIN_SECTOR_Pos 18
#define FLASH_ADR_MAIN_SECTOR_Msk (0x1UL << FLASH_ADR_MAIN_SECTOR_Pos)
#define FLASH_ADR_MAIN_SECTOR_A   (0x0UL << FLASH_ADR_MAIN_SECTOR_Pos)
#define FLASH_ADR_MAIN_SECTOR_B   (0x1UL << FLASH_ADR_MAIN_SECTOR_Pos)

#endif

/** @} */ /* End of group MDR32VF0xI_FLASH_ADR */

/** @defgroup MDR32VF0xI_FLASH_DI FLASH DI
 * @{
 */

#define FLASH_DI_DATA_Pos 0
#define FLASH_DI_DATA_Msk (0xFFFFFFFFUL << FLASH_DI_DATA_Pos)

/** @} */ /* End of group MDR32VF0xI_FLASH_DI */

/** @defgroup MDR32VF0xI_FLASH_DO FLASH DO
 * @{
 */

#define FLASH_DO_DATA_Pos 0
#define FLASH_DO_DATA_Msk (0xFFFFFFFFUL << FLASH_DO_DATA_Pos)

/** @} */ /* End of group MDR32VF0xI_FLASH_DO */

/** @defgroup MDR32VF0xI_FLASH_KEY FLASH KEY
 * @{
 */

#define FLASH_KEY_KEY_Pos    0
#define FLASH_KEY_KEY_Msk    (0xFFFFFFFFUL << FLASH_KEY_KEY_Pos)
#define FLASH_KEY_KEY_LOCK   (0x00000000UL)
#define FLASH_KEY_KEY_UNLOCK (0x8AAA5551UL)

/** @} */ /* End of group MDR32VF0xI_FLASH_KEY */

#if defined(USE_MDR1206AFI) || defined(USE_MDR1206)

/** @defgroup MDR32VF0xI_FLASH_CTRL FLASH CTRL
 * @{
 */

#define FLASH_CTRL_BYTE_Pos   0
#define FLASH_CTRL_BYTE_Msk   (0xFUL << FLASH_CTRL_BYTE_Pos)
#define FLASH_CTRL_BYTE_0     (0x1UL << FLASH_CTRL_BYTE_Pos)
#define FLASH_CTRL_BYTE_1     (0x2UL << FLASH_CTRL_BYTE_Pos)
#define FLASH_CTRL_BYTE_2     (0x4UL << FLASH_CTRL_BYTE_Pos)
#define FLASH_CTRL_BYTE_3     (0x8UL << FLASH_CTRL_BYTE_Pos)

#define FLASH_CTRL_CONFEN_Pos 4
#define FLASH_CTRL_CONFEN_Msk (0x1UL << FLASH_CTRL_CONFEN_Pos)
#define FLASH_CTRL_CONFEN     FLASH_CTRL_CONFEN_Msk

#define FLASH_CTRL_READM0_Pos 5
#define FLASH_CTRL_READM0_Msk (0x1UL << FLASH_CTRL_READM0_Pos)
#define FLASH_CTRL_READM0     FLASH_CTRL_READM0_Msk

#define FLASH_CTRL_READM1_Pos 6
#define FLASH_CTRL_READM1_Msk (0x1UL << FLASH_CTRL_READM1_Pos)
#define FLASH_CTRL_READM1     FLASH_CTRL_READM1_Msk

#define FLASH_CTRL_READCF_Pos 7
#define FLASH_CTRL_READCF_Msk (0x1UL << FLASH_CTRL_READCF_Pos)
#define FLASH_CTRL_READCF     FLASH_CTRL_READCF_Msk

#define FLASH_CTRL_RCELL_Pos  8
#define FLASH_CTRL_RCELL_Msk  (0x1UL << FLASH_CTRL_RCELL_Pos)
#define FLASH_CTRL_RCELL      FLASH_CTRL_RCELL_Msk

#define FLASH_CTRL_TERS_Pos   9
#define FLASH_CTRL_TERS_Msk   (0x1UL << FLASH_CTRL_TERS_Pos)
#define FLASH_CTRL_TERS       FLASH_CTRL_TERS_Msk

/** @} */ /* End of group MDR32VF0xI_FLASH_CTRL */

#endif

#if defined(USE_MDR1206FI) || defined(USE_MDR1206AFI) || defined(USE_MDR1206)

/** @defgroup MDR32VF0xI_FLASH_CHIP_ID_CTRL FLASH CHIP_ID_CTRL
 * @{
 */

#define FLASH_CHIP_ID_CTRL_OTP_PROG_Pos 0
#define FLASH_CHIP_ID_CTRL_OTP_PROG_Msk (0x1UL << FLASH_CHIP_ID_CTRL_OTP_PROG_Pos)
#define FLASH_CHIP_ID_CTRL_OTP_PROG     FLASH_CHIP_ID_CTRL_OTP_PROG_Msk

#define FLASH_CHIP_ID_CTRL_OTPTST2_Pos  1
#define FLASH_CHIP_ID_CTRL_OTPTST2_Msk  (0x1UL << FLASH_CHIP_ID_CTRL_OTPTST2_Pos)
#define FLASH_CHIP_ID_CTRL_OTPTST2      FLASH_CHIP_ID_CTRL_OTPTST2_Msk

#define FLASH_CHIP_ID_CTRL_CHIP_ID_Pos  2
#define FLASH_CHIP_ID_CTRL_CHIP_ID_Msk  (0xFFUL << FLASH_CHIP_ID_CTRL_CHIP_ID_Pos)
#define FLASH_CHIP_ID_CTRL_CHIP_ID217   (0xD9UL << FLASH_CHIP_ID_CTRL_CHIP_ID_Pos)
#define FLASH_CHIP_ID_CTRL_CHIP_ID215   (0xD7UL << FLASH_CHIP_ID_CTRL_CHIP_ID_Pos)

/** @} */ /* End of group MDR32VF0xI_FLASH_CHIP_ID_CTRL */

/** @defgroup MDR32VF0xI_FLASH_OTP_KEY FLASH OTP_KEY
 * @{
 */

#define FLASH_OTP_KEY_OTP_KEY_Pos    0
#define FLASH_OTP_KEY_OTP_KEY_Msk    (0xFFFFFFFFUL << FLASH_OTP_KEY_OTP_KEY_Pos)
#define FLASH_OTP_KEY_OTP_KEY_LOCK   (0x00000000UL)
#define FLASH_OTP_KEY_OTP_KEY_UNLOCK (0x1555AAA8UL)

/** @} */ /* End of group MDR32VF0xI_FLASH_OTP_KEY */

#endif

/** @} */ /* End of group MDR32VF0xI_FLASH_Defines */

/** @} */ /* End of group MDR32VF0xI_FLASH */

#if defined(USE_MDR1206FI) || defined(USE_MDR1206AFI) || defined(USE_MDR1206)

/** @defgroup MDR32VF0xI_FLASH_SPECIAL_FIELDS MDR_FLASH_SPECIAL_FIELDS
 * @{
 */

/**
 * @brief MDR32VF0xI FLASH_SPECIAL_FIELDS structure.
 */
typedef struct {
    __I uint32_t UNIQUE_ID[5]; /*!<FLASH_SPECIAL_FIELDS Unique Chip Identifier Fields */
    __I uint8_t  TRIM_HSI;     /*!<FLASH_SPECIAL_FIELDS HSI and LSI Trimming Field */
    __I uint8_t  TRIM_BG;      /*!<FLASH_SPECIAL_FIELDS BandGap Reference Voltage Trimming Field */
    __I uint8_t  TRIM_LDO;     /*!<FLASH_SPECIAL_FIELDS LDO Reference Voltage Trimming Field */
    __I uint8_t  TRIM_TS;      /*!<FLASH_SPECIAL_FIELDS TS Voltage Offset Field */
    __I uint8_t  TRIM_CS;      /*!<FLASH_SPECIAL_FIELDS Checksum Field */
    __I uint8_t  RESERVED0[3];
    __I uint32_t PROTECTION; /*!<FLASH_SPECIAL_FIELDS Flash Protection Word Field */
} MDR_FLASH_SpecialFields_TypeDef;

/** @defgroup MDR32VF0xI_FLASH_SPECIAL_FIELDS_Defines FLASH_SPECIAL_FIELDS Defines
 * @{
 */

/** @defgroup MDR32VF0xI_FLASH_SPECIAL_FIELDS_UNIQUE_ID FLASH_SPECIAL_FIELDS UNIQUE_ID
 * @{
 */

#define FLASH_SPECIAL_FIELDS_UNIQUE_IDx_UNIQUE_ID_Pos 0
#define FLASH_SPECIAL_FIELDS_UNIQUE_IDx_UNIQUE_ID_Msk (0xFFFFFFFFUL << FLASH_SPECIAL_FIELDS_UNIQUE_IDx_UNIQUE_ID_Pos)

/** @} */ /* End of group MDR32VF0xI_FLASH_SPECIAL_FIELDS_UNIQUE_ID */

/** @defgroup MDR32VF0xI_FLASH_SPECIAL_FIELDS_TRIM_HSI FLASH_SPECIAL_FIELDS TRIM_HSI
 * @{
 */

#define FLASH_SPECIAL_FIELDS_TRIM_HSI_TRIM_HSI_Pos 0
#define FLASH_SPECIAL_FIELDS_TRIM_HSI_TRIM_HSI_Msk (0xFFUL << FLASH_SPECIAL_FIELDS_TRIM_HSI_TRIM_HSI_Pos)

/** @} */ /* End of group MDR32VF0xI_FLASH_SPECIAL_FIELDS_TRIM_HSI */

/** @defgroup MDR32VF0xI_FLASH_SPECIAL_FIELDS_TRIM_BG FLASH_SPECIAL_FIELDS TRIM_BG
 * @{
 */

#define FLASH_SPECIAL_FIELDS_TRIM_BG_TRIM_BG_Pos 0
#define FLASH_SPECIAL_FIELDS_TRIM_BG_TRIM_BG_Msk (0xFFUL << FLASH_SPECIAL_FIELDS_TRIM_BG_TRIM_BG_Pos)

/** @} */ /* End of group MDR32VF0xI_FLASH_SPECIAL_FIELDS_TRIM_BG */

/** @defgroup MDR32VF0xI_FLASH_SPECIAL_FIELDS_TRIM_LDO FLASH_SPECIAL_FIELDS TRIM_LDO
 * @{
 */

#define FLASH_SPECIAL_FIELDS_TRIM_LDO_TRIM_LDO_Pos 0
#define FLASH_SPECIAL_FIELDS_TRIM_LDO_TRIM_LDO_Msk (0xFFUL << FLASH_SPECIAL_FIELDS_TRIM_LDO_TRIM_LDO_Pos)

/** @} */ /* End of group MDR32VF0xI_FLASH_SPECIAL_FIELDS_TRIM_LDO */

/** @defgroup MDR32VF0xI_FLASH_SPECIAL_FIELDS_TRIM_TS FLASH_SPECIAL_FIELDS TRIM_TS
 * @{
 */

#define FLASH_SPECIAL_FIELDS_TRIM_TS_TRIM_TS_Pos 0
#define FLASH_SPECIAL_FIELDS_TRIM_TS_TRIM_TS_Msk (0xFFUL << FLASH_SPECIAL_FIELDS_TRIM_TS_TRIM_TS_Pos)

/** @} */ /* End of group MDR32VF0xI_FLASH_SPECIAL_FIELDS_TRIM_TS */

/** @defgroup MDR32VF0xI_FLASH_SPECIAL_FIELDS_TRIM_CS FLASH_SPECIAL_FIELDS TRIM_CS
 * @{
 */

#define FLASH_SPECIAL_FIELDS_TRIM_CS_TRIM_CS_Pos 0
#define FLASH_SPECIAL_FIELDS_TRIM_CS_TRIM_CS_Msk (0xFFUL << FLASH_SPECIAL_FIELDS_TRIM_CS_TRIM_CS_Pos)

/** @} */ /* End of group MDR32VF0xI_FLASH_SPECIAL_FIELDS_TRIM_CS */

/** @defgroup MDR32VF0xI_FLASH_SPECIAL_FIELDS_PROTECTION FLASH_SPECIAL_FIELDS PROTECTION
 * @{
 */

#define FLASH_SPECIAL_FIELDS_PROTECTION_REGIONS_Pos 0
#define FLASH_SPECIAL_FIELDS_PROTECTION_REGIONS_Msk (0xFFUL << FLASH_SPECIAL_FIELDS_PROTECTION_REGIONS_Pos)

#define FLASH_SPECIAL_FIELDS_PROTECTION_KEY_Pos     8
#define FLASH_SPECIAL_FIELDS_PROTECTION_KEY_Msk     (0xFFFFFFUL << FLASH_SPECIAL_FIELDS_PROTECTION_KEY_Pos)
#define FLASH_SPECIAL_FIELDS_PROTECTION_KEY_LOCK    (0xFFFFFFUL << FLASH_SPECIAL_FIELDS_PROTECTION_KEY_Pos)
#define FLASH_SPECIAL_FIELDS_PROTECTION_KEY_UNLOCK  (0x1234BCUL << FLASH_SPECIAL_FIELDS_PROTECTION_KEY_Pos)

/** @} */ /* End of group MDR32VF0xI_FLASH_SPECIAL_FIELDS_PROTECTION */

/** @} */ /* End of group MDR32VF0xI_FLASH_SPECIAL_FIELDS_Defines */

/** @} */ /* End of group MDR32VF0xI_FLASH_SPECIAL_FIELDS */

#endif

/** @defgroup MDR32VF0xI_RST_CLK MDR_RST_CLK
 * @{
 */

/**
 * @brief MDR32VF0xI RST_CLK control structure.
 */
typedef struct {
    __I uint32_t  CLOCK_STATUS; /*!<RST_CLK Clock Status Register */
    __IO uint32_t PLL_CONTROL;  /*!<RST_CLK PLL Control Register */
    __IO uint32_t HS_CONTROL;   /*!<RST_CLK HS Control Register */
    __IO uint32_t CPU_CLOCK;    /*!<RST_CLK CPU Clock Register */
    __IO uint32_t PER1_CLOCK;   /*!<RST_CLK PER1 Clock Register */
    __IO uint32_t ADC_CLOCK;    /*!<RST_CLK ADC Clock Register */
    __IO uint32_t RTC_CLOCK;    /*!<RST_CLK RTC Clock Register */
    __IO uint32_t PER2_CLOCK;   /*!<RST_CLK Peripheral Clock Enable Register */
#if defined(USE_MDR32F02_REV_1X)
    __I uint32_t RESERVED0; /*!<Reserved */
#else
    __IO uint32_t DMA_DONE_STICK; /*!<DMA_DONE Fixation Register */
#endif
    __IO uint32_t TIM_CLOCK;  /*!<RST_CLK Timer Clock Register */
    __IO uint32_t UART_CLOCK; /*!<RST_CLK UART Clock Register */
    __IO uint32_t SSP_CLOCK;  /*!<RST_CLK SSP Clock Register */
#if defined(USE_MDR32F02_REV_2) || defined(USE_MDR1206FI) || defined(USE_MDR1206AFI) || defined(USE_MDR1206)
    __IO uint32_t DIV_SYS_TIM; /*!<Machine Timer Prescaler Register */
#endif
} MDR_RST_CLK_TypeDef;

/** @defgroup MDR32VF0xI_RST_CLK_Defines RST_CLK Defines
 * @{
 */

/** @defgroup MDR32VF0xI_RST_CLK_CLOCK_STATUS RST_CLK CLOCK_STATUS
 * @{
 */

#define RST_CLK_CLOCK_STATUS_PLL_CPU_RDY_Pos 1
#define RST_CLK_CLOCK_STATUS_PLL_CPU_RDY_Msk (0x1UL << RST_CLK_CLOCK_STATUS_PLL_CPU_RDY_Pos)
#define RST_CLK_CLOCK_STATUS_PLL_CPU_RDY     RST_CLK_CLOCK_STATUS_PLL_CPU_RDY_Msk

#define RST_CLK_CLOCK_STATUS_HSE_RDY_Pos     2
#define RST_CLK_CLOCK_STATUS_HSE_RDY_Msk     (0x1UL << RST_CLK_CLOCK_STATUS_HSE_RDY_Pos)
#define RST_CLK_CLOCK_STATUS_HSE_RDY         RST_CLK_CLOCK_STATUS_HSE_RDY_Msk

/** @} */ /* End of group MDR32VF0xI_RST_CLK_CLOCK_STATUS */

/** @defgroup MDR32VF0xI_RST_CLK_PLL_CONTROL RST_CLK PLL_CONTROL
 * @{
 */

#define RST_CLK_PLL_CONTROL_PLL_CPU_ON_Pos  2
#define RST_CLK_PLL_CONTROL_PLL_CPU_ON_Msk  (0x1UL << RST_CLK_PLL_CONTROL_PLL_CPU_ON_Pos)
#define RST_CLK_PLL_CONTROL_PLL_CPU_ON      RST_CLK_PLL_CONTROL_PLL_CPU_ON_Msk

#define RST_CLK_PLL_CONTROL_PLL_CPU_SEL_Pos 3
#define RST_CLK_PLL_CONTROL_PLL_CPU_SEL_Msk (0x1UL << RST_CLK_PLL_CONTROL_PLL_CPU_SEL_Pos)
#define RST_CLK_PLL_CONTROL_PLL_CPU_SEL     RST_CLK_PLL_CONTROL_PLL_CPU_SEL_Msk

#define RST_CLK_PLL_CONTROL_PLL_CPU_MUL_Pos 8
#define RST_CLK_PLL_CONTROL_PLL_CPU_MUL_Msk (0x7UL << RST_CLK_PLL_CONTROL_PLL_CPU_MUL_Pos)

/** @} */ /* End of group MDR32VF0xI_RST_CLK_PLL_CONTROL */

/** @defgroup MDR32VF0xI_RST_CLK_HS_CONTROL RST_CLK HS_CONTROL
 * @{
 */

#define RST_CLK_HS_CONTROL_HSE_ON_Pos  0
#define RST_CLK_HS_CONTROL_HSE_ON_Msk  (0x1UL << RST_CLK_HS_CONTROL_HSE_ON_Pos)
#define RST_CLK_HS_CONTROL_HSE_ON      RST_CLK_HS_CONTROL_HSE_ON_Msk

#define RST_CLK_HS_CONTROL_HSE_BYP_Pos 1
#define RST_CLK_HS_CONTROL_HSE_BYP_Msk (0x1UL << RST_CLK_HS_CONTROL_HSE_BYP_Pos)
#define RST_CLK_HS_CONTROL_HSE_BYP     RST_CLK_HS_CONTROL_HSE_BYP_Msk

/** @} */ /* End of group MDR32VF0xI_RST_CLK_HS_CONTROL */

/** @defgroup MDR32VF0xI_RST_CLK_CPU_CLOCK RST_CLK CPU_CLOCK
 * @{
 */

#define RST_CLK_CPU_CLOCK_CPU_C1_SEL_Pos       0
#define RST_CLK_CPU_CLOCK_CPU_C1_SEL_Msk       (0x3UL << RST_CLK_CPU_CLOCK_CPU_C1_SEL_Pos)
#define RST_CLK_CPU_CLOCK_CPU_C1_SEL_HSI       (0x0UL << RST_CLK_CPU_CLOCK_CPU_C1_SEL_Pos)
#define RST_CLK_CPU_CLOCK_CPU_C1_SEL_HSI_DIV_2 (0x1UL << RST_CLK_CPU_CLOCK_CPU_C1_SEL_Pos)
#define RST_CLK_CPU_CLOCK_CPU_C1_SEL_HSE       (0x2UL << RST_CLK_CPU_CLOCK_CPU_C1_SEL_Pos)
#define RST_CLK_CPU_CLOCK_CPU_C1_SEL_HSE_DIV_2 (0x3UL << RST_CLK_CPU_CLOCK_CPU_C1_SEL_Pos)

#define RST_CLK_CPU_CLOCK_CPU_C2_SEL_Pos       2
#define RST_CLK_CPU_CLOCK_CPU_C2_SEL_Msk       (0x1UL << RST_CLK_CPU_CLOCK_CPU_C2_SEL_Pos)
#define RST_CLK_CPU_CLOCK_CPU_C2_SEL           RST_CLK_CPU_CLOCK_CPU_C2_SEL_Msk

#define RST_CLK_CPU_CLOCK_CPU_C3_SEL_Pos       4
#define RST_CLK_CPU_CLOCK_CPU_C3_SEL_Msk       (0xFUL << RST_CLK_CPU_CLOCK_CPU_C3_SEL_Pos)

#define RST_CLK_CPU_CLOCK_HCLK_SEL_Pos         8
#define RST_CLK_CPU_CLOCK_HCLK_SEL_Msk         (0x3UL << RST_CLK_CPU_CLOCK_HCLK_SEL_Pos)
#define RST_CLK_CPU_CLOCK_HCLK_SEL_HSI         (0x0UL << RST_CLK_CPU_CLOCK_HCLK_SEL_Pos)
#define RST_CLK_CPU_CLOCK_HCLK_SEL_CPU_C3      (0x1UL << RST_CLK_CPU_CLOCK_HCLK_SEL_Pos)
#define RST_CLK_CPU_CLOCK_HCLK_SEL_LSE         (0x2UL << RST_CLK_CPU_CLOCK_HCLK_SEL_Pos)
#define RST_CLK_CPU_CLOCK_HCLK_SEL_LSI         (0x3UL << RST_CLK_CPU_CLOCK_HCLK_SEL_Pos)

/** @} */ /* End of group MDR32VF0xI_RST_CLK_CPU_CLOCK */

/** @defgroup MDR32VF0xI_RST_CLK_PER1_CLOCK RST_CLK PER1_CLOCK
 * @{
 */

#define RST_CLK_PER1_CLOCK_PER1_C1_SEL_Pos        0
#define RST_CLK_PER1_CLOCK_PER1_C1_SEL_Msk        (0x3UL << RST_CLK_PER1_CLOCK_PER1_C1_SEL_Pos)
#define RST_CLK_PER1_CLOCK_PER1_C1_SEL_LSI        (0x0UL << RST_CLK_PER1_CLOCK_PER1_C1_SEL_Pos)
#define RST_CLK_PER1_CLOCK_PER1_C1_SEL_LSI_DIV_2  (0x1UL << RST_CLK_PER1_CLOCK_PER1_C1_SEL_Pos)
#define RST_CLK_PER1_CLOCK_PER1_C1_SEL_LSE        (0x2UL << RST_CLK_PER1_CLOCK_PER1_C1_SEL_Pos)
#define RST_CLK_PER1_CLOCK_PER1_C1_SEL_LSE_DIV_2  (0x3UL << RST_CLK_PER1_CLOCK_PER1_C1_SEL_Pos)

#define RST_CLK_PER1_CLOCK_PER1_C2_SEL_Pos        2
#define RST_CLK_PER1_CLOCK_PER1_C2_SEL_Msk        (0x3UL << RST_CLK_PER1_CLOCK_PER1_C2_SEL_Pos)
#define RST_CLK_PER1_CLOCK_PER1_C2_SEL_CPU_C1     (0x0UL << RST_CLK_PER1_CLOCK_PER1_C2_SEL_Pos)
#define RST_CLK_PER1_CLOCK_PER1_C2_SEL_PER1_C1    (0x1UL << RST_CLK_PER1_CLOCK_PER1_C2_SEL_Pos)
#define RST_CLK_PER1_CLOCK_PER1_C2_SEL_PLLCPU     (0x2UL << RST_CLK_PER1_CLOCK_PER1_C2_SEL_Pos)
#define RST_CLK_PER1_CLOCK_PER1_C2_SEL_HSI_C1_CLK (0x3UL << RST_CLK_PER1_CLOCK_PER1_C2_SEL_Pos)

#define RST_CLK_PER1_CLOCK_KEY_RESET_PROG_Pos     4
#define RST_CLK_PER1_CLOCK_KEY_RESET_PROG_Msk     (0x1UL << RST_CLK_PER1_CLOCK_KEY_RESET_PROG_Pos)
#define RST_CLK_PER1_CLOCK_KEY_RESET_PROG         RST_CLK_PER1_CLOCK_KEY_RESET_PROG_Msk

#define RST_CLK_PER1_CLOCK_DMA_EN_Pos             5
#define RST_CLK_PER1_CLOCK_DMA_EN_Msk             (0x1UL << RST_CLK_PER1_CLOCK_DMA_EN_Pos)
#define RST_CLK_PER1_CLOCK_DMA_EN                 RST_CLK_PER1_CLOCK_DMA_EN_Msk

#define RST_CLK_PER1_CLOCK_WDG_C1_SEL_Pos         6
#define RST_CLK_PER1_CLOCK_WDG_C1_SEL_Msk         (0x3UL << RST_CLK_PER1_CLOCK_WDG_C1_SEL_Pos)
#define RST_CLK_PER1_CLOCK_WDG_C1_SEL_LSI         (0x0UL << RST_CLK_PER1_CLOCK_WDG_C1_SEL_Pos)
#define RST_CLK_PER1_CLOCK_WDG_C1_SEL_LSI_DIV_2   (0x1UL << RST_CLK_PER1_CLOCK_WDG_C1_SEL_Pos)
#define RST_CLK_PER1_CLOCK_WDG_C1_SEL_LSE         (0x2UL << RST_CLK_PER1_CLOCK_WDG_C1_SEL_Pos)
#define RST_CLK_PER1_CLOCK_WDG_C1_SEL_LSE_DIV_2   (0x3UL << RST_CLK_PER1_CLOCK_WDG_C1_SEL_Pos)

#define RST_CLK_PER1_CLOCK_WDG_C2_SEL_Pos         8
#define RST_CLK_PER1_CLOCK_WDG_C2_SEL_Msk         (0x3UL << RST_CLK_PER1_CLOCK_WDG_C2_SEL_Pos)
#define RST_CLK_PER1_CLOCK_WDG_C2_SEL_CPU_C1      (0x0UL << RST_CLK_PER1_CLOCK_WDG_C2_SEL_Pos)
#define RST_CLK_PER1_CLOCK_WDG_C2_SEL_PER1_C1     (0x1UL << RST_CLK_PER1_CLOCK_WDG_C2_SEL_Pos)
#define RST_CLK_PER1_CLOCK_WDG_C2_SEL_PLLCPU      (0x2UL << RST_CLK_PER1_CLOCK_WDG_C2_SEL_Pos)
#define RST_CLK_PER1_CLOCK_WDG_C2_SEL_HSI_C1_CLK  (0x3UL << RST_CLK_PER1_CLOCK_WDG_C2_SEL_Pos)

#define RST_CLK_PER1_CLOCK_WDG_BRG_Pos            10
#define RST_CLK_PER1_CLOCK_WDG_BRG_Msk            (0x7UL << RST_CLK_PER1_CLOCK_WDG_BRG_Pos)

#define RST_CLK_PER1_CLOCK_WDG_CLK_EN_Pos         13
#define RST_CLK_PER1_CLOCK_WDG_CLK_EN_Msk         (0x1UL << RST_CLK_PER1_CLOCK_WDG_CLK_EN_Pos)
#define RST_CLK_PER1_CLOCK_WDG_CLK_EN             RST_CLK_PER1_CLOCK_WDG_CLK_EN_Msk

/** @} */ /* End of group MDR32VF0xI_RST_CLK_PER1_CLOCK */

/** @defgroup MDR32VF0xI_RST_CLK_ADC_CLOCK RST_CLK ADC_CLOCK
 * @{
 */

#define RST_CLK_ADC_CLOCK_ADC_C1_SEL_Pos          0
#define RST_CLK_ADC_CLOCK_ADC_C1_SEL_Msk          (0x3UL << RST_CLK_ADC_CLOCK_ADC_C1_SEL_Pos)
#define RST_CLK_ADC_CLOCK_ADC_C1_SEL_CPU_C1       (0x0UL << RST_CLK_ADC_CLOCK_ADC_C1_SEL_Pos)
#define RST_CLK_ADC_CLOCK_ADC_C1_SEL_PER1_C1      (0x1UL << RST_CLK_ADC_CLOCK_ADC_C1_SEL_Pos)
#define RST_CLK_ADC_CLOCK_ADC_C1_SEL_PLLCPU       (0x2UL << RST_CLK_ADC_CLOCK_ADC_C1_SEL_Pos)
#define RST_CLK_ADC_CLOCK_ADC_C1_SEL_HSE_C1_CLK   (0x3UL << RST_CLK_ADC_CLOCK_ADC_C1_SEL_Pos)

#define RST_CLK_ADC_CLOCK_ADCUI_C1_SEL_Pos        2
#define RST_CLK_ADC_CLOCK_ADCUI_C1_SEL_Msk        (0x3UL << RST_CLK_ADC_CLOCK_ADCUI_C1_SEL_Pos)
#define RST_CLK_ADC_CLOCK_ADCUI_C1_SEL_CPU_C1     (0x0UL << RST_CLK_ADC_CLOCK_ADCUI_C1_SEL_Pos)
#define RST_CLK_ADC_CLOCK_ADCUI_C1_SEL_PER1_C1    (0x1UL << RST_CLK_ADC_CLOCK_ADCUI_C1_SEL_Pos)
#define RST_CLK_ADC_CLOCK_ADCUI_C1_SEL_PLLCPU     (0x2UL << RST_CLK_ADC_CLOCK_ADCUI_C1_SEL_Pos)
#define RST_CLK_ADC_CLOCK_ADCUI_C1_SEL_HSE_C1_CLK (0x3UL << RST_CLK_ADC_CLOCK_ADCUI_C1_SEL_Pos)

#define RST_CLK_ADC_CLOCK_ADCUI_C3_SEL_Pos        4
#define RST_CLK_ADC_CLOCK_ADCUI_C3_SEL_Msk        (0xFUL << RST_CLK_ADC_CLOCK_ADCUI_C3_SEL_Pos)

#define RST_CLK_ADC_CLOCK_ADC_C3_SEL_Pos          8
#define RST_CLK_ADC_CLOCK_ADC_C3_SEL_Msk          (0xFUL << RST_CLK_ADC_CLOCK_ADC_C3_SEL_Pos)

#define RST_CLK_ADC_CLOCK_ADCUI_CLK_EN_Pos        12
#define RST_CLK_ADC_CLOCK_ADCUI_CLK_EN_Msk        (0x1UL << RST_CLK_ADC_CLOCK_ADCUI_CLK_EN_Pos)
#define RST_CLK_ADC_CLOCK_ADCUI_CLK_EN            RST_CLK_ADC_CLOCK_ADCUI_CLK_EN_Msk

#define RST_CLK_ADC_CLOCK_ADC_CLK_EN_Pos          13
#define RST_CLK_ADC_CLOCK_ADC_CLK_EN_Msk          (0x1UL << RST_CLK_ADC_CLOCK_ADC_CLK_EN_Pos)
#define RST_CLK_ADC_CLOCK_ADC_CLK_EN              RST_CLK_ADC_CLOCK_ADC_CLK_EN_Msk

/** @} */ /* End of group MDR32VF0xI_RST_CLK_ADC_CLOCK */

/** @defgroup MDR32VF0xI_RST_CLK_RTC_CLOCK RST_CLK RTC_CLOCK
 * @{
 */

#define RST_CLK_RTC_CLOCK_HSE_SEL_Pos    0
#define RST_CLK_RTC_CLOCK_HSE_SEL_Msk    (0xFUL << RST_CLK_RTC_CLOCK_HSE_SEL_Pos)

#define RST_CLK_RTC_CLOCK_HSI_SEL_Pos    4
#define RST_CLK_RTC_CLOCK_HSI_SEL_Msk    (0xFUL << RST_CLK_RTC_CLOCK_HSI_SEL_Pos)

#define RST_CLK_RTC_CLOCK_HSE_RTC_EN_Pos 8
#define RST_CLK_RTC_CLOCK_HSE_RTC_EN_Msk (0x1UL << RST_CLK_RTC_CLOCK_HSE_RTC_EN_Pos)
#define RST_CLK_RTC_CLOCK_HSE_RTC_EN     RST_CLK_RTC_CLOCK_HSE_RTC_EN_Msk

#define RST_CLK_RTC_CLOCK_HSI_RTC_EN_Pos 9
#define RST_CLK_RTC_CLOCK_HSI_RTC_EN_Msk (0x1UL << RST_CLK_RTC_CLOCK_HSI_RTC_EN_Pos)
#define RST_CLK_RTC_CLOCK_HSI_RTC_EN     RST_CLK_RTC_CLOCK_HSI_RTC_EN_Msk

/** @} */ /* End of group MDR32VF0xI_RST_CLK_RTC_CLOCK */

/** @defgroup MDR32VF0xI_RST_CLK_PER2_CLOCK RST_CLK PER2_CLOCK
 * @{
 */

#define RST_CLK_PER2_CLOCK_PCLK_EN_SSP1_Pos    0
#define RST_CLK_PER2_CLOCK_PCLK_EN_SSP1_Msk    (0x1UL << RST_CLK_PER2_CLOCK_PCLK_EN_SSP1_Pos)
#define RST_CLK_PER2_CLOCK_PCLK_EN_SSP1        RST_CLK_PER2_CLOCK_PCLK_EN_SSP1_Msk

#define RST_CLK_PER2_CLOCK_PCLK_EN_UART1_Pos   1
#define RST_CLK_PER2_CLOCK_PCLK_EN_UART1_Msk   (0x1UL << RST_CLK_PER2_CLOCK_PCLK_EN_UART1_Pos)
#define RST_CLK_PER2_CLOCK_PCLK_EN_UART1       RST_CLK_PER2_CLOCK_PCLK_EN_UART1_Msk

#define RST_CLK_PER2_CLOCK_PCLK_EN_UART2_Pos   2
#define RST_CLK_PER2_CLOCK_PCLK_EN_UART2_Msk   (0x1UL << RST_CLK_PER2_CLOCK_PCLK_EN_UART2_Pos)
#define RST_CLK_PER2_CLOCK_PCLK_EN_UART2       RST_CLK_PER2_CLOCK_PCLK_EN_UART2_Msk

#define RST_CLK_PER2_CLOCK_PCLK_EN_FLASH_Pos   3
#define RST_CLK_PER2_CLOCK_PCLK_EN_FLASH_Msk   (0x1UL << RST_CLK_PER2_CLOCK_PCLK_EN_FLASH_Pos)
#define RST_CLK_PER2_CLOCK_PCLK_EN_FLASH       RST_CLK_PER2_CLOCK_PCLK_EN_FLASH_Msk

#define RST_CLK_PER2_CLOCK_PCLK_EN_RST_CLK_Pos 4
#define RST_CLK_PER2_CLOCK_PCLK_EN_RST_CLK_Msk (0x1UL << RST_CLK_PER2_CLOCK_PCLK_EN_RST_CLK_Pos)
#define RST_CLK_PER2_CLOCK_PCLK_EN_RST_CLK     RST_CLK_PER2_CLOCK_PCLK_EN_RST_CLK_Msk

#define RST_CLK_PER2_CLOCK_PCLK_EN_DMA_Pos     5
#define RST_CLK_PER2_CLOCK_PCLK_EN_DMA_Msk     (0x1UL << RST_CLK_PER2_CLOCK_PCLK_EN_DMA_Pos)
#define RST_CLK_PER2_CLOCK_PCLK_EN_DMA         RST_CLK_PER2_CLOCK_PCLK_EN_DMA_Msk

#define RST_CLK_PER2_CLOCK_PCLK_EN_I2C_Pos     6
#define RST_CLK_PER2_CLOCK_PCLK_EN_I2C_Msk     (0x1UL << RST_CLK_PER2_CLOCK_PCLK_EN_I2C_Pos)
#define RST_CLK_PER2_CLOCK_PCLK_EN_I2C         RST_CLK_PER2_CLOCK_PCLK_EN_I2C_Msk

#define RST_CLK_PER2_CLOCK_PCLK_EN_UART3_Pos   7
#define RST_CLK_PER2_CLOCK_PCLK_EN_UART3_Msk   (0x1UL << RST_CLK_PER2_CLOCK_PCLK_EN_UART3_Pos)
#define RST_CLK_PER2_CLOCK_PCLK_EN_UART3       RST_CLK_PER2_CLOCK_PCLK_EN_UART3_Msk

#define RST_CLK_PER2_CLOCK_PCLK_EN_ADC_Pos     8
#define RST_CLK_PER2_CLOCK_PCLK_EN_ADC_Msk     (0x1UL << RST_CLK_PER2_CLOCK_PCLK_EN_ADC_Pos)
#define RST_CLK_PER2_CLOCK_PCLK_EN_ADC         RST_CLK_PER2_CLOCK_PCLK_EN_ADC_Msk

#define RST_CLK_PER2_CLOCK_PCLK_EN_WWDG_Pos    9
#define RST_CLK_PER2_CLOCK_PCLK_EN_WWDG_Msk    (0x1UL << RST_CLK_PER2_CLOCK_PCLK_EN_WWDG_Pos)
#define RST_CLK_PER2_CLOCK_PCLK_EN_WWDG        RST_CLK_PER2_CLOCK_PCLK_EN_WWDG_Msk

#define RST_CLK_PER2_CLOCK_PCLK_EN_IWDG_Pos    10
#define RST_CLK_PER2_CLOCK_PCLK_EN_IWDG_Msk    (0x1UL << RST_CLK_PER2_CLOCK_PCLK_EN_IWDG_Pos)
#define RST_CLK_PER2_CLOCK_PCLK_EN_IWDG        RST_CLK_PER2_CLOCK_PCLK_EN_IWDG_Msk

#define RST_CLK_PER2_CLOCK_PCLK_EN_POWER_Pos   11
#define RST_CLK_PER2_CLOCK_PCLK_EN_POWER_Msk   (0x1UL << RST_CLK_PER2_CLOCK_PCLK_EN_POWER_Pos)
#define RST_CLK_PER2_CLOCK_PCLK_EN_POWER       RST_CLK_PER2_CLOCK_PCLK_EN_POWER_Msk

#define RST_CLK_PER2_CLOCK_PCLK_EN_BKP_Pos     12
#define RST_CLK_PER2_CLOCK_PCLK_EN_BKP_Msk     (0x1UL << RST_CLK_PER2_CLOCK_PCLK_EN_BKP_Pos)
#define RST_CLK_PER2_CLOCK_PCLK_EN_BKP         RST_CLK_PER2_CLOCK_PCLK_EN_BKP_Msk

#define RST_CLK_PER2_CLOCK_PCLK_EN_ADCUI_Pos   13
#define RST_CLK_PER2_CLOCK_PCLK_EN_ADCUI_Msk   (0x1UL << RST_CLK_PER2_CLOCK_PCLK_EN_ADCUI_Pos)
#define RST_CLK_PER2_CLOCK_PCLK_EN_ADCUI       RST_CLK_PER2_CLOCK_PCLK_EN_ADCUI_Msk

#define RST_CLK_PER2_CLOCK_PCLK_EN_TIMER1_Pos  14
#define RST_CLK_PER2_CLOCK_PCLK_EN_TIMER1_Msk  (0x1UL << RST_CLK_PER2_CLOCK_PCLK_EN_TIMER1_Pos)
#define RST_CLK_PER2_CLOCK_PCLK_EN_TIMER1      RST_CLK_PER2_CLOCK_PCLK_EN_TIMER1_Msk

#define RST_CLK_PER2_CLOCK_PCLK_EN_TIMER2_Pos  15
#define RST_CLK_PER2_CLOCK_PCLK_EN_TIMER2_Msk  (0x1UL << RST_CLK_PER2_CLOCK_PCLK_EN_TIMER2_Pos)
#define RST_CLK_PER2_CLOCK_PCLK_EN_TIMER2      RST_CLK_PER2_CLOCK_PCLK_EN_TIMER2_Msk

#define RST_CLK_PER2_CLOCK_PCLK_EN_PORTA_Pos   16
#define RST_CLK_PER2_CLOCK_PCLK_EN_PORTA_Msk   (0x1UL << RST_CLK_PER2_CLOCK_PCLK_EN_PORTA_Pos)
#define RST_CLK_PER2_CLOCK_PCLK_EN_PORTA       RST_CLK_PER2_CLOCK_PCLK_EN_PORTA_Msk

#define RST_CLK_PER2_CLOCK_PCLK_EN_PORTB_Pos   17
#define RST_CLK_PER2_CLOCK_PCLK_EN_PORTB_Msk   (0x1UL << RST_CLK_PER2_CLOCK_PCLK_EN_PORTB_Pos)
#define RST_CLK_PER2_CLOCK_PCLK_EN_PORTB       RST_CLK_PER2_CLOCK_PCLK_EN_PORTB_Msk

#define RST_CLK_PER2_CLOCK_PCLK_EN_PORTC_Pos   18
#define RST_CLK_PER2_CLOCK_PCLK_EN_PORTC_Msk   (0x1UL << RST_CLK_PER2_CLOCK_PCLK_EN_PORTC_Pos)
#define RST_CLK_PER2_CLOCK_PCLK_EN_PORTC       RST_CLK_PER2_CLOCK_PCLK_EN_PORTC_Msk

#define RST_CLK_PER2_CLOCK_PCLK_EN_CRC_Pos     19
#define RST_CLK_PER2_CLOCK_PCLK_EN_CRC_Msk     (0x1UL << RST_CLK_PER2_CLOCK_PCLK_EN_CRC_Pos)
#define RST_CLK_PER2_CLOCK_PCLK_EN_CRC         RST_CLK_PER2_CLOCK_PCLK_EN_CRC_Msk

#if defined(USE_MDR32F02)

#define RST_CLK_PER2_CLOCK_PCLK_EN_SENSORS_Pos     21
#define RST_CLK_PER2_CLOCK_PCLK_EN_SENSORS_Msk     (0x1UL << RST_CLK_PER2_CLOCK_PCLK_EN_SENSORS_Pos)
#define RST_CLK_PER2_CLOCK_PCLK_EN_SENSORS         RST_CLK_PER2_CLOCK_PCLK_EN_SENSORS_Msk

#define RST_CLK_PER2_CLOCK_PCLK_EN_CLR_MEASURE_Pos 22
#define RST_CLK_PER2_CLOCK_PCLK_EN_CLR_MEASURE_Msk (0x1UL << RST_CLK_PER2_CLOCK_PCLK_EN_CLR_MEASURE_Pos)
#define RST_CLK_PER2_CLOCK_PCLK_EN_CLR_MEASURE     RST_CLK_PER2_CLOCK_PCLK_EN_CLR_MEASURE_Msk

#endif

#define RST_CLK_PER2_CLOCK_PCLK_EN_RANDOM_Pos  23
#define RST_CLK_PER2_CLOCK_PCLK_EN_RANDOM_Msk  (0x1UL << RST_CLK_PER2_CLOCK_PCLK_EN_RANDOM_Pos)
#define RST_CLK_PER2_CLOCK_PCLK_EN_RANDOM      RST_CLK_PER2_CLOCK_PCLK_EN_RANDOM_Msk

#define RST_CLK_PER2_CLOCK_PCLK_EN_ISO7816_Pos 24
#define RST_CLK_PER2_CLOCK_PCLK_EN_ISO7816_Msk (0x1UL << RST_CLK_PER2_CLOCK_PCLK_EN_ISO7816_Pos)
#define RST_CLK_PER2_CLOCK_PCLK_EN_ISO7816     RST_CLK_PER2_CLOCK_PCLK_EN_ISO7816_Msk

#define RST_CLK_PER2_CLOCK_PCLK_EN_SSP2_Pos    25
#define RST_CLK_PER2_CLOCK_PCLK_EN_SSP2_Msk    (0x1UL << RST_CLK_PER2_CLOCK_PCLK_EN_SSP2_Pos)
#define RST_CLK_PER2_CLOCK_PCLK_EN_SSP2        RST_CLK_PER2_CLOCK_PCLK_EN_SSP2_Msk

#define RST_CLK_PER2_CLOCK_PCLK_EN_SSP3_Pos    26
#define RST_CLK_PER2_CLOCK_PCLK_EN_SSP3_Msk    (0x1UL << RST_CLK_PER2_CLOCK_PCLK_EN_SSP3_Pos)
#define RST_CLK_PER2_CLOCK_PCLK_EN_SSP3        RST_CLK_PER2_CLOCK_PCLK_EN_SSP3_Msk

#define RST_CLK_PER2_CLOCK_PCLK_EN_TIMER3_Pos  27
#define RST_CLK_PER2_CLOCK_PCLK_EN_TIMER3_Msk  (0x1UL << RST_CLK_PER2_CLOCK_PCLK_EN_TIMER3_Pos)
#define RST_CLK_PER2_CLOCK_PCLK_EN_TIMER3      RST_CLK_PER2_CLOCK_PCLK_EN_TIMER3_Msk

#define RST_CLK_PER2_CLOCK_PCLK_EN_TIMER4_Pos  28
#define RST_CLK_PER2_CLOCK_PCLK_EN_TIMER4_Msk  (0x1UL << RST_CLK_PER2_CLOCK_PCLK_EN_TIMER4_Pos)
#define RST_CLK_PER2_CLOCK_PCLK_EN_TIMER4      RST_CLK_PER2_CLOCK_PCLK_EN_TIMER4_Msk

#define RST_CLK_PER2_CLOCK_PCLK_EN_UART4_Pos   29
#define RST_CLK_PER2_CLOCK_PCLK_EN_UART4_Msk   (0x1UL << RST_CLK_PER2_CLOCK_PCLK_EN_UART4_Pos)
#define RST_CLK_PER2_CLOCK_PCLK_EN_UART4       RST_CLK_PER2_CLOCK_PCLK_EN_UART4_Msk

#define RST_CLK_PER2_CLOCK_PCLK_EN_PORTD_Pos   30
#define RST_CLK_PER2_CLOCK_PCLK_EN_PORTD_Msk   (0x1UL << RST_CLK_PER2_CLOCK_PCLK_EN_PORTD_Pos)
#define RST_CLK_PER2_CLOCK_PCLK_EN_PORTD       RST_CLK_PER2_CLOCK_PCLK_EN_PORTD_Msk

/** @} */ /* End of group MDR32VF0xI_RST_CLK_PER2_CLOCK */

/** @defgroup MDR32VF0xI_RST_CLK_DMA_DONE_STICK RST_CLK DMA_DONE_STICK
 * @{
 */

#define RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK_Pos 0
#define RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK_Msk (0xFFFFFFFFUL << RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK_Pos)
#define RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK0    (0x00000001UL << RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK_Pos)
#define RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK1    (0x00000002UL << RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK_Pos)
#define RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK2    (0x00000004UL << RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK_Pos)
#define RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK3    (0x00000008UL << RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK_Pos)
#define RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK4    (0x00000010UL << RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK_Pos)
#define RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK5    (0x00000020UL << RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK_Pos)
#define RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK6    (0x00000040UL << RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK_Pos)
#define RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK7    (0x00000080UL << RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK_Pos)
#define RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK8    (0x00000100UL << RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK_Pos)
#define RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK9    (0x00000200UL << RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK_Pos)
#define RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK10   (0x00000400UL << RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK_Pos)
#define RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK11   (0x00000800UL << RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK_Pos)
#define RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK12   (0x00001000UL << RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK_Pos)
#define RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK13   (0x00002000UL << RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK_Pos)
#define RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK14   (0x00004000UL << RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK_Pos)
#define RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK15   (0x00008000UL << RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK_Pos)
#define RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK16   (0x00010000UL << RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK_Pos)
#define RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK17   (0x00020000UL << RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK_Pos)
#define RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK18   (0x00040000UL << RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK_Pos)
#define RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK19   (0x00080000UL << RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK_Pos)
#define RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK20   (0x00100000UL << RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK_Pos)
#define RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK21   (0x00200000UL << RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK_Pos)
#define RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK22   (0x00400000UL << RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK_Pos)
#define RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK23   (0x00800000UL << RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK_Pos)
#define RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK24   (0x01000000UL << RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK_Pos)
#define RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK25   (0x02000000UL << RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK_Pos)
#define RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK26   (0x04000000UL << RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK_Pos)
#define RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK27   (0x08000000UL << RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK_Pos)
#define RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK28   (0x10000000UL << RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK_Pos)
#define RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK29   (0x20000000UL << RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK_Pos)
#define RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK30   (0x40000000UL << RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK_Pos)
#define RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK31   (0x80000000UL << RST_CLK_DMA_DONE_STICK_DMA_DONE_STICK_Pos)

/** @} */ /* End of group MDR32VF0xI_RST_CLK_DMA_DONE_STICK */

/** @defgroup MDR32VF0xI_RST_CLK_TIM_CLOCK RST_CLK TIM_CLOCK
 * @{
 */

#define RST_CLK_TIM_CLOCK_TIM1_BRG_Pos     0
#define RST_CLK_TIM_CLOCK_TIM1_BRG_Msk     (0x7UL << RST_CLK_TIM_CLOCK_TIM1_BRG_Pos)
#define RST_CLK_TIM_CLOCK_TIM1_BRG_DIV_1   (0x0UL << RST_CLK_TIM_CLOCK_TIM1_BRG_Pos)
#define RST_CLK_TIM_CLOCK_TIM1_BRG_DIV_2   (0x1UL << RST_CLK_TIM_CLOCK_TIM1_BRG_Pos)
#define RST_CLK_TIM_CLOCK_TIM1_BRG_DIV_4   (0x2UL << RST_CLK_TIM_CLOCK_TIM1_BRG_Pos)
#define RST_CLK_TIM_CLOCK_TIM1_BRG_DIV_8   (0x3UL << RST_CLK_TIM_CLOCK_TIM1_BRG_Pos)
#define RST_CLK_TIM_CLOCK_TIM1_BRG_DIV_16  (0x4UL << RST_CLK_TIM_CLOCK_TIM1_BRG_Pos)
#define RST_CLK_TIM_CLOCK_TIM1_BRG_DIV_32  (0x5UL << RST_CLK_TIM_CLOCK_TIM1_BRG_Pos)
#define RST_CLK_TIM_CLOCK_TIM1_BRG_DIV_64  (0x6UL << RST_CLK_TIM_CLOCK_TIM1_BRG_Pos)
#define RST_CLK_TIM_CLOCK_TIM1_BRG_DIV_128 (0x7UL << RST_CLK_TIM_CLOCK_TIM1_BRG_Pos)

#define RST_CLK_TIM_CLOCK_TIM2_BRG_Pos     8
#define RST_CLK_TIM_CLOCK_TIM2_BRG_Msk     (0x7UL << RST_CLK_TIM_CLOCK_TIM2_BRG_Pos)
#define RST_CLK_TIM_CLOCK_TIM2_BRG_DIV_1   (0x0UL << RST_CLK_TIM_CLOCK_TIM2_BRG_Pos)
#define RST_CLK_TIM_CLOCK_TIM2_BRG_DIV_2   (0x1UL << RST_CLK_TIM_CLOCK_TIM2_BRG_Pos)
#define RST_CLK_TIM_CLOCK_TIM2_BRG_DIV_4   (0x2UL << RST_CLK_TIM_CLOCK_TIM2_BRG_Pos)
#define RST_CLK_TIM_CLOCK_TIM2_BRG_DIV_8   (0x3UL << RST_CLK_TIM_CLOCK_TIM2_BRG_Pos)
#define RST_CLK_TIM_CLOCK_TIM2_BRG_DIV_16  (0x4UL << RST_CLK_TIM_CLOCK_TIM2_BRG_Pos)
#define RST_CLK_TIM_CLOCK_TIM2_BRG_DIV_32  (0x5UL << RST_CLK_TIM_CLOCK_TIM2_BRG_Pos)
#define RST_CLK_TIM_CLOCK_TIM2_BRG_DIV_64  (0x6UL << RST_CLK_TIM_CLOCK_TIM2_BRG_Pos)
#define RST_CLK_TIM_CLOCK_TIM2_BRG_DIV_128 (0x7UL << RST_CLK_TIM_CLOCK_TIM2_BRG_Pos)

#define RST_CLK_TIM_CLOCK_TIM3_BRG_Pos     16
#define RST_CLK_TIM_CLOCK_TIM3_BRG_Msk     (0x7UL << RST_CLK_TIM_CLOCK_TIM3_BRG_Pos)
#define RST_CLK_TIM_CLOCK_TIM3_BRG_DIV_1   (0x0UL << RST_CLK_TIM_CLOCK_TIM3_BRG_Pos)
#define RST_CLK_TIM_CLOCK_TIM3_BRG_DIV_2   (0x1UL << RST_CLK_TIM_CLOCK_TIM3_BRG_Pos)
#define RST_CLK_TIM_CLOCK_TIM3_BRG_DIV_4   (0x2UL << RST_CLK_TIM_CLOCK_TIM3_BRG_Pos)
#define RST_CLK_TIM_CLOCK_TIM3_BRG_DIV_8   (0x3UL << RST_CLK_TIM_CLOCK_TIM3_BRG_Pos)
#define RST_CLK_TIM_CLOCK_TIM3_BRG_DIV_16  (0x4UL << RST_CLK_TIM_CLOCK_TIM3_BRG_Pos)
#define RST_CLK_TIM_CLOCK_TIM3_BRG_DIV_32  (0x5UL << RST_CLK_TIM_CLOCK_TIM3_BRG_Pos)
#define RST_CLK_TIM_CLOCK_TIM3_BRG_DIV_64  (0x6UL << RST_CLK_TIM_CLOCK_TIM3_BRG_Pos)
#define RST_CLK_TIM_CLOCK_TIM3_BRG_DIV_128 (0x7UL << RST_CLK_TIM_CLOCK_TIM3_BRG_Pos)

#define RST_CLK_TIM_CLOCK_TIM4_BRG_Pos     19
#define RST_CLK_TIM_CLOCK_TIM4_BRG_Msk     (0x7UL << RST_CLK_TIM_CLOCK_TIM4_BRG_Pos)
#define RST_CLK_TIM_CLOCK_TIM4_BRG_DIV_1   (0x0UL << RST_CLK_TIM_CLOCK_TIM4_BRG_Pos)
#define RST_CLK_TIM_CLOCK_TIM4_BRG_DIV_2   (0x1UL << RST_CLK_TIM_CLOCK_TIM4_BRG_Pos)
#define RST_CLK_TIM_CLOCK_TIM4_BRG_DIV_4   (0x2UL << RST_CLK_TIM_CLOCK_TIM4_BRG_Pos)
#define RST_CLK_TIM_CLOCK_TIM4_BRG_DIV_8   (0x3UL << RST_CLK_TIM_CLOCK_TIM4_BRG_Pos)
#define RST_CLK_TIM_CLOCK_TIM4_BRG_DIV_16  (0x4UL << RST_CLK_TIM_CLOCK_TIM4_BRG_Pos)
#define RST_CLK_TIM_CLOCK_TIM4_BRG_DIV_32  (0x5UL << RST_CLK_TIM_CLOCK_TIM4_BRG_Pos)
#define RST_CLK_TIM_CLOCK_TIM4_BRG_DIV_64  (0x6UL << RST_CLK_TIM_CLOCK_TIM4_BRG_Pos)
#define RST_CLK_TIM_CLOCK_TIM4_BRG_DIV_128 (0x7UL << RST_CLK_TIM_CLOCK_TIM4_BRG_Pos)

#define RST_CLK_TIM_CLOCK_TIM1_CLK_EN_Pos  24
#define RST_CLK_TIM_CLOCK_TIM1_CLK_EN_Msk  (0x1UL << RST_CLK_TIM_CLOCK_TIM1_CLK_EN_Pos)
#define RST_CLK_TIM_CLOCK_TIM1_CLK_EN      RST_CLK_TIM_CLOCK_TIM1_CLK_EN_Msk

#define RST_CLK_TIM_CLOCK_TIM2_CLK_EN_Pos  25
#define RST_CLK_TIM_CLOCK_TIM2_CLK_EN_Msk  (0x1UL << RST_CLK_TIM_CLOCK_TIM2_CLK_EN_Pos)
#define RST_CLK_TIM_CLOCK_TIM2_CLK_EN      RST_CLK_TIM_CLOCK_TIM2_CLK_EN_Msk

#define RST_CLK_TIM_CLOCK_TIM3_CLK_EN_Pos  26
#define RST_CLK_TIM_CLOCK_TIM3_CLK_EN_Msk  (0x1UL << RST_CLK_TIM_CLOCK_TIM3_CLK_EN_Pos)
#define RST_CLK_TIM_CLOCK_TIM3_CLK_EN      RST_CLK_TIM_CLOCK_TIM3_CLK_EN_Msk

#define RST_CLK_TIM_CLOCK_TIM4_CLK_EN_Pos  27
#define RST_CLK_TIM_CLOCK_TIM4_CLK_EN_Msk  (0x1UL << RST_CLK_TIM_CLOCK_TIM4_CLK_EN_Pos)
#define RST_CLK_TIM_CLOCK_TIM4_CLK_EN      RST_CLK_TIM_CLOCK_TIM4_CLK_EN_Msk

/** @} */ /* End of group MDR32VF0xI_RST_CLK_TIM_CLOCK */

/** @defgroup MDR32VF0xI_RST_CLK_UART_CLOCK RST_CLK UART_CLOCK
 * @{
 */

#define RST_CLK_UART_CLOCK_UART1_BRG_Pos     0
#define RST_CLK_UART_CLOCK_UART1_BRG_Msk     (0x7UL << RST_CLK_UART_CLOCK_UART1_BRG_Pos)
#define RST_CLK_UART_CLOCK_UART1_BRG_DIV_1   (0x0UL << RST_CLK_UART_CLOCK_UART1_BRG_Pos)
#define RST_CLK_UART_CLOCK_UART1_BRG_DIV_2   (0x1UL << RST_CLK_UART_CLOCK_UART1_BRG_Pos)
#define RST_CLK_UART_CLOCK_UART1_BRG_DIV_4   (0x2UL << RST_CLK_UART_CLOCK_UART1_BRG_Pos)
#define RST_CLK_UART_CLOCK_UART1_BRG_DIV_8   (0x3UL << RST_CLK_UART_CLOCK_UART1_BRG_Pos)
#define RST_CLK_UART_CLOCK_UART1_BRG_DIV_16  (0x4UL << RST_CLK_UART_CLOCK_UART1_BRG_Pos)
#define RST_CLK_UART_CLOCK_UART1_BRG_DIV_32  (0x5UL << RST_CLK_UART_CLOCK_UART1_BRG_Pos)
#define RST_CLK_UART_CLOCK_UART1_BRG_DIV_64  (0x6UL << RST_CLK_UART_CLOCK_UART1_BRG_Pos)
#define RST_CLK_UART_CLOCK_UART1_BRG_DIV_128 (0x7UL << RST_CLK_UART_CLOCK_UART1_BRG_Pos)

#define RST_CLK_UART_CLOCK_UART2_BRG_Pos     8
#define RST_CLK_UART_CLOCK_UART2_BRG_Msk     (0x7UL << RST_CLK_UART_CLOCK_UART2_BRG_Pos)
#define RST_CLK_UART_CLOCK_UART2_BRG_DIV_1   (0x0UL << RST_CLK_UART_CLOCK_UART2_BRG_Pos)
#define RST_CLK_UART_CLOCK_UART2_BRG_DIV_2   (0x1UL << RST_CLK_UART_CLOCK_UART2_BRG_Pos)
#define RST_CLK_UART_CLOCK_UART2_BRG_DIV_4   (0x2UL << RST_CLK_UART_CLOCK_UART2_BRG_Pos)
#define RST_CLK_UART_CLOCK_UART2_BRG_DIV_8   (0x3UL << RST_CLK_UART_CLOCK_UART2_BRG_Pos)
#define RST_CLK_UART_CLOCK_UART2_BRG_DIV_16  (0x4UL << RST_CLK_UART_CLOCK_UART2_BRG_Pos)
#define RST_CLK_UART_CLOCK_UART2_BRG_DIV_32  (0x5UL << RST_CLK_UART_CLOCK_UART2_BRG_Pos)
#define RST_CLK_UART_CLOCK_UART2_BRG_DIV_64  (0x6UL << RST_CLK_UART_CLOCK_UART2_BRG_Pos)
#define RST_CLK_UART_CLOCK_UART2_BRG_DIV_128 (0x7UL << RST_CLK_UART_CLOCK_UART2_BRG_Pos)

#define RST_CLK_UART_CLOCK_UART3_BRG_Pos     16
#define RST_CLK_UART_CLOCK_UART3_BRG_Msk     (0x7UL << RST_CLK_UART_CLOCK_UART3_BRG_Pos)
#define RST_CLK_UART_CLOCK_UART3_BRG_DIV_1   (0x0UL << RST_CLK_UART_CLOCK_UART3_BRG_Pos)
#define RST_CLK_UART_CLOCK_UART3_BRG_DIV_2   (0x1UL << RST_CLK_UART_CLOCK_UART3_BRG_Pos)
#define RST_CLK_UART_CLOCK_UART3_BRG_DIV_4   (0x2UL << RST_CLK_UART_CLOCK_UART3_BRG_Pos)
#define RST_CLK_UART_CLOCK_UART3_BRG_DIV_8   (0x3UL << RST_CLK_UART_CLOCK_UART3_BRG_Pos)
#define RST_CLK_UART_CLOCK_UART3_BRG_DIV_16  (0x4UL << RST_CLK_UART_CLOCK_UART3_BRG_Pos)
#define RST_CLK_UART_CLOCK_UART3_BRG_DIV_32  (0x5UL << RST_CLK_UART_CLOCK_UART3_BRG_Pos)
#define RST_CLK_UART_CLOCK_UART3_BRG_DIV_64  (0x6UL << RST_CLK_UART_CLOCK_UART3_BRG_Pos)
#define RST_CLK_UART_CLOCK_UART3_BRG_DIV_128 (0x7UL << RST_CLK_UART_CLOCK_UART3_BRG_Pos)

#define RST_CLK_UART_CLOCK_UART1_CLK_EN_Pos  24
#define RST_CLK_UART_CLOCK_UART1_CLK_EN_Msk  (0x1UL << RST_CLK_UART_CLOCK_UART1_CLK_EN_Pos)
#define RST_CLK_UART_CLOCK_UART1_CLK_EN      RST_CLK_UART_CLOCK_UART1_CLK_EN_Msk

#define RST_CLK_UART_CLOCK_UART2_CLK_EN_Pos  25
#define RST_CLK_UART_CLOCK_UART2_CLK_EN_Msk  (0x1UL << RST_CLK_UART_CLOCK_UART2_CLK_EN_Pos)
#define RST_CLK_UART_CLOCK_UART2_CLK_EN      RST_CLK_UART_CLOCK_UART2_CLK_EN_Msk

#define RST_CLK_UART_CLOCK_UART3_CLK_EN_Pos  26
#define RST_CLK_UART_CLOCK_UART3_CLK_EN_Msk  (0x1UL << RST_CLK_UART_CLOCK_UART3_CLK_EN_Pos)
#define RST_CLK_UART_CLOCK_UART3_CLK_EN      RST_CLK_UART_CLOCK_UART3_CLK_EN_Msk

#define RST_CLK_UART_CLOCK_UART4_BRG_Pos     27
#define RST_CLK_UART_CLOCK_UART4_BRG_Msk     (0x7UL << RST_CLK_UART_CLOCK_UART4_BRG_Pos)
#define RST_CLK_UART_CLOCK_UART4_BRG_DIV_1   (0x0UL << RST_CLK_UART_CLOCK_UART4_BRG_Pos)
#define RST_CLK_UART_CLOCK_UART4_BRG_DIV_2   (0x1UL << RST_CLK_UART_CLOCK_UART4_BRG_Pos)
#define RST_CLK_UART_CLOCK_UART4_BRG_DIV_4   (0x2UL << RST_CLK_UART_CLOCK_UART4_BRG_Pos)
#define RST_CLK_UART_CLOCK_UART4_BRG_DIV_8   (0x3UL << RST_CLK_UART_CLOCK_UART4_BRG_Pos)
#define RST_CLK_UART_CLOCK_UART4_BRG_DIV_16  (0x4UL << RST_CLK_UART_CLOCK_UART4_BRG_Pos)
#define RST_CLK_UART_CLOCK_UART4_BRG_DIV_32  (0x5UL << RST_CLK_UART_CLOCK_UART4_BRG_Pos)
#define RST_CLK_UART_CLOCK_UART4_BRG_DIV_64  (0x6UL << RST_CLK_UART_CLOCK_UART4_BRG_Pos)
#define RST_CLK_UART_CLOCK_UART4_BRG_DIV_128 (0x7UL << RST_CLK_UART_CLOCK_UART4_BRG_Pos)

#define RST_CLK_UART_CLOCK_UART4_CLK_EN_Pos  30
#define RST_CLK_UART_CLOCK_UART4_CLK_EN_Msk  (0x1UL << RST_CLK_UART_CLOCK_UART4_CLK_EN_Pos)
#define RST_CLK_UART_CLOCK_UART4_CLK_EN      RST_CLK_UART_CLOCK_UART4_CLK_EN_Msk

/** @} */ /* End of group MDR32VF0xI_RST_CLK_UART_CLOCK */

/** @defgroup MDR32VF0xI_RST_CLK_SSP_CLOCK RST_CLK SSP_CLOCK
 * @{
 */

#define RST_CLK_SSP_CLOCK_SSP1_BRG_Pos     0
#define RST_CLK_SSP_CLOCK_SSP1_BRG_Msk     (0x7UL << RST_CLK_SSP_CLOCK_SSP1_BRG_Pos)
#define RST_CLK_SSP_CLOCK_SSP1_BRG_DIV_1   (0x0UL << RST_CLK_SSP_CLOCK_SSP1_BRG_Pos)
#define RST_CLK_SSP_CLOCK_SSP1_BRG_DIV_2   (0x1UL << RST_CLK_SSP_CLOCK_SSP1_BRG_Pos)
#define RST_CLK_SSP_CLOCK_SSP1_BRG_DIV_4   (0x2UL << RST_CLK_SSP_CLOCK_SSP1_BRG_Pos)
#define RST_CLK_SSP_CLOCK_SSP1_BRG_DIV_8   (0x3UL << RST_CLK_SSP_CLOCK_SSP1_BRG_Pos)
#define RST_CLK_SSP_CLOCK_SSP1_BRG_DIV_16  (0x4UL << RST_CLK_SSP_CLOCK_SSP1_BRG_Pos)
#define RST_CLK_SSP_CLOCK_SSP1_BRG_DIV_32  (0x5UL << RST_CLK_SSP_CLOCK_SSP1_BRG_Pos)
#define RST_CLK_SSP_CLOCK_SSP1_BRG_DIV_64  (0x6UL << RST_CLK_SSP_CLOCK_SSP1_BRG_Pos)
#define RST_CLK_SSP_CLOCK_SSP1_BRG_DIV_128 (0x7UL << RST_CLK_SSP_CLOCK_SSP1_BRG_Pos)

#define RST_CLK_SSP_CLOCK_SSP2_BRG_Pos     8
#define RST_CLK_SSP_CLOCK_SSP2_BRG_Msk     (0x7UL << RST_CLK_SSP_CLOCK_SSP2_BRG_Pos)
#define RST_CLK_SSP_CLOCK_SSP2_BRG_DIV_1   (0x0UL << RST_CLK_SSP_CLOCK_SSP2_BRG_Pos)
#define RST_CLK_SSP_CLOCK_SSP2_BRG_DIV_2   (0x1UL << RST_CLK_SSP_CLOCK_SSP2_BRG_Pos)
#define RST_CLK_SSP_CLOCK_SSP2_BRG_DIV_4   (0x2UL << RST_CLK_SSP_CLOCK_SSP2_BRG_Pos)
#define RST_CLK_SSP_CLOCK_SSP2_BRG_DIV_8   (0x3UL << RST_CLK_SSP_CLOCK_SSP2_BRG_Pos)
#define RST_CLK_SSP_CLOCK_SSP2_BRG_DIV_16  (0x4UL << RST_CLK_SSP_CLOCK_SSP2_BRG_Pos)
#define RST_CLK_SSP_CLOCK_SSP2_BRG_DIV_32  (0x5UL << RST_CLK_SSP_CLOCK_SSP2_BRG_Pos)
#define RST_CLK_SSP_CLOCK_SSP2_BRG_DIV_64  (0x6UL << RST_CLK_SSP_CLOCK_SSP2_BRG_Pos)
#define RST_CLK_SSP_CLOCK_SSP2_BRG_DIV_128 (0x7UL << RST_CLK_SSP_CLOCK_SSP2_BRG_Pos)

#define RST_CLK_SSP_CLOCK_SSP3_BRG_Pos     16
#define RST_CLK_SSP_CLOCK_SSP3_BRG_Msk     (0x7UL << RST_CLK_SSP_CLOCK_SSP3_BRG_Pos)
#define RST_CLK_SSP_CLOCK_SSP3_BRG_DIV_1   (0x0UL << RST_CLK_SSP_CLOCK_SSP3_BRG_Pos)
#define RST_CLK_SSP_CLOCK_SSP3_BRG_DIV_2   (0x1UL << RST_CLK_SSP_CLOCK_SSP3_BRG_Pos)
#define RST_CLK_SSP_CLOCK_SSP3_BRG_DIV_4   (0x2UL << RST_CLK_SSP_CLOCK_SSP3_BRG_Pos)
#define RST_CLK_SSP_CLOCK_SSP3_BRG_DIV_8   (0x3UL << RST_CLK_SSP_CLOCK_SSP3_BRG_Pos)
#define RST_CLK_SSP_CLOCK_SSP3_BRG_DIV_16  (0x4UL << RST_CLK_SSP_CLOCK_SSP3_BRG_Pos)
#define RST_CLK_SSP_CLOCK_SSP3_BRG_DIV_32  (0x5UL << RST_CLK_SSP_CLOCK_SSP3_BRG_Pos)
#define RST_CLK_SSP_CLOCK_SSP3_BRG_DIV_64  (0x6UL << RST_CLK_SSP_CLOCK_SSP3_BRG_Pos)
#define RST_CLK_SSP_CLOCK_SSP3_BRG_DIV_128 (0x7UL << RST_CLK_SSP_CLOCK_SSP3_BRG_Pos)

#define RST_CLK_SSP_CLOCK_SSP1_CLK_EN_Pos  24
#define RST_CLK_SSP_CLOCK_SSP1_CLK_EN_Msk  (0x1UL << RST_CLK_SSP_CLOCK_SSP1_CLK_EN_Pos)
#define RST_CLK_SSP_CLOCK_SSP1_CLK_EN      RST_CLK_SSP_CLOCK_SSP1_CLK_EN_Msk

#define RST_CLK_SSP_CLOCK_SSP2_CLK_EN_Pos  25
#define RST_CLK_SSP_CLOCK_SSP2_CLK_EN_Msk  (0x1UL << RST_CLK_SSP_CLOCK_SSP2_CLK_EN_Pos)
#define RST_CLK_SSP_CLOCK_SSP2_CLK_EN      RST_CLK_SSP_CLOCK_SSP2_CLK_EN_Msk

#define RST_CLK_SSP_CLOCK_SSP3_CLK_EN_Pos  26
#define RST_CLK_SSP_CLOCK_SSP3_CLK_EN_Msk  (0x1UL << RST_CLK_SSP_CLOCK_SSP3_CLK_EN_Pos)
#define RST_CLK_SSP_CLOCK_SSP3_CLK_EN      RST_CLK_SSP_CLOCK_SSP3_CLK_EN_Msk

/** @} */ /* End of group MDR32VF0xI_RST_CLK_SSP_CLOCK */

#if !defined(USE_MDR32F02_REV_1X)

/** @defgroup MDR32VF0xI_RST_CLK_DIV_SYS_TIM RST_CLK DIV_SYS_TIM
 * @{
 */

#define RST_CLK_DIV_SYS_TIM_DIV_SYS_TIM_Pos 0
#define RST_CLK_DIV_SYS_TIM_DIV_SYS_TIM_Msk (0xFFUL << RST_CLK_DIV_SYS_TIM_DIV_SYS_TIM_Pos)

/** @} */ /* End of group MDR32VF0xI_RST_CLK_DIV_SYS_TIM */

#endif

/** @} */ /* End of group MDR32VF0xI_RST_CLK_Defines */

/** @} */ /* End of group MDR32VF0xI_RST_CLK */

/** @defgroup MDR32VF0xI_DMA MDR_DMA
 * @{
 */

/**
 * @brief MDR32VF0xI DMA control structure.
 */
typedef struct {
    __I uint32_t  STATUS;            /*!<DMA Status Register */
    __O uint32_t  CFG;               /*!<DMA Configuration Register */
    __IO uint32_t CTRL_BASE_PTR;     /*!<DMA Channel Control Data Base Pointer Register */
    __I uint32_t  ALT_CTRL_BASE_PTR; /*!<DMA Channel Alternate Control Data Base Pointer Register */
    __I uint32_t  WAITONREQ_STATUS;  /*!<DMA Channel Wait on Request Status Register */
    __O uint32_t  CHNL_SW_REQUEST;   /*!<DMA Channel Software Request Register */
    __IO uint32_t CHNL_USEBURST_SET; /*!<DMA Channel Use Burst Set Register */
    __O uint32_t  CHNL_USEBURST_CLR; /*!<DMA Channel Use Burst Clear Register */
    __IO uint32_t CHNL_REQ_MASK_SET; /*!<DMA Channel Request Mask Set Register */
    __O uint32_t  CHNL_REQ_MASK_CLR; /*!<DMA Channel Request Mask Clear Register */
    __IO uint32_t CHNL_ENABLE_SET;   /*!<DMA Channel Enable Set Register */
    __O uint32_t  CHNL_ENABLE_CLR;   /*!<DMA Channel Enable Clear Register */
    __IO uint32_t CHNL_PRI_ALT_SET;  /*!<DMA Channel Primary-Alternate set Register */
    __O uint32_t  CHNL_PRI_ALT_CLR;  /*!<DMA Channel Primary-Alternate clear Register */
    __IO uint32_t CHNL_PRIORITY_SET; /*!<DMA Channel Priority Set Register */
    __O uint32_t  CHNL_PRIORITY_CLR; /*!<DMA Channel Priority Clear Register */
    __I uint32_t  RESERVED0[3];      /*!<Reserved */
    __IO uint32_t ERR_CLR;           /*!<DMA Bus Error Flag Clear Register */
} MDR_DMA_TypeDef;

/** @defgroup MDR32VF0xI_DMA_Defines DMA Defines
 * @{
 */

/** @defgroup MDR32VF0xI_DMA_STATUS DMA STATUS
 * @{
 */

#define DMA_STATUS_MASTER_ENABLE_Pos 0
#define DMA_STATUS_MASTER_ENABLE_Msk (0x1UL << DMA_STATUS_MASTER_ENABLE_Pos)
#define DMA_STATUS_MASTER_ENABLE     DMA_STATUS_MASTER_ENABLE_Msk

#define DMA_STATUS_STATE_Pos         4
#define DMA_STATUS_STATE_Msk         (0xFUL << DMA_STATUS_STATE_Pos)

#define DMA_STATUS_CHNLS_MINUS1_Pos  16
#define DMA_STATUS_CHNLS_MINUS1_Msk  (0x1FUL << DMA_STATUS_CHNLS_MINUS1_Pos)

#define DMA_STATUS_TEST_STATUS_Pos   28
#define DMA_STATUS_TEST_STATUS_Msk   (0xFUL << DMA_STATUS_TEST_STATUS_Pos)

/** @} */ /* End of group MDR32VF0xI_DMA_STATUS */

/** @defgroup MDR32VF0xI_DMA_CFG DMA CFG
 * @{
 */

#define DMA_CFG_MASTER_ENABLE_Pos     0
#define DMA_CFG_MASTER_ENABLE_Msk     (0x1UL << DMA_CFG_MASTER_ENABLE_Pos)
#define DMA_CFG_MASTER_ENABLE         DMA_CFG_MASTER_ENABLE_Msk

#define DMA_CFG_CHNL_PROT_CTRL_Pos    5
#define DMA_CFG_CHNL_PROT_CTRL_Msk    (0x7UL << DMA_CFG_CHNL_PROT_CTRL_Pos)
#define DMA_CFG_CHNL_PROT_CTRL_HPROT1 (0x1UL << DMA_CFG_CHNL_PROT_CTRL_Pos)
#define DMA_CFG_CHNL_PROT_CTRL_HPROT2 (0x2UL << DMA_CFG_CHNL_PROT_CTRL_Pos)
#define DMA_CFG_CHNL_PROT_CTRL_HPROT3 (0x4UL << DMA_CFG_CHNL_PROT_CTRL_Pos)

/** @} */ /* End of group MDR32VF0xI_DMA_CFG */

/** @defgroup MDR32VF0xI_DMA_CTRL_BASE_PTR DMA CTRL_BASE_PTR
 * @{
 */

#define DMA_CTRL_BASE_PTR_CTRL_BASE_PTR_Pos 0
#define DMA_CTRL_BASE_PTR_CTRL_BASE_PTR_Msk (0x1UL << DMA_CTRL_BASE_PTR_CTRL_BASE_PTR_Pos)
#define DMA_CTRL_BASE_PTR_CTRL_BASE_PTR     DMA_CTRL_BASE_PTR_CTRL_BASE_PTR_Msk

/** @} */ /* End of group MDR32VF0xI_DMA_CTRL_BASE_PTR */

/** @defgroup MDR32VF0xI_DMA_ERR_CLR DMA ERR_CLR
 * @{
 */

#define DMA_ERR_CLR_ERR_CLR_Pos 0
#define DMA_ERR_CLR_ERR_CLR_Msk (0x1UL << DMA_ERR_CLR_ERR_CLR_Pos)
#define DMA_ERR_CLR_ERR_CLR     DMA_ERR_CLR_ERR_CLR_Msk

/** @} */ /* End of group MDR32VF0xI_DMA_ERR_CLR */

/** @defgroup MDR32VF0xI_DMA_CHANNEL_CFG DMA CHANNEL_CFG
 * @{
 */

#define DMA_CHANNEL_CFG_CYCLE_CTRL_Pos             0
#define DMA_CHANNEL_CFG_CYCLE_CTRL_Msk             (0x7UL << DMA_CHANNEL_CFG_CYCLE_CTRL_Pos)
#define DMA_CHANNEL_CFG_CYCLE_CTRL_STOP            (0x0UL << DMA_CHANNEL_CFG_CYCLE_CTRL_Pos)
#define DMA_CHANNEL_CFG_CYCLE_CTRL_BASIC           (0x1UL << DMA_CHANNEL_CFG_CYCLE_CTRL_Pos)
#define DMA_CHANNEL_CFG_CYCLE_CTRL_AUTO_REQUEST    (0x2UL << DMA_CHANNEL_CFG_CYCLE_CTRL_Pos)
#define DMA_CHANNEL_CFG_CYCLE_CTRL_PING_PONG       (0x3UL << DMA_CHANNEL_CFG_CYCLE_CTRL_Pos)
#define DMA_CHANNEL_CFG_CYCLE_CTRL_MEM_SCATTER_PRI (0x4UL << DMA_CHANNEL_CFG_CYCLE_CTRL_Pos)
#define DMA_CHANNEL_CFG_CYCLE_CTRL_MEM_SCATTER_ALT (0x5UL << DMA_CHANNEL_CFG_CYCLE_CTRL_Pos)
#define DMA_CHANNEL_CFG_CYCLE_CTRL_PER_SCATTER_PRI (0x6UL << DMA_CHANNEL_CFG_CYCLE_CTRL_Pos)
#define DMA_CHANNEL_CFG_CYCLE_CTRL_PER_SCATTER_ALT (0x7UL << DMA_CHANNEL_CFG_CYCLE_CTRL_Pos)

#define DMA_CHANNEL_CFG_NEXT_USEBURST_Pos          3
#define DMA_CHANNEL_CFG_NEXT_USEBURST_Msk          (0x1UL << DMA_CHANNEL_CFG_NEXT_USEBURST_Pos)
#define DMA_CHANNEL_CFG_NEXT_USEBURST              DMA_CHANNEL_CFG_NEXT_USEBURST_Msk

#define DMA_CHANNEL_CFG_N_MINUS_1_Pos              4
#define DMA_CHANNEL_CFG_N_MINUS_1_Msk              (0x3FFUL << DMA_CHANNEL_CFG_N_MINUS_1_Pos)

#define DMA_CHANNEL_CFG_R_POWER_Pos                14
#define DMA_CHANNEL_CFG_R_POWER_Msk                (0xFUL << DMA_CHANNEL_CFG_R_POWER_Pos)

#define DMA_CHANNEL_CFG_SRC_PROT_CTRL_Pos          18
#define DMA_CHANNEL_CFG_SRC_PROT_CTRL_Msk          (0x7UL << DMA_CHANNEL_CFG_SRC_PROT_CTRL_Pos)
#define DMA_CHANNEL_CFG_SRC_PROT_CTRL_HPROT1       (0x1UL << DMA_CHANNEL_CFG_SRC_PROT_CTRL_Pos)
#define DMA_CHANNEL_CFG_SRC_PROT_CTRL_HPROT2       (0x2UL << DMA_CHANNEL_CFG_SRC_PROT_CTRL_Pos)
#define DMA_CHANNEL_CFG_SRC_PROT_CTRL_HPROT3       (0x4UL << DMA_CHANNEL_CFG_SRC_PROT_CTRL_Pos)

#define DMA_CHANNEL_CFG_DST_PROT_CTRL_Pos          21
#define DMA_CHANNEL_CFG_DST_PROT_CTRL_Msk          (0x7UL << DMA_CHANNEL_CFG_DST_PROT_CTRL_Pos)
#define DMA_CHANNEL_CFG_DST_PROT_CTRL_HPROT1       (0x1UL << DMA_CHANNEL_CFG_DST_PROT_CTRL_Pos)
#define DMA_CHANNEL_CFG_DST_PROT_CTRL_HPROT2       (0x2UL << DMA_CHANNEL_CFG_DST_PROT_CTRL_Pos)
#define DMA_CHANNEL_CFG_DST_PROT_CTRL_HPROT3       (0x4UL << DMA_CHANNEL_CFG_DST_PROT_CTRL_Pos)

#define DMA_CHANNEL_CFG_SRC_SIZE_Pos               24
#define DMA_CHANNEL_CFG_SRC_SIZE_Msk               (0x3UL << DMA_CHANNEL_CFG_SRC_SIZE_Pos)
#define DMA_CHANNEL_CFG_SRC_SIZE_8BIT              (0x0UL << DMA_CHANNEL_CFG_SRC_SIZE_Pos)
#define DMA_CHANNEL_CFG_SRC_SIZE_16BIT             (0x1UL << DMA_CHANNEL_CFG_SRC_SIZE_Pos)
#define DMA_CHANNEL_CFG_SRC_SIZE_32BIT             (0x2UL << DMA_CHANNEL_CFG_SRC_SIZE_Pos)

#define DMA_CHANNEL_CFG_SRC_INC_Pos                26
#define DMA_CHANNEL_CFG_SRC_INC_Msk                (0x3UL << DMA_CHANNEL_CFG_SRC_INC_Pos)
#define DMA_CHANNEL_CFG_SRC_INC_8BIT               (0x0UL << DMA_CHANNEL_CFG_SRC_INC_Pos)
#define DMA_CHANNEL_CFG_SRC_INC_16BIT              (0x1UL << DMA_CHANNEL_CFG_SRC_INC_Pos)
#define DMA_CHANNEL_CFG_SRC_INC_32BIT              (0x2UL << DMA_CHANNEL_CFG_SRC_INC_Pos)
#define DMA_CHANNEL_CFG_SRC_INC_NO                 (0x3UL << DMA_CHANNEL_CFG_SRC_INC_Pos)

#define DMA_CHANNEL_CFG_DST_SIZE_Pos               28
#define DMA_CHANNEL_CFG_DST_SIZE_Msk               (0x3UL << DMA_CHANNEL_CFG_DST_SIZE_Pos)
#define DMA_CHANNEL_CFG_DST_SIZE_8BIT              (0x0UL << DMA_CHANNEL_CFG_DST_SIZE_Pos)
#define DMA_CHANNEL_CFG_DST_SIZE_16BIT             (0x1UL << DMA_CHANNEL_CFG_DST_SIZE_Pos)
#define DMA_CHANNEL_CFG_DST_SIZE_32BIT             (0x2UL << DMA_CHANNEL_CFG_DST_SIZE_Pos)

#define DMA_CHANNEL_CFG_DST_INC_Pos                30
#define DMA_CHANNEL_CFG_DST_INC_Msk                (0x3UL << DMA_CHANNEL_CFG_DST_INC_Pos)
#define DMA_CHANNEL_CFG_DST_INC_8BIT               (0x0UL << DMA_CHANNEL_CFG_DST_INC_Pos)
#define DMA_CHANNEL_CFG_DST_INC_16BIT              (0x1UL << DMA_CHANNEL_CFG_DST_INC_Pos)
#define DMA_CHANNEL_CFG_DST_INC_32BIT              (0x2UL << DMA_CHANNEL_CFG_DST_INC_Pos)
#define DMA_CHANNEL_CFG_DST_INC_NO                 (0x3UL << DMA_CHANNEL_CFG_DST_INC_Pos)

/** @} */ /* End of group MDR32VF0xI_DMA_CHANNEL_CFG */

/** @} */ /* End of group MDR32VF0xI_DMA_Defines */

/** @} */ /* End of group MDR32VF0xI_DMA */

/** @defgroup MDR32VF0xI_I2C MDR_I2C
 * @{
 */

/**
 * @brief MDR32VF0xI I2C control structure
 */
typedef struct {
    __IO uint32_t PRL; /*!<I2C Frequency Prescaler Lower Part Register */
    __IO uint32_t PRH; /*!<I2C Frequency Prescaler High Part Register */
    __IO uint32_t CTR; /*!<I2C Control Register */
    __I uint32_t  RXD; /*!<I2C Received Data Register */
    __IO uint32_t STA; /*!<I2C Status Register */
    __IO uint32_t TXD; /*!<I2C Transmitting Data Register */
    __IO uint32_t CMD; /*!<I2C Command Register */
} MDR_I2C_TypeDef;

/** @defgroup MDR32VF0xI_I2C_Defines I2C Defines
 * @{
 */

/** @defgroup MDR32VF0xI_I2C_PRL I2C PRL
 * @{
 */

#define I2C_PRL_PR_Pos 0
#define I2C_PRL_PR_Msk (0xFFUL << I2C_PRL_PR_Pos)

/** @} */ /* End of group MDR32VF0xI_I2C_PRL */

/** @defgroup MDR32VF0xI_I2C_PRH I2C PRH
 * @{
 */

#define I2C_PRH_PR_Pos 0
#define I2C_PRH_PR_Msk (0xFFUL << I2C_PRH_PR_Pos)

/** @} */ /* End of group MDR32VF0xI_I2C_PRH */

/** @defgroup MDR32VF0xI_I2C_CTR I2C CTR
 * @{
 */

#define I2C_CTR_EN_INT_Pos 6
#define I2C_CTR_EN_INT_Msk (0x1UL << I2C_CTR_EN_INT_Pos)
#define I2C_CTR_EN_INT     I2C_CTR_EN_INT_Msk

#define I2C_CTR_EN_I2C_Pos 7
#define I2C_CTR_EN_I2C_Msk (0x1UL << I2C_CTR_EN_I2C_Pos)
#define I2C_CTR_EN_I2C     I2C_CTR_EN_I2C_Msk

/** @} */ /* End of group MDR32VF0xI_I2C_CTR */

/** @defgroup MDR32VF0xI_I2C_RXD I2C RXD
 * @{
 */

#define I2C_RXD_RXD_Pos 0
#define I2C_RXD_RXD_Msk (0xFFUL << I2C_RXD_RXD_Pos)

/** @} */ /* End of group MDR32VF0xI_I2C_RXD */

/** @defgroup MDR32VF0xI_I2C_STA I2C STA
 * @{
 */

#define I2C_STA_INT_Pos      0
#define I2C_STA_INT_Msk      (0x1UL << I2C_STA_INT_Pos)
#define I2C_STA_INT          I2C_STA_INT_Msk

#define I2C_STA_TR_PROG_Pos  1
#define I2C_STA_TR_PROG_Msk  (0x1UL << I2C_STA_TR_PROG_Pos)
#define I2C_STA_TR_PROG      I2C_STA_TR_PROG_Msk

#define I2C_STA_LOST_ARB_Pos 5
#define I2C_STA_LOST_ARB_Msk (0x1UL << I2C_STA_LOST_ARB_Pos)
#define I2C_STA_LOST_ARB     I2C_STA_LOST_ARB_Msk

#define I2C_STA_BUSY_Pos     6
#define I2C_STA_BUSY_Msk     (0x1UL << I2C_STA_BUSY_Pos)
#define I2C_STA_BUSY         I2C_STA_BUSY_Msk

#define I2C_STA_RX_ACK_Pos   7
#define I2C_STA_RX_ACK_Msk   (0x1UL << I2C_STA_RX_ACK_Pos)
#define I2C_STA_RX_ACK       I2C_STA_RX_ACK_Msk

/** @} */ /* End of group MDR32VF0xI_I2C_STA */

/** @defgroup MDR32VF0xI_I2C_TXD I2C TXD
 * @{
 */

#define I2C_TXD_TXD_Pos 0
#define I2C_TXD_TXD_Msk (0xFFUL << I2C_TXD_TXD_Pos)

/** @} */ /* End of group MDR32VF0xI_I2C_TXD */

/** @defgroup MDR32VF0xI_I2C_CMD I2C CMD
 * @{
 */

#define I2C_CMD_CLRINT_Pos 0
#define I2C_CMD_CLRINT_Msk (0x1UL << I2C_CMD_CLRINT_Pos)
#define I2C_CMD_CLRINT     I2C_CMD_CLRINT_Msk

#define I2C_CMD_ACK_Pos    3
#define I2C_CMD_ACK_Msk    (0x1UL << I2C_CMD_ACK_Pos)
#define I2C_CMD_ACK        I2C_CMD_ACK_Msk

#define I2C_CMD_WR_Pos     4
#define I2C_CMD_WR_Msk     (0x1UL << I2C_CMD_WR_Pos)
#define I2C_CMD_WR         I2C_CMD_WR_Msk

#define I2C_CMD_RD_Pos     5
#define I2C_CMD_RD_Msk     (0x1UL << I2C_CMD_RD_Pos)
#define I2C_CMD_RD         I2C_CMD_RD_Msk

#define I2C_CMD_STOP_Pos   6
#define I2C_CMD_STOP_Msk   (0x1UL << I2C_CMD_STOP_Pos)
#define I2C_CMD_STOP       I2C_CMD_STOP_Msk

#define I2C_CMD_START_Pos  7
#define I2C_CMD_START_Msk  (0x1UL << I2C_CMD_START_Pos)
#define I2C_CMD_START      I2C_CMD_START_Msk

/** @} */ /* End of group MDR32VF0xI_I2C_CMD */

/** @} */ /* End of group MDR32VF0xI_I2C_Defines */

/** @} */ /* End of group MDR32VF0xI_I2C */

/** @defgroup MDR32VF0xI_ADC MDR_ADC
 * @{
 */

/**
 * @brief MDR32VF0xI ADC control structure.
 */
typedef struct {
    __IO uint32_t ADC1_CFG;     /*!<ADC1 Configuration Register */
    __I uint32_t  RESERVED0;    /*!<Reserved */
    __IO uint32_t ADC1_H_LEVEL; /*!<ADC1 High Level Register */
    __I uint32_t  RESERVED1;    /*!<Reserved */
    __IO uint32_t ADC1_L_LEVEL; /*!<ADC1 Low Level Register */
    __I uint32_t  RESERVED2;    /*!<Reserved */
    __IO uint32_t ADC1_RESULT;  /*!<ADC1 Result Register */
    __I uint32_t  RESERVED3;    /*!<Reserved */
    __IO uint32_t ADC1_STATUS;  /*!<ADC1 Status Register */
    __I uint32_t  RESERVED4;    /*!<Reserved */
    __IO uint32_t ADC1_CHSEL;   /*!<ADC1 Channel Selector Register */
    __I uint32_t  RESERVED5;    /*!<Reserved */
    __IO uint32_t ADC_TRIM;     /*!<ADC Reference Voltage Trim Register */
} MDR_ADC_TypeDef;

/** @defgroup MDR32VF0xI_ADC_Defines ADC Defines
 * @{
 */

/** @defgroup MDR32VF0xI_ADC1_CFG ADC ADC1_CFG
 * @{
 */

#define ADC1_CFG_REG_ADON_Pos   0
#define ADC1_CFG_REG_ADON_Msk   (0x1UL << ADC1_CFG_REG_ADON_Pos)
#define ADC1_CFG_REG_ADON       ADC1_CFG_REG_ADON_Msk

#define ADC1_CFG_REG_GO_Pos     1
#define ADC1_CFG_REG_GO_Msk     (0x1UL << ADC1_CFG_REG_GO_Pos)
#define ADC1_CFG_REG_GO         ADC1_CFG_REG_GO_Msk

#define ADC1_CFG_REG_CLKS_Pos   2
#define ADC1_CFG_REG_CLKS_Msk   (0x1UL << ADC1_CFG_REG_CLKS_Pos)
#define ADC1_CFG_REG_CLKS       ADC1_CFG_REG_CLKS_Msk

#define ADC1_CFG_REG_SAMPLE_Pos 3
#define ADC1_CFG_REG_SAMPLE_Msk (0x1UL << ADC1_CFG_REG_SAMPLE_Pos)
#define ADC1_CFG_REG_SAMPLE     ADC1_CFG_REG_SAMPLE_Msk

#define ADC1_CFG_REG_CHS_Pos    4
#define ADC1_CFG_REG_CHS_Msk    (0x7UL << ADC1_CFG_REG_CHS_Pos)

#define ADC1_CFG_REG_CHCH_Pos   9
#define ADC1_CFG_REG_CHCH_Msk   (0x1UL << ADC1_CFG_REG_CHCH_Pos)
#define ADC1_CFG_REG_CHCH       ADC1_CFG_REG_CHCH_Msk

#define ADC1_CFG_REG_RNGC_Pos   10
#define ADC1_CFG_REG_RNGC_Msk   (0x1UL << ADC1_CFG_REG_RNGC_Pos)
#define ADC1_CFG_REG_RNGC       ADC1_CFG_REG_RNGC_Msk

#define ADC1_CFG_M_REF_Pos      11
#define ADC1_CFG_M_REF_Msk      (0x1UL << ADC1_CFG_M_REF_Pos)
#define ADC1_CFG_M_REF          ADC1_CFG_M_REF_Msk

#define ADC1_CFG_REG_DIVCLK_Pos 12
#define ADC1_CFG_REG_DIVCLK_Msk (0xFUL << ADC1_CFG_REG_DIVCLK_Pos)

#if defined(USE_MDR32F02)
#define ADC1_CFG_EN_NOISE_RNG_Pos 19
#define ADC1_CFG_EN_NOISE_RNG_Msk (0x1UL << ADC1_CFG_EN_NOISE_RNG_Pos)
#define ADC1_CFG_EN_NOISE_RNG     ADC1_CFG_EN_NOISE_RNG_Msk
#endif

#define ADC1_CFG_DELAY_GO_Pos 25
#define ADC1_CFG_DELAY_GO_Msk (0x7UL << ADC1_CFG_DELAY_GO_Pos)

/** @} */ /* End of group MDR32VF0xI_ADC1_CFG */

/** @defgroup MDR32VF0xI_ADC1_H_LEVEL ADC ADC1_H_LEVEL
 * @{
 */

#define ADC1_H_LEVEL_REG_H_LEVEL_Pos 0
#define ADC1_H_LEVEL_REG_H_LEVEL_Msk (0x3FFUL << ADC1_H_LEVEL_REG_H_LEVEL_Pos)

/** @} */ /* End of group MDR32VF0xI_ADC1_H_LEVEL */

/** @defgroup MDR32VF0xI_ADC1_L_LEVEL ADC ADC1_L_LEVEL
 * @{
 */

#define ADC1_L_LEVEL_REG_L_LEVEL_Pos 0
#define ADC1_L_LEVEL_REG_L_LEVEL_Msk (0x3FFUL << ADC1_L_LEVEL_REG_L_LEVEL_Pos)

/** @} */ /* End of group MDR32VF0xI_ADC1_L_LEVEL */

/** @defgroup MDR32VF0xI_ADC1_RESULT ADC ADC1_RESULT
 * @{
 */

#define ADC1_RESULT_RESULT_Pos  0
#define ADC1_RESULT_RESULT_Msk  (0x3FFUL << ADC1_RESULT_RESULT_Pos)

#define ADC1_RESULT_CHANNEL_Pos 16
#define ADC1_RESULT_CHANNEL_Msk (0x7UL << ADC1_RESULT_CHANNEL_Pos)

/** @} */ /* End of group MDR32VF0xI_ADC1_RESULT */

/** @defgroup MDR32VF0xI_ADC1_STATUS ADC ADC1_STATUS
 * @{
 */

#define ADC1_STATUS_FLG_REG_OVERWRITE_Pos 0
#define ADC1_STATUS_FLG_REG_OVERWRITE_Msk (0x1UL << ADC1_STATUS_FLG_REG_OVERWRITE_Pos)
#define ADC1_STATUS_FLG_REG_OVERWRITE     ADC1_STATUS_FLG_REG_OVERWRITE_Msk

#define ADC1_STATUS_FLG_REG_AWOIFEN_Pos   1
#define ADC1_STATUS_FLG_REG_AWOIFEN_Msk   (0x1UL << ADC1_STATUS_FLG_REG_AWOIFEN_Pos)
#define ADC1_STATUS_FLG_REG_AWOIFEN       ADC1_STATUS_FLG_REG_AWOIFEN_Msk

#define ADC1_STATUS_FLG_REG_EOCIF_Pos     2
#define ADC1_STATUS_FLG_REG_EOCIF_Msk     (0x1UL << ADC1_STATUS_FLG_REG_EOCIF_Pos)
#define ADC1_STATUS_FLG_REG_EOCIF         ADC1_STATUS_FLG_REG_EOCIF_Msk

#define ADC1_STATUS_AWOIF_IE_Pos          3
#define ADC1_STATUS_AWOIF_IE_Msk          (0x1UL << ADC1_STATUS_AWOIF_IE_Pos)
#define ADC1_STATUS_AWOIF_IE              ADC1_STATUS_AWOIF_IE_Msk

#define ADC1_STATUS_ECOIF_IE_Pos          4
#define ADC1_STATUS_ECOIF_IE_Msk          (0x1UL << ADC1_STATUS_ECOIF_IE_Pos)
#define ADC1_STATUS_ECOIF_IE              ADC1_STATUS_ECOIF_IE_Msk

/** @} */ /* End of group MDR32VF0xI_ADC1_STATUS */

/** @defgroup MDR32VF0xI_ADC1_CHSEL ADC ADC1_CHSEL
 * @{
 */

#define ADC1_CHSEL_SL_CH_CH_REF0_Pos 0
#define ADC1_CHSEL_SL_CH_CH_REF0_Msk (0x1UL << ADC1_CHSEL_SL_CH_CH_REF0_Pos)
#define ADC1_CHSEL_SL_CH_CH_REF0     ADC1_CHSEL_SL_CH_CH_REF0_Msk

#define ADC1_CHSEL_SL_CH_CH_REF1_Pos 1
#define ADC1_CHSEL_SL_CH_CH_REF1_Msk (0x1UL << ADC1_CHSEL_SL_CH_CH_REF1_Pos)
#define ADC1_CHSEL_SL_CH_CH_REF1     ADC1_CHSEL_SL_CH_CH_REF1_Msk

#define ADC1_CHSEL_SL_CH_CH_REF2_Pos 2
#define ADC1_CHSEL_SL_CH_CH_REF2_Msk (0x1UL << ADC1_CHSEL_SL_CH_CH_REF2_Pos)
#define ADC1_CHSEL_SL_CH_CH_REF2     ADC1_CHSEL_SL_CH_CH_REF2_Msk

#define ADC1_CHSEL_SL_CH_CH_REF3_Pos 3
#define ADC1_CHSEL_SL_CH_CH_REF3_Msk (0x1UL << ADC1_CHSEL_SL_CH_CH_REF3_Pos)
#define ADC1_CHSEL_SL_CH_CH_REF3     ADC1_CHSEL_SL_CH_CH_REF3_Msk

#define ADC1_CHSEL_SL_CH_CH_REF4_Pos 4
#define ADC1_CHSEL_SL_CH_CH_REF4_Msk (0x1UL << ADC1_CHSEL_SL_CH_CH_REF4_Pos)
#define ADC1_CHSEL_SL_CH_CH_REF4     ADC1_CHSEL_SL_CH_CH_REF4_Msk

#define ADC1_CHSEL_SL_CH_CH_REF5_Pos 5
#define ADC1_CHSEL_SL_CH_CH_REF5_Msk (0x1UL << ADC1_CHSEL_SL_CH_CH_REF5_Pos)
#define ADC1_CHSEL_SL_CH_CH_REF5     ADC1_CHSEL_SL_CH_CH_REF5_Msk

#define ADC1_CHSEL_SL_CH_CH_REF6_Pos 6
#define ADC1_CHSEL_SL_CH_CH_REF6_Msk (0x1UL << ADC1_CHSEL_SL_CH_CH_REF6_Pos)
#define ADC1_CHSEL_SL_CH_CH_REF6     ADC1_CHSEL_SL_CH_CH_REF6_Msk

#define ADC1_CHSEL_SL_CH_CH_REF7_Pos 7
#define ADC1_CHSEL_SL_CH_CH_REF7_Msk (0x1UL << ADC1_CHSEL_SL_CH_CH_REF7_Pos)
#define ADC1_CHSEL_SL_CH_CH_REF7     ADC1_CHSEL_SL_CH_CH_REF7_Msk

/** @} */ /* End of group MDR32VF0xI_ADC1_CHSEL */

/** @defgroup MDR32VF0xI_ADC_TRIM ADC ADC_TRIM
 * @{
 */

#define ADC_TRIM_BG_TRIM_Pos 1
#define ADC_TRIM_BG_TRIM_Msk (0x1FUL << ADC_TRIM_BG_TRIM_Pos)

/** @} */ /* End of group MDR32VF0xI_ADC_TRIM */

/** @} */ /* End of group MDR32VF0xI_ADC_Defines */

/** @} */ /* End of group MDR32VF0xI_ADC */

/** @defgroup MDR32VF0xI_WWDG MDR_WWDG
 * @{
 */

/**
 * @brief MDR32VF0xI WWDG control structure.
 */
typedef struct {
    __IO uint32_t CR;  /*!<WWDG Command Register */
    __IO uint32_t CFR; /*!<WWDG Configuration Register */
    __IO uint32_t SR;  /*!<WWDG Status Register */
} MDR_WWDG_TypeDef;

/** @defgroup MDR32VF0xI_IWDG_Defines IWDG Defines
 * @{
 */

/** @defgroup MDR32VF0xI_WWDG_CR WWDG_CR
 * @{
 */

#define WWDG_CR_T_Pos    0
#define WWDG_CR_T_Msk    (0x7FUL << WWDG_CR_T_Pos)

#define WWDG_CR_WDGA_Pos 7
#define WWDG_CR_WDGA_Msk (0x1UL << WWDG_CR_WDGA_Pos)
#define WWDG_CR_WDGA     WWDG_CR_WDGA_Msk

/** @} */ /* End of group MDR32VF0xI_WWDG_CR */

/** @defgroup MDR32VF0xI_WWDG_CFR WWDG CFR
 * @{
 */

#define WWDG_CFR_W_Pos       0
#define WWDG_CFR_W_Msk       (0x7FUL << WWDG_CFR_W_Pos)

#define WWDG_CFR_WDGTB_Pos   7
#define WWDG_CFR_WDGTB_Msk   (0x3UL << WWDG_CFR_WDGTB_Pos)
#define WWDG_CFR_WDGTB_DIV_1 (0x0UL << IWDG_PR_PR_Pos)
#define WWDG_CFR_WDGTB_DIV_2 (0x1UL << IWDG_PR_PR_Pos)
#define WWDG_CFR_WDGTB_DIV_4 (0x2UL << IWDG_PR_PR_Pos)
#define WWDG_CFR_WDGTB_DIV_8 (0x3UL << IWDG_PR_PR_Pos)

#define WWDG_CFR_EWI_Pos     9
#define WWDG_CFR_EWI_Msk     (0x1UL << WWDG_CFR_EWI_Pos)
#define WWDG_CFR_EWI         WWDG_CFR_EWI_Msk

/** @} */ /* End of group MDR32VF0xI_WWDG_CFR */

/** @defgroup MDR32VF0xI_WWDG_SR WWDG_SR
 * @{
 */

#define WWDG_SR_EWIF_Pos    0
#define WWDG_SR_EWIF_Msk    (0x1UL << WWDG_SR_EWIF_Pos)
#define WWDG_SR_EWIF        WWDG_SR_EWIF_Msk

#define WWDG_SR_WDG_WEC_Pos 1
#define WWDG_SR_WDG_WEC_Msk (0x1UL << WWDG_SR_WDG_WEC_Pos)
#define WWDG_SR_WDG_WEC     WWDG_SR_WDG_WEC_Msk

/** @} */ /* End of group MDR32VF0xI_WWDG_SR */

/** @} */ /* End of group MDR32VF0xI_WWDG_Defines */

/** @} */ /* End of group MDR32VF0xI_WWDG */

/** @defgroup MDR32VF0xI_IWDG MDR_IWDG
 * @{
 */

/**
 * @brief MDR32VF0xI IWDG control structure.
 */
typedef struct {
    __O uint32_t  KR;  /*!<IWDG Key Register */
    __IO uint32_t PR;  /*!<IWDG Clock Prescaler Register */
    __IO uint32_t RLR; /*!<IWDG Reload Register */
    __I uint32_t  SR;  /*!<IWDG Status Register */
} MDR_IWDG_TypeDef;

/** @defgroup MDR32VF0xI_IWDG_Defines IWDG Defines
 * @{
 */

/** @defgroup MDR32VF0xI_IWDG_KR IWDG_KR
 * @{
 */

#define IWDG_KR_KEY_Pos          0
#define IWDG_KR_KEY_Msk          (0xFFFFUL << IWDG_KR_KEY_Pos)
#define IWDG_KR_KEY_RELOAD       0x0000AAAAUL
#define IWDG_KR_KEY_ENABLE       0x0000CCCCUL
#define IWDG_KR_KEY_WRITE_ENABLE 0x00005555UL

/** @} */ /* End of group MDR32VF0xI_IWDG_KR */

/** @defgroup MDR32VF0xI_IWDG_PR IWDG_PR
 * @{
 */

#define IWDG_PR_PR_Pos     0
#define IWDG_PR_PR_Msk     (0x7UL << IWDG_PR_PR_Pos)
#define IWDG_PR_PR_DIV_4   (0x0UL << IWDG_PR_PR_Pos)
#define IWDG_PR_PR_DIV_8   (0x1UL << IWDG_PR_PR_Pos)
#define IWDG_PR_PR_DIV_16  (0x2UL << IWDG_PR_PR_Pos)
#define IWDG_PR_PR_DIV_32  (0x3UL << IWDG_PR_PR_Pos)
#define IWDG_PR_PR_DIV_64  (0x4UL << IWDG_PR_PR_Pos)
#define IWDG_PR_PR_DIV_128 (0x5UL << IWDG_PR_PR_Pos)
#define IWDG_PR_PR_DIV_256 (0x6UL << IWDG_PR_PR_Pos)

/** @} */ /* End of group MDR32VF0xI_IWDG_PR */

/** @defgroup MDR32VF0xI_IWDG_RLR IWDG_RLR
 * @{
 */

#define IWDG_RLR_RLR_Pos 0
#define IWDG_RLR_RLR_Msk (0xFFFUL << IWDG_RLR_RLR_Pos)

/** @} */ /* End of group MDR32VF0xI_IWDG_RLR */

/** @defgroup MDR32VF0xI_IWDG_SR IWDG_SR
 * @{
 */

#define IWDG_SR_PVU_Pos 0
#define IWDG_SR_PVU_Msk (0x1UL << IWDG_SR_PVU_Pos)
#define IWDG_SR_PVU     IWDG_SR_PVU_Msk

#define IWDG_SR_RVU_Pos 1
#define IWDG_SR_RVU_Msk (0x1UL << IWDG_SR_RVU_Pos)
#define IWDG_SR_RVU     IWDG_SR_RVU_Msk

/** @} */ /* End of group MDR32VF0xI_IWDG_SR */

/** @} */ /* End of group MDR32VF0xI_IWDG_Defines */

/** @} */ /* End of group MDR32VF0xI_IWDG */

/** @defgroup MDR32VF0xI_POWER MDR_POWER
 * @{
 */

/**
 * @brief MDR32VF0xI POWER control structure.
 */
typedef struct {
    __IO uint32_t PVDCS; /*!<POWER Power Voltage Detector Control and Status Register */
} MDR_POWER_TypeDef;

/** @defgroup MDR32VF0xI_POWER_Defines POWER Defines
 * @{
 */

/** @defgroup MDR32VF0xI_POWER_PVDCS POWER PVDCS
 * @{
 */

#define POWER_PVDCS_PVDBEN_Pos 0
#define POWER_PVDCS_PVDBEN_Msk (0x1UL << POWER_PVDCS_PVDBEN_Pos)
#define POWER_PVDCS_PVDBEN     POWER_PVDCS_PVDBEN_Msk

#define POWER_PVDCS_PVDEN_Pos  2
#define POWER_PVDCS_PVDEN_Msk  (0x1UL << POWER_PVDCS_PVDEN_Pos)
#define POWER_PVDCS_PVDEN      POWER_PVDCS_PVDEN_Msk

#define POWER_PVDCS_PLSB_Pos   3
#define POWER_PVDCS_PLSB_Msk   (0x7UL << POWER_PVDCS_PLSB_Pos)

#define POWER_PVDCS_PLS_Pos    13
#define POWER_PVDCS_PLS_Msk    (0x7UL << POWER_PVDCS_PLS_Pos)

#define POWER_PVDCS_PVBD_Pos   18
#define POWER_PVDCS_PVBD_Msk   (0x1UL << POWER_PVDCS_PVBD_Pos)
#define POWER_PVDCS_PVBD       POWER_PVDCS_PVBD_Msk

#define POWER_PVDCS_PVD_Pos    20
#define POWER_PVDCS_PVD_Msk    (0x1UL << POWER_PVDCS_PVD_Pos)
#define POWER_PVDCS_PVD        POWER_PVDCS_PVD_Msk

#define POWER_PVDCS_IEPVDB_Pos 21
#define POWER_PVDCS_IEPVDB_Msk (0x1UL << POWER_PVDCS_IEPVDB_Pos)
#define POWER_PVDCS_IEPVDB     POWER_PVDCS_IEPVDB_Msk

#define POWER_PVDCS_IEPVD_Pos  22
#define POWER_PVDCS_IEPVD_Msk  (0x1UL << POWER_PVDCS_IEPVD_Pos)
#define POWER_PVDCS_IEPVD      POWER_PVDCS_IEPVD_Msk

#define POWER_PVDCS_INV_Pos    24
#define POWER_PVDCS_INV_Msk    (0x1UL << POWER_PVDCS_INV_Pos)
#define POWER_PVDCS_INV        POWER_PVDCS_INV_Msk

#define POWER_PVDCS_INVB_Pos   26
#define POWER_PVDCS_INVB_Msk   (0x1UL << POWER_PVDCS_INVB_Pos)
#define POWER_PVDCS_INVB       POWER_PVDCS_INVB_Msk

/** @} */ /* End of group MDR32VF0xI_POWER_PVDCS */

/** @} */ /* End of group MDR32VF0xI_POWER_Defines */

/** @} */ /* End of group MDR32VF0xI_POWER */

/** @defgroup MDR32VF0xI_BKP MDR_BKP
 * @{
 */

/**
 * @brief MDR32VF0xI BKP and RTC control structure.
 */
typedef struct {
    __IO uint32_t WPR;          /*!<BKP and RTC Protection Key Register */
    __IO uint32_t LDO;          /*!<BKP LDO Control Register */
    __IO uint32_t CLK;          /*!<BKP Clock Control Register */
    __IO uint32_t RTC_CR;       /*!<RTC Control Register */
    __IO uint32_t RTC_WUTR;     /*!<RTC Watchdog Counter Register */
    __IO uint32_t RTC_PREDIV_S; /*!<RTC Prescaler Counter Register */
    __IO uint32_t RTC_PRL;      /*!<RTC Prescaler Reload Value Register */
    __IO uint32_t RTC_ALRM;     /*!<RTC Alarm Register */
    __IO uint32_t RTC_CS;       /*!<RTC Control and Status Register */
    __IO uint32_t RTC_TR;       /*!<RTC Time Register */
    __IO uint32_t RTC_DR;       /*!<RTC Date Register */
    __IO uint32_t RTC_ALRMAR;   /*!<RTC Alarm A Reference Register */
    __IO uint32_t RTC_ALRMBR;   /*!<RTC Alarm B Reference Register */
    __I uint32_t  RTC_TSTR1;    /*!<RTC Timestamp1 Time Registerer */
    __I uint32_t  RTC_TSTR2;    /*!<RTC Timestamp2 Time Registerer */
    __I uint32_t  RTC_TSTR3;    /*!<RTC Timestamp3 Time Registerer */
    __I uint32_t  RTC_TSDR1;    /*!<RTC Timestamp1 Date Registerer */
    __I uint32_t  RTC_TSDR2;    /*!<RTC Timestamp2 Date Registerer */
    __I uint32_t  RTC_TSDR3;    /*!<RTC Timestamp3 Date Registerer */
    __IO uint32_t RTC_TAFCR;    /*!<RTC Tamper/Timestamp Control*/
#if defined(USE_MDR32F02)
    __I uint32_t RESERVED0[1004]; /*!<Reserved */
#elif defined(USE_MDR1206FI) || defined(USE_MDR1206AFI) || defined(USE_MDR1206)
    __IO uint32_t RTC_TMPCAL1;     /*!<RTC Temperature Compensation Calibration and Control Register 1 */
    __IO uint32_t RTC_TMPCAL2;     /*!<RTC Temperature Compensation Calibration and Control Register 2 */
    __IO uint32_t RTC_TMPCAL3;     /*!<RTC Temperature Compensation Calibration and Control Register 3 */
    __I uint32_t  RESERVED0[1001]; /*!<Reserved */
#endif
    __IO uint32_t MEM[128]; /*!<BKP Memory */
} MDR_BKP_TypeDef;

/** @defgroup MDR32VF0xI_BKP_Defines BKP Defines
 * @{
 */

/** @defgroup MDR32VF0xI_BKP_WPR BKP WPR
 * @{
 */

#define BKP_WPR_BKP_WPR_Pos    0
#define BKP_WPR_BKP_WPR_Msk    (0xFFFFFFFFUL << BKP_WPR_BKP_WPR_Pos)
#define BKP_WPR_BKP_WPR_LOCK   (0x00000000UL)
#define BKP_WPR_BKP_WPR_UNLOCK (0x8555AAA1UL)

/** @} */ /* End of group MDR32VF0xI_BKP_WPR */

/** @defgroup MDR32VF0xI_BKP_LDO BKP LDO
 * @{
 */

#define BKP_LDO_BLDO_TRIM_Pos  0
#define BKP_LDO_BLDO_TRIM_Msk  (0x3UL << BKP_LDO_BLDO_TRIM_Pos)

#define BKP_LDO_BLDO_BOOST_Pos 2
#define BKP_LDO_BLDO_BOOST_Msk (0x1UL << BKP_LDO_BLDO_BOOST_Pos)
#define BKP_LDO_BLDO_BOOST     BKP_LDO_BLDO_BOOST_Msk

#define BKP_LDO_LDO_BOOST_Pos  3
#define BKP_LDO_LDO_BOOST_Msk  (0x3UL << BKP_LDO_LDO_BOOST_Pos)

#define BKP_LDO_SW_CNTRL_Pos   5
#define BKP_LDO_SW_CNTRL_Msk   (0x1UL << BKP_LDO_SW_CNTRL_Pos)
#define BKP_LDO_SW_CNTRL       BKP_LDO_SW_CNTRL_Msk

#define BKP_LDO_JTAG_ON_Pos    6
#define BKP_LDO_JTAG_ON_Msk    (0x1UL << BKP_LDO_JTAG_ON_Pos)
#define BKP_LDO_JTAG_ON        BKP_LDO_JTAG_ON_Msk

#define BKP_LDO_WAKEUP1_Pos    7
#define BKP_LDO_WAKEUP1_Msk    (0x1UL << BKP_LDO_WAKEUP1_Pos)
#define BKP_LDO_WAKEUP1        BKP_LDO_WAKEUP1_Msk

#define BKP_LDO_WAKEUP2_Pos    8
#define BKP_LDO_WAKEUP2_Msk    (0x1UL << BKP_LDO_WAKEUP2_Pos)
#define BKP_LDO_WAKEUP2        BKP_LDO_WAKEUP2_Msk

#define BKP_LDO_WAKEUP3_Pos    9
#define BKP_LDO_WAKEUP3_Msk    (0x1UL << BKP_LDO_WAKEUP3_Pos)
#define BKP_LDO_WAKEUP3        BKP_LDO_WAKEUP3_Msk

#define BKP_LDO_FPOR_Pos       25
#define BKP_LDO_FPOR_Msk       (0x1UL << BKP_LDO_FPOR_Pos)
#define BKP_LDO_FPOR           BKP_LDO_FPOR_Msk

#define BKP_LDO_TAMPF_Pos      26
#define BKP_LDO_TAMPF_Msk      (0x1UL << BKP_LDO_TAMPF_Pos)
#define BKP_LDO_TAMPF          BKP_LDO_TAMPF_Msk

#define BKP_LDO_MODE_Pos       27
#define BKP_LDO_MODE_Msk       (0x3UL << BKP_LDO_MODE_Pos)

#define BKP_LDO_SW_OFF_Pos     29
#define BKP_LDO_SW_OFF_Msk     (0x1UL << BKP_LDO_SW_OFF_Pos)
#define BKP_LDO_SW_OFF         BKP_LDO_SW_OFF_Msk

/** @} */ /* End of group MDR32VF0xI_BKP_LDO */

/** @defgroup MDR32VF0xI_BKP_CLK BKP CLK
 * @{
 */

#define BKP_CLK_LSE_ON_Pos       0
#define BKP_CLK_LSE_ON_Msk       (0x1UL << BKP_CLK_LSE_ON_Pos)
#define BKP_CLK_LSE_ON           BKP_CLK_LSE_ON_Msk

#define BKP_CLK_LSE_BYP_Pos      1
#define BKP_CLK_LSE_BYP_Msk      (0x1UL << BKP_CLK_LSE_BYP_Pos)
#define BKP_CLK_LSE_BYP          BKP_CLK_LSE_BYP_Msk

#define BKP_CLK_LSE_RDY_Pos      2
#define BKP_CLK_LSE_RDY_Msk      (0x1UL << BKP_CLK_LSE_RDY_Pos)
#define BKP_CLK_LSE_RDY          BKP_CLK_LSE_RDY_Msk

#define BKP_CLK_LSE_CONF_Pos     3
#define BKP_CLK_LSE_CONF_Msk     (0x3UL << BKP_CLK_LSE_CONF_Pos)

#define BKP_CLK_LSI_ON_Pos       5
#define BKP_CLK_LSI_ON_Msk       (0x1UL << BKP_CLK_LSI_ON_Pos)
#define BKP_CLK_LSI_ON           BKP_CLK_LSI_ON_Msk

#define BKP_CLK_LSI_RDY_Pos      12
#define BKP_CLK_LSI_RDY_Msk      (0x1UL << BKP_CLK_LSI_RDY_Pos)
#define BKP_CLK_LSI_RDY          BKP_CLK_LSI_RDY_Msk

#define BKP_CLK_HSI_ON_Pos       13
#define BKP_CLK_HSI_ON_Msk       (0x1UL << BKP_CLK_HSI_ON_Pos)
#define BKP_CLK_HSI_ON           BKP_CLK_HSI_ON_Msk

#define BKP_CLK_HSI_RDY_Pos      14
#define BKP_CLK_HSI_RDY_Msk      (0x1UL << BKP_CLK_HSI_RDY_Pos)
#define BKP_CLK_HSI_RDY          BKP_CLK_HSI_RDY_Msk

#define BKP_CLK_HSI_LSI_TRIM_Pos 15
#define BKP_CLK_HSI_LSI_TRIM_Msk (0x3FUL << BKP_CLK_HSI_LSI_TRIM_Pos)

/** @} */ /* End of group MDR32VF0xI_BKP_CLK */

/** @defgroup MDR32VF0xI_BKP_RTC_CR BKP RTC_CR
 * @{
 */

#define BKP_RTC_CR_RTC_SEL_Pos             0
#define BKP_RTC_CR_RTC_SEL_Msk             (0x3UL << BKP_RTC_CR_RTC_SEL_Pos)
#define BKP_RTC_CR_RTC_SEL_LSI             (0x0UL << BKP_RTC_CR_RTC_SEL_Pos)
#define BKP_RTC_CR_RTC_SEL_LSE             (0x1UL << BKP_RTC_CR_RTC_SEL_Pos)
#define BKP_RTC_CR_RTC_SEL_HSI_RTC         (0x2UL << BKP_RTC_CR_RTC_SEL_Pos)
#define BKP_RTC_CR_RTC_SEL_HSE_RTC         (0x3UL << BKP_RTC_CR_RTC_SEL_Pos)

#define BKP_RTC_CR_RTC_EN_Pos              2
#define BKP_RTC_CR_RTC_EN_Msk              (0x1UL << BKP_RTC_CR_RTC_EN_Pos)
#define BKP_RTC_CR_RTC_EN                  BKP_RTC_CR_RTC_EN_Msk

#define BKP_RTC_CR_RTC_CAL_Pos             3
#define BKP_RTC_CR_RTC_CAL_Msk             (0xFFUL << BKP_RTC_CR_RTC_CAL_Pos)

#define BKP_RTC_CR_RTC_RESET_Pos           11
#define BKP_RTC_CR_RTC_RESET_Msk           (0x1UL << BKP_RTC_CR_RTC_RESET_Pos)
#define BKP_RTC_CR_RTC_RESET               BKP_RTC_CR_RTC_RESET_Msk

#define BKP_RTC_CR_WUCK_SEL_Pos            12
#define BKP_RTC_CR_WUCK_SEL_Msk            (0x7UL << BKP_RTC_CR_WUCK_SEL_Pos)
#define BKP_RTC_CR_WUCK_SEL_RTC_CLK_DIV_16 (0x0UL << BKP_RTC_CR_WUCK_SEL_Pos)
#define BKP_RTC_CR_WUCK_SEL_RTC_CLK_DIV_8  (0x1UL << BKP_RTC_CR_WUCK_SEL_Pos)
#define BKP_RTC_CR_WUCK_SEL_RTC_CLK_DIV_4  (0x2UL << BKP_RTC_CR_WUCK_SEL_Pos)
#define BKP_RTC_CR_WUCK_SEL_RTC_CLK_DIV_2  (0x3UL << BKP_RTC_CR_WUCK_SEL_Pos)
#define BKP_RTC_CR_WUCK_SEL_SEC_CLK        (0x4UL << BKP_RTC_CR_WUCK_SEL_Pos)

#define BKP_RTC_CR_FMT_Pos                 25
#define BKP_RTC_CR_FMT_Msk                 (0x1UL << BKP_RTC_CR_FMT_Pos)
#define BKP_RTC_CR_FMT                     BKP_RTC_CR_FMT_Msk

/** @} */ /* End of group MDR32VF0xI_BKP_RTC_CR */

/** @defgroup MDR32VF0xI_BKP_RTC_WUTR BKP RTC_WUTR
 * @{
 */

#define BKP_RTC_WUTR_RTC_WUTR_Pos 0
#define BKP_RTC_WUTR_RTC_WUTR_Msk (0xFFFFFFFFUL << BKP_RTC_WUTR_RTC_WUTR_Pos)

/** @} */ /* End of group MDR32VF0xI_BKP_RTC_WUTR */

/** @defgroup MDR32VF0xI_BKP_RTC_PREDIV_S BKP RTC_PREDIV_S
 * @{
 */

#define BKP_RTC_PREDIV_S_RTC_DIV_Pos 0
#define BKP_RTC_PREDIV_S_RTC_DIV_Msk (0xFFFFFUL << BKP_RTC_PREDIV_S_RTC_DIV_Pos)

/** @} */ /* End of group MDR32VF0xI_BKP_RTC_PREDIV_S */

/** @defgroup MDR32VF0xI_BKP_RTC_PRL BKP RTC_PRL
 * @{
 */

#define BKP_RTC_PRL_RTC_PRL_Pos 0
#define BKP_RTC_PRL_RTC_PRL_Msk (0xFFFFFUL << BKP_RTC_PRL_RTC_PRL_Pos)

/** @} */ /* End of group MDR32VF0xI_BKP_RTC_PRL */

/** @defgroup MDR32VF0xI_BKP_RTC_ALRM BKP RTC_ALRM
 * @{
 */

#define BKP_RTC_ALRM_RTC_ALRM_Pos 0
#define BKP_RTC_ALRM_RTC_ALRM_Msk (0xFFFFFFFFUL << BKP_RTC_ALRM_RTC_ALRM_Pos)

/** @} */ /* End of group MDR32VF0xI_BKP_RTC_ALRM */

/** @defgroup MDR32VF0xI_BKP_RTC_CS BKP RTC_CS
 * @{
 */

#define BKP_RTC_CS_OWF_Pos      0
#define BKP_RTC_CS_OWF_Msk      (0x1UL << BKP_RTC_CS_OWF_Pos)
#define BKP_RTC_CS_OWF          BKP_RTC_CS_OWF_Msk

#define BKP_RTC_CS_SECF_Pos     1
#define BKP_RTC_CS_SECF_Msk     (0x1UL << BKP_RTC_CS_SECF_Pos)
#define BKP_RTC_CS_SECF         BKP_RTC_CS_SECF_Msk

#define BKP_RTC_CS_WUTF_Pos     2
#define BKP_RTC_CS_WUTF_Msk     (0x1UL << BKP_RTC_CS_WUTF_Pos)
#define BKP_RTC_CS_WUTF         BKP_RTC_CS_WUTF_Msk

#define BKP_RTC_CS_OWF_IE_Pos   3
#define BKP_RTC_CS_OWF_IE_Msk   (0x1UL << BKP_RTC_CS_OWF_IE_Pos)
#define BKP_RTC_CS_OWF_IE       BKP_RTC_CS_OWF_IE_Msk

#define BKP_RTC_CS_SECF_IE_Pos  4
#define BKP_RTC_CS_SECF_IE_Msk  (0x1UL << BKP_RTC_CS_SECF_IE_Pos)
#define BKP_RTC_CS_SECF_IE      BKP_RTC_CS_SECF_IE_Msk

#define BKP_RTC_CS_WUTF_IE_Pos  5
#define BKP_RTC_CS_WUTF_IE_Msk  (0x1UL << BKP_RTC_CS_WUTF_IE_Pos)
#define BKP_RTC_CS_WUTF_IE      BKP_RTC_CS_WUTF_IE_Msk

#define BKP_RTC_CS_WEC_Pos      6
#define BKP_RTC_CS_WEC_Msk      (0x1UL << BKP_RTC_CS_WEC_Pos)
#define BKP_RTC_CS_WEC          BKP_RTC_CS_WEC_Msk

#define BKP_RTC_CS_ALRA_EN_Pos  7
#define BKP_RTC_CS_ALRA_EN_Msk  (0x1UL << BKP_RTC_CS_ALRA_EN_Pos)
#define BKP_RTC_CS_ALRA_EN      BKP_RTC_CS_ALRA_EN_Msk

#define BKP_RTC_CS_ALRB_EN_Pos  8
#define BKP_RTC_CS_ALRB_EN_Msk  (0x1UL << BKP_RTC_CS_ALRB_EN_Pos)
#define BKP_RTC_CS_ALRB_EN      BKP_RTC_CS_ALRB_EN_Msk

#define BKP_RTC_CS_ALRAF_Pos    9
#define BKP_RTC_CS_ALRAF_Msk    (0x1UL << BKP_RTC_CS_ALRAF_Pos)
#define BKP_RTC_CS_ALRAF        BKP_RTC_CS_ALRAF_Msk

#define BKP_RTC_CS_ALRBF_Pos    10
#define BKP_RTC_CS_ALRBF_Msk    (0x1UL << BKP_RTC_CS_ALRBF_Pos)
#define BKP_RTC_CS_ALRBF        BKP_RTC_CS_ALRBF_Msk

#define BKP_RTC_CS_ALRAF_IE_Pos 11
#define BKP_RTC_CS_ALRAF_IE_Msk (0x1UL << BKP_RTC_CS_ALRAF_IE_Pos)
#define BKP_RTC_CS_ALRAF_IE     BKP_RTC_CS_ALRAF_IE_Msk

#define BKP_RTC_CS_ALRBF_IE_Pos 12
#define BKP_RTC_CS_ALRBF_IE_Msk (0x1UL << BKP_RTC_CS_ALRBF_IE_Pos)
#define BKP_RTC_CS_ALRBF_IE     BKP_RTC_CS_ALRBF_IE_Msk

#define BKP_RTC_CS_TSF_Pos      13
#define BKP_RTC_CS_TSF_Msk      (0x1UL << BKP_RTC_CS_TSF_Pos)
#define BKP_RTC_CS_TSF          BKP_RTC_CS_TSF_Msk

/** @} */ /* End of group MDR32VF0xI_BKP_RTC_CS */

/** @defgroup MDR32VF0xI_BKP_RTC_TR BKP RTC_TR
 * @{
 */

#define BKP_RTC_TR_SU_Pos  0
#define BKP_RTC_TR_SU_Msk  (0xFUL << BKP_RTC_TR_SU_Pos)

#define BKP_RTC_TR_ST_Pos  4
#define BKP_RTC_TR_ST_Msk  (0x7UL << BKP_RTC_TR_ST_Pos)

#define BKP_RTC_TR_MNU_Pos 8
#define BKP_RTC_TR_MNU_Msk (0xFUL << BKP_RTC_TR_MNU_Pos)

#define BKP_RTC_TR_MNT_Pos 12
#define BKP_RTC_TR_MNT_Msk (0x7UL << BKP_RTC_TR_MNT_Pos)

#define BKP_RTC_TR_HU_Pos  16
#define BKP_RTC_TR_HU_Msk  (0xFUL << BKP_RTC_TR_HU_Pos)

#define BKP_RTC_TR_HT_Pos  20
#define BKP_RTC_TR_HT_Msk  (0x3UL << BKP_RTC_TR_HT_Pos)

#define BKP_RTC_TR_PM_Pos  22
#define BKP_RTC_TR_PM_Msk  (0x1UL << BKP_RTC_TR_PM_Pos)
#define BKP_RTC_TR_PM      BKP_RTC_TR_PM_Msk

/** @} */ /* End of group MDR32VF0xI_BKP_RTC_TR */

/** @defgroup MDR32VF0xI_BKP_RTC_DR BKP RTC_DR
 * @{
 */

#define BKP_RTC_DR_DU_Pos  0
#define BKP_RTC_DR_DU_Msk  (0xFUL << BKP_RTC_DR_DU_Pos)

#define BKP_RTC_DR_DT_Pos  4
#define BKP_RTC_DR_DT_Msk  (0x3UL << BKP_RTC_DR_DT_Pos)

#define BKP_RTC_DR_MU_Pos  8
#define BKP_RTC_DR_MU_Msk  (0xFUL << BKP_RTC_DR_MU_Pos)

#define BKP_RTC_DR_MT_Pos  12
#define BKP_RTC_DR_MT_Msk  (0x1UL << BKP_RTC_DR_MT_Pos)
#define BKP_RTC_DR_MT      BKP_RTC_DR_MT_Msk

#define BKP_RTC_DR_WDU_Pos 13
#define BKP_RTC_DR_WDU_Msk (0x7UL << BKP_RTC_DR_WDU_Pos)

#define BKP_RTC_DR_YU_Pos  16
#define BKP_RTC_DR_YU_Msk  (0xFUL << BKP_RTC_DR_YU_Pos)

#define BKP_RTC_DR_YT_Pos  20
#define BKP_RTC_DR_YT_Msk  (0xFUL << BKP_RTC_DR_YT_Pos)

/** @} */ /* End of group MDR32VF0xI_BKP_RTC_DR */

/** @defgroup MDR32VF0xI_BKP_RTC_ALARMxR BKP RTC_ALARMxR
 * @{
 */

#define BKP_RTC_ALARMxR_SU_Pos    0
#define BKP_RTC_ALARMxR_SU_Msk    (0xFUL << BKP_RTC_ALARMxR_SU_Pos)

#define BKP_RTC_ALARMxR_ST_Pos    4
#define BKP_RTC_ALARMxR_ST_Msk    (0x7UL << BKP_RTC_ALARMxR_ST_Pos)

#define BKP_RTC_ALARMxR_MSK1_Pos  7
#define BKP_RTC_ALARMxR_MSK1_Msk  (0x1UL << BKP_RTC_ALARMxR_MSK1_Pos)
#define BKP_RTC_ALARMxR_MSK1      BKP_RTC_ALARMxR_MSK1_Msk

#define BKP_RTC_ALARMxR_MNU_Pos   8
#define BKP_RTC_ALARMxR_MNU_Msk   (0xFUL << BKP_RTC_ALARMxR_MNU_Pos)

#define BKP_RTC_ALARMxR_MNT_Pos   12
#define BKP_RTC_ALARMxR_MNT_Msk   (0x7UL << BKP_RTC_ALARMxR_MNT_Pos)

#define BKP_RTC_ALARMxR_MSK2_Pos  15
#define BKP_RTC_ALARMxR_MSK2_Msk  (0x1UL << BKP_RTC_ALARMxR_MSK2_Pos)
#define BKP_RTC_ALARMxR_MSK2      BKP_RTC_ALARMxR_MSK2_Msk

#define BKP_RTC_ALARMxR_HU_Pos    16
#define BKP_RTC_ALARMxR_HU_Msk    (0xFUL << BKP_RTC_ALARMxR_HU_Pos)

#define BKP_RTC_ALARMxR_HT_Pos    20
#define BKP_RTC_ALARMxR_HT_Msk    (0x3UL << BKP_RTC_ALARMxR_HT_Pos)

#define BKP_RTC_ALARMxR_PM_Pos    22
#define BKP_RTC_ALARMxR_PM_Msk    (0x1UL << BKP_RTC_ALARMxR_PM_Pos)
#define BKP_RTC_ALARMxR_PM        BKP_RTC_ALARMxR_PM_Msk

#define BKP_RTC_ALARMxR_MSK3_Pos  23
#define BKP_RTC_ALARMxR_MSK3_Msk  (0x1UL << BKP_RTC_ALARMxR_MSK3_Pos)
#define BKP_RTC_ALARMxR_MSK3      BKP_RTC_ALARMxR_MSK3_Msk

#define BKP_RTC_ALARMxR_DU_Pos    24
#define BKP_RTC_ALARMxR_DU_Msk    (0xFUL << BKP_RTC_ALARMxR_DU_Pos)

#define BKP_RTC_ALARMxR_DT_Pos    28
#define BKP_RTC_ALARMxR_DT_Msk    (0x3UL << BKP_RTC_ALARMxR_DT_Pos)

#define BKP_RTC_ALARMxR_WDSEL_Pos 30
#define BKP_RTC_ALARMxR_WDSEL_Msk (0x1UL << BKP_RTC_ALARMxR_WDSEL_Pos)
#define BKP_RTC_ALARMxR_WDSEL     BKP_RTC_ALARMxR_WDSEL_Msk

#define BKP_RTC_ALARMxR_MSK4_Pos  31
#define BKP_RTC_ALARMxR_MSK4_Msk  (0x1UL << BKP_RTC_ALARMxR_MSK4_Pos)
#define BKP_RTC_ALARMxR_MSK4      BKP_RTC_ALARMxR_MSK4_Msk

/** @} */ /* End of group MDR32VF0xI_BKP_RTC_ALARMxR */

/** @defgroup MDR32VF0xI_BKP_RTC_TSTRx BKP RTC_TSTRx
 * @{
 */

#define BKP_RTC_TSTRx_SU_Pos  0
#define BKP_RTC_TSTRx_SU_Msk  (0xFUL << BKP_RTC_TSTRx_SU_Pos)

#define BKP_RTC_TSTRx_ST_Pos  4
#define BKP_RTC_TSTRx_ST_Msk  (0x7UL << BKP_RTC_TSTRx_ST_Pos)

#define BKP_RTC_TSTRx_MNU_Pos 8
#define BKP_RTC_TSTRx_MNU_Msk (0xFUL << BKP_RTC_TSTRx_MNU_Pos)

#define BKP_RTC_TSTRx_MNT_Pos 12
#define BKP_RTC_TSTRx_MNT_Msk (0x7UL << BKP_RTC_TSTRx_MNT_Pos)

#define BKP_RTC_TSTRx_HU_Pos  16
#define BKP_RTC_TSTRx_HU_Msk  (0xFUL << BKP_RTC_TSTRx_HU_Pos)

#define BKP_RTC_TSTRx_HT_Pos  20
#define BKP_RTC_TSTRx_HT_Msk  (0x3UL << BKP_RTC_TSTRx_HT_Pos)

#define BKP_RTC_TSTRx_PM_Pos  22
#define BKP_RTC_TSTRx_PM_Msk  (0x1UL << BKP_RTC_TSTRx_PM_Pos)
#define BKP_RTC_TSTRx_PM      BKP_RTC_TSTRx_PM_Msk

/** @} */ /* End of group MDR32VF0xI_BKP_RTC_TSTRx */

/** @defgroup MDR32VF0xI_BKP_RTC_TSDRx BKP RTC_TSDRx
 * @{
 */

#define BKP_RTC_TSDRx_DU_Pos  0
#define BKP_RTC_TSDRx_DU_Msk  (0xFUL << BKP_RTC_TSDRx_DU_Pos)

#define BKP_RTC_TSDRx_DT_Pos  4
#define BKP_RTC_TSDRx_DT_Msk  (0x3UL << BKP_RTC_TSDRx_DT_Pos)

#define BKP_RTC_TSDRx_MU_Pos  8
#define BKP_RTC_TSDRx_MU_Msk  (0xFUL << BKP_RTC_TSDRx_MU_Pos)

#define BKP_RTC_TSDRx_MT_Pos  12
#define BKP_RTC_TSDRx_MT_Msk  (0x1UL << BKP_RTC_TSDRx_MT_Pos)
#define BKP_RTC_TSDRx_MT      BKP_RTC_TSDRx_MT_Msk

#define BKP_RTC_TSDRx_WDU_Pos 13
#define BKP_RTC_TSDRx_WDU_Msk (0x7UL << BKP_RTC_TSDRx_WDU_Pos)

#define BKP_RTC_TSDRx_YU_Pos  16
#define BKP_RTC_TSDRx_YU_Msk  (0xFUL << BKP_RTC_TSDRx_YU_Pos)

#define BKP_RTC_TSDRx_YT_Pos  20
#define BKP_RTC_TSDRx_YT_Msk  (0xFUL << BKP_RTC_TSDRx_YT_Pos)

/** @} */ /* End of group MDR32VF0xI_BKP_RTC_TSDRx */

/** @defgroup MDR32VF0xI_BKP_RTC_TAFCR BKP RTC_TAFCR
 * @{
 */

#define BKP_RTC_TAFCR_TAMP_EN_Pos     0
#define BKP_RTC_TAFCR_TAMP_EN_Msk     (0x1UL << BKP_RTC_TAFCR_TAMP_EN_Pos)
#define BKP_RTC_TAFCR_TAMP_EN         BKP_RTC_TAFCR_TAMP_EN_Msk

#define BKP_RTC_TAFCR_TAMP_TRG_Pos    1
#define BKP_RTC_TAFCR_TAMP_TRG_Msk    (0x7UL << BKP_RTC_TAFCR_TAMP_TRG_Pos)
#define BKP_RTC_TAFCR_TAMP_TRG1       (0x1UL << BKP_RTC_TAFCR_TAMP_TRG_Pos)
#define BKP_RTC_TAFCR_TAMP_TRG2       (0x2UL << BKP_RTC_TAFCR_TAMP_TRG_Pos)
#define BKP_RTC_TAFCR_TAMP_TRG3       (0x4UL << BKP_RTC_TAFCR_TAMP_TRG_Pos)

#define BKP_RTC_TAFCR_TAMP_IE_Pos     4
#define BKP_RTC_TAFCR_TAMP_IE_Msk     (0x1UL << BKP_RTC_TAFCR_TAMP_IE_Pos)
#define BKP_RTC_TAFCR_TAMP_IE         BKP_RTC_TAFCR_TAMP_IE_Msk

#define BKP_RTC_TAFCR_TS_EN_Pos       5
#define BKP_RTC_TAFCR_TS_EN_Msk       (0x1UL << BKP_RTC_TAFCR_TS_EN_Pos)
#define BKP_RTC_TAFCR_TS_EN           BKP_RTC_TAFCR_TS_EN_Msk

#define BKP_RTC_TAFCR_TS_EDGE_Pos     6
#define BKP_RTC_TAFCR_TS_EDGE_Msk     (0x7UL << BKP_RTC_TAFCR_TS_EDGE_Pos)
#define BKP_RTC_TAFCR_TS_EDGE1        (0x1UL << BKP_RTC_TAFCR_TS_EDGE_Pos)
#define BKP_RTC_TAFCR_TS_EDGE2        (0x2UL << BKP_RTC_TAFCR_TS_EDGE_Pos)
#define BKP_RTC_TAFCR_TS_EDGE3        (0x4UL << BKP_RTC_TAFCR_TS_EDGE_Pos)

#define BKP_RTC_TAFCR_TS_IE_Pos       9
#define BKP_RTC_TAFCR_TS_IE_Msk       (0x1UL << BKP_RTC_TAFCR_TS_IE_Pos)
#define BKP_RTC_TAFCR_TS_IE           BKP_RTC_TAFCR_TS_IE_Msk

#define BKP_RTC_TAFCR_TAMP_IN_SEL_Pos 16
#define BKP_RTC_TAFCR_TAMP_IN_SEL_Msk (0x7UL << BKP_RTC_TAFCR_TAMP_IN_SEL_Pos)
#define BKP_RTC_TAFCR_TAMP_IN_SEL1    (0x1UL << BKP_RTC_TAFCR_TAMP_IN_SEL_Pos)
#define BKP_RTC_TAFCR_TAMP_IN_SEL2    (0x2UL << BKP_RTC_TAFCR_TAMP_IN_SEL_Pos)
#define BKP_RTC_TAFCR_TAMP_IN_SEL3    (0x4UL << BKP_RTC_TAFCR_TAMP_IN_SEL_Pos)

#define BKP_RTC_TAFCR_TS_IN_SEL_Pos   19
#define BKP_RTC_TAFCR_TS_IN_SEL_Msk   (0x7UL << BKP_RTC_TAFCR_TS_IN_SEL_Pos)
#define BKP_RTC_TAFCR_TS_IN_SEL1      (0x1UL << BKP_RTC_TAFCR_TS_IN_SEL_Pos)
#define BKP_RTC_TAFCR_TS_IN_SEL2      (0x2UL << BKP_RTC_TAFCR_TS_IN_SEL_Pos)
#define BKP_RTC_TAFCR_TS_IN_SEL3      (0x4UL << BKP_RTC_TAFCR_TS_IN_SEL_Pos)

#if defined(USE_MDR32F02_REV_2) || defined(USE_MDR1206FI) || defined(USE_MDR1206AFI) || defined(USE_MDR1206)

#define BKP_RTC_TAFCR_WAKE_SLEEP_Pos   22
#define BKP_RTC_TAFCR_WAKE_SLEEP_Msk   (0x1UL << BKP_RTC_TAFCR_WAKE_SLEEP_Pos)
#define BKP_RTC_TAFCR_WAKE_SLEEP       BKP_RTC_TAFCR_WAKE_SLEEP_Msk

#define BKP_RTC_TAFCR_TAMP_EN_W_Pos    23
#define BKP_RTC_TAFCR_TAMP_EN_W_Msk    (0x7UL << BKP_RTC_TAFCR_TAMP_EN_W_Pos)
#define BKP_RTC_TAFCR_TAMP_EN_W1       (0x1UL << BKP_RTC_TAFCR_TAMP_EN_W_Pos)
#define BKP_RTC_TAFCR_TAMP_EN_W2       (0x2UL << BKP_RTC_TAFCR_TAMP_EN_W_Pos)
#define BKP_RTC_TAFCR_TAMP_EN_W3       (0x4UL << BKP_RTC_TAFCR_TAMP_EN_W_Pos)

#define BKP_RTC_TAFCR_TAMP_EDGE_W_Pos  26
#define BKP_RTC_TAFCR_TAMP_EDGE_W_Msk  (0x7UL << BKP_RTC_TAFCR_TAMP_EDGE_W_Pos)
#define BKP_RTC_TAFCR_TAMP_EDGE_W1     (0x1UL << BKP_RTC_TAFCR_TAMP_EDGE_W_Pos)
#define BKP_RTC_TAFCR_TAMP_EDGE_W2     (0x2UL << BKP_RTC_TAFCR_TAMP_EDGE_W_Pos)
#define BKP_RTC_TAFCR_TAMP_EDGE_W3     (0x4UL << BKP_RTC_TAFCR_TAMP_EDGE_W_Pos)

#define BKP_RTC_TAFCR_TAMP_LEVEL_W_Pos 29
#define BKP_RTC_TAFCR_TAMP_LEVEL_W_Msk (0x7UL << BKP_RTC_TAFCR_TAMP_LEVEL_W_Pos)
#define BKP_RTC_TAFCR_TAMP_LEVEL_W1    (0x1UL << BKP_RTC_TAFCR_TAMP_LEVEL_W_Pos)
#define BKP_RTC_TAFCR_TAMP_LEVEL_W2    (0x2UL << BKP_RTC_TAFCR_TAMP_LEVEL_W_Pos)
#define BKP_RTC_TAFCR_TAMP_LEVEL_W3    (0x4UL << BKP_RTC_TAFCR_TAMP_LEVEL_W_Pos)

#endif

/** @} */ /* End of group MDR32VF0xI_BKP_RTC_TAFCR */

#if defined(USE_MDR1206FI) || defined(USE_MDR1206AFI) || defined(USE_MDR1206)

/** @defgroup MDR32VF0xI_BKP_RTC_TMPCAL1 BKP RTC_TMPCAL1
 * @{
 */

#define BKP_RTC_TMPCAL1_CONST0_Pos     0
#define BKP_RTC_TMPCAL1_CONST0_Msk     (0x1FUL << BKP_RTC_TMPCAL1_CONST0_Pos)

#define BKP_RTC_TMPCAL1_CONST1_Pos     5
#define BKP_RTC_TMPCAL1_CONST1_Msk     (0x1FUL << BKP_RTC_TMPCAL1_CONST1_Pos)

#define BKP_RTC_TMPCAL1_CONST2_Pos     10
#define BKP_RTC_TMPCAL1_CONST2_Msk     (0x1FUL << BKP_RTC_TMPCAL1_CONST2_Pos)

#define BKP_RTC_TMPCAL1_CONST3_Pos     15
#define BKP_RTC_TMPCAL1_CONST3_Msk     (0x1FUL << BKP_RTC_TMPCAL1_CONST3_Pos)

#define BKP_RTC_TMPCAL1_CONST4_Pos     20
#define BKP_RTC_TMPCAL1_CONST4_Msk     (0x1FUL << BKP_RTC_TMPCAL1_CONST4_Pos)

#define BKP_RTC_TMPCAL1_CONST5_Pos     25
#define BKP_RTC_TMPCAL1_CONST5_Msk     (0x1FUL << BKP_RTC_TMPCAL1_CONST5_Pos)

#define BKP_RTC_TMPCAL1_CONST6_LOW_Pos 30
#define BKP_RTC_TMPCAL1_CONST6_LOW_Msk (0x3UL << BKP_RTC_TMPCAL1_CONST6_LOW_Pos)

/** @} */ /* End of group MDR32VF0xI_BKP_RTC_TMPCAL1 */

/** @defgroup MDR32VF0xI_BKP_RTC_TMPCAL2 BKP RTC_TMPCAL2
 * @{
 */

#define BKP_RTC_TMPCAL2_CONST6_HIGH_Pos 0
#define BKP_RTC_TMPCAL2_CONST6_HIGH_Msk (0x7UL << BKP_RTC_TMPCAL2_CONST6_HIGH_Pos)

#define BKP_RTC_TMPCAL2_CONST7_Pos      3
#define BKP_RTC_TMPCAL2_CONST7_Msk      (0x1FUL << BKP_RTC_TMPCAL2_CONST7_Pos)

#define BKP_RTC_TMPCAL2_CONST8_Pos      8
#define BKP_RTC_TMPCAL2_CONST8_Msk      (0x1FUL << BKP_RTC_TMPCAL2_CONST8_Pos)

#define BKP_RTC_TMPCAL2_CONST9_Pos      13
#define BKP_RTC_TMPCAL2_CONST9_Msk      (0x1FUL << BKP_RTC_TMPCAL2_CONST9_Pos)

#define BKP_RTC_TMPCAL2_CONST10_Pos     18
#define BKP_RTC_TMPCAL2_CONST10_Msk     (0x1FUL << BKP_RTC_TMPCAL2_CONST10_Pos)

#define BKP_RTC_TMPCAL2_CONST11_Pos     23
#define BKP_RTC_TMPCAL2_CONST11_Msk     (0x1FUL << BKP_RTC_TMPCAL2_CONST11_Pos)

#define BKP_RTC_TMPCAL2_CONST12_LOW_Pos 28
#define BKP_RTC_TMPCAL2_CONST12_LOW_Msk (0xFUL << BKP_RTC_TMPCAL2_CONST12_LOW_Pos)

/** @} */ /* End of group MDR32VF0xI_BKP_RTC_TMPCAL2 */

/** @defgroup MDR32VF0xI_BKP_RTC_TMPCAL3 BKP RTC_TMPCAL3
 * @{
 */

#define BKP_RTC_TMPCAL3_SWCTR_INV_Pos       0
#define BKP_RTC_TMPCAL3_SWCTR_INV_Msk       (0x1UL << BKP_RTC_TMPCAL3_SWCTR_INV_Pos)
#define BKP_RTC_TMPCAL3_SWCTR_INV           BKP_RTC_TMPCAL3_SWCTR_INV_Msk

#define BKP_RTC_TMPCAL3_BLDO_TRIM_B2_Pos    2
#define BKP_RTC_TMPCAL3_BLDO_TRIM_B2_Msk    (0x1UL << BKP_RTC_TMPCAL3_BLDO_TRIM_B2_Pos)
#define BKP_RTC_TMPCAL3_BLDO_TRIM_B2        BKP_RTC_TMPCAL3_BLDO_TRIM_B2_Msk

#define BKP_RTC_TMPCAL3_LSE_CONF_B2_Pos     3
#define BKP_RTC_TMPCAL3_LSE_CONF_B2_Msk     (0x1UL << BKP_RTC_TMPCAL3_LSE_CONF_B2_Pos)
#define BKP_RTC_TMPCAL3_LSE_CONF_B2         BKP_RTC_TMPCAL3_LSE_CONF_B2_Msk

#define BKP_RTC_TMPCAL3_TEMP_EN_Pos         4
#define BKP_RTC_TMPCAL3_TEMP_EN_Msk         (0x1UL << BKP_RTC_TMPCAL3_TEMP_EN_Pos)
#define BKP_RTC_TMPCAL3_TEMP_EN             BKP_RTC_TMPCAL3_TEMP_EN_Msk

#define BKP_RTC_TMPCAL3_CONST12_HIGH_Pos    8
#define BKP_RTC_TMPCAL3_CONST12_HIGH_Msk    (0x1UL << BKP_RTC_TMPCAL3_CONST12_HIGH_Pos)
#define BKP_RTC_TMPCAL3_CONST12_HIGH        BKP_RTC_TMPCAL3_CONST12_HIGH_Msk

#define BKP_RTC_TMPCAL3_CONST13_Pos         9
#define BKP_RTC_TMPCAL3_CONST13_Msk         (0x1FUL << BKP_RTC_TMPCAL3_CONST13_Pos)

#define BKP_RTC_TMPCAL3_CONST14_Pos         14
#define BKP_RTC_TMPCAL3_CONST14_Msk         (0x1FUL << BKP_RTC_TMPCAL3_CONST14_Pos)

#define BKP_RTC_TMPCAL3_CONST15_Pos         19
#define BKP_RTC_TMPCAL3_CONST15_Msk         (0x1FUL << BKP_RTC_TMPCAL3_CONST15_Pos)

#define BKP_RTC_TMPCAL3_TEMP_RESULT_Pos     24
#define BKP_RTC_TMPCAL3_TEMP_RESULT_Msk     (0xFUL << BKP_RTC_TMPCAL3_TEMP_RESULT_Pos)

#define BKP_RTC_TMPCAL3_TEMP_RESULT_RDY_Pos 28
#define BKP_RTC_TMPCAL3_TEMP_RESULT_RDY_Msk (0x1UL << BKP_RTC_TMPCAL3_TEMP_RESULT_RDY_Pos)
#define BKP_RTC_TMPCAL3_TEMP_RESULT_RDY     BKP_RTC_TMPCAL3_TEMP_RESULT_RDY_Msk

/** @} */ /* End of group MDR32VF0xI_BKP_RTC_TMPCAL3 */

#endif

/** @} */ /* End of group MDR32VF0xI_BKP_Defines */

/** @} */ /* End of group MDR32VF0xI_BKP */

/** @defgroup MDR32VF0xI_ADCUI MDR_ADCUI
 * @{
 */

/**
 * @brief MDR32VF0xI ADCUI control structure
 */
typedef struct {
    __IO uint32_t CTRL1; /*!<ADCUI General Control1 Register */
    __IO uint32_t CTRL2; /*!<ADCUI General control2 Register */
    __IO uint32_t CTRL3; /*!<ADCUI General Control3 Register */

    __IO uint32_t F0CTR;   /*!<ADCUI Control Register for channel F0 */
    __IO uint32_t F0WC;    /*!<ADCUI Active Power Calculation Control Register for Channel F0 */
    __IO uint32_t F0WATTP; /*!<ADCUI Positive Active Power Value (bits [57..25]) Register for Channel F0 */
    __IO uint32_t F0WATTN; /*!<ADCUI Negative Active Power Value (bits [57..25]) Register for Channel F0 */
    __IO uint32_t F0VC;    /*!<ADCUI Reactive Power Calculation Control Register for Channel F0 */
    __IO uint32_t F0VARP;  /*!<ADCUI Positive Reactive Power Value (bits [57..25]) Register for Channel F0 */
    __IO uint32_t F0VARN;  /*!<ADCUI Negative Reactive Power Value (bits [57..25]) Register for Channel F0 */
    __IO uint32_t F0AC;    /*!<ADCUI Full Power Calculation Control Register for Channel F0 */
    __IO uint32_t F0VR;    /*!<ADCUI Full Power Value (bits [57..25]) Register for Channel F0 */
    __IO uint32_t F0MD0;   /*!<ADCUI Parameters0 Register for Channel F0 */
    __IO uint32_t F0MD1;   /*!<ADCUI Parameters1 Register for Channel F0 */
    __IO uint32_t F0VPEAK; /*!<ADCUI Register of Peak Value in Voltage Channel in Channel F0 */
    __IO uint32_t F0IPEAK; /*!<ADCUI Register of Peak Calue in Current Channel in Channel F0 */
    __IO uint32_t F0VDAT;  /*!<ADCUI Voltage Register V0 for Channel F0 */
    __IO uint32_t F0I0DAT; /*!<ADCUI Current Register I0 for Channel F0 */
    __IO uint32_t F0I3DAT; /*!<ADCUI Current Register I3 for Channel F0 */
    __IO uint32_t F0VRMS;  /*!<ADCUI RMS Value Channel Voltage F0 Register */
    __IO uint32_t F0VRMS2; /*!<ADCUI RMS Squared in F0 Voltage Channel Register */
    __IO uint32_t F0IRMS;  /*!<ADCUI RMS Current Channel F0 Register */
    __IO uint32_t F0IRMS2; /*!<ADCUI RMS Squared in Current Channel F0 Register */
    __IO uint32_t F0STAT;  /*!<ADCUI Channel status F0 Register */
    __IO uint32_t F0MASK;  /*!<ADCUI F0 Channel Interrupt Mask */

    __IO uint32_t F1CTR;   /*!<ADCUI Control Register for channel F1 */
    __IO uint32_t F1WC;    /*!<ADCUI Active Power Calculation Control Register for Channel F1 */
    __IO uint32_t F1WATTP; /*!<ADCUI Positive Active Power Value (bits [57..25]) Register for Channel F1 */
    __IO uint32_t F1WATTN; /*!<ADCUI Negative Active Power Value (bits [57..25]) Register for Channel F1 */
    __IO uint32_t F1VC;    /*!<ADCUI Reactive Power Calculation Control Register for Channel F1 */
    __IO uint32_t F1VARP;  /*!<ADCUI Positive Reactive Power Value (bits [57..25]) Register for Channel F1 */
    __IO uint32_t F1VARN;  /*!<ADCUI Negative Reactive Power Value (bits [57..25]) Register for Channel F1 */
    __IO uint32_t F1AC;    /*!<ADCUI Full Power Calculation Control Register for Channel F1 */
    __IO uint32_t F1VR;    /*!<ADCUI Full Power Value (bits [57..25]) Register for Channel F1 */
    __IO uint32_t F1MD0;   /*!<ADCUI Parameters0 Register for Channel F1 */
    __IO uint32_t F1MD1;   /*!<ADCUI Parameters1 Register for Channel F1 */
    __IO uint32_t F1MD2;   /*!<ADCUI Parameters2 Register for Channel F1 */
    __IO uint32_t F1VPEAK; /*!<ADCUI Register of Peak Value in Voltage Channel in Channel F1 */
    __IO uint32_t F1IPEAK; /*!<ADCUI Register of Peak Calue in Current Channel in Channel F1 */
    __IO uint32_t F1VDAT;  /*!<ADCUI Voltage Register V1 for Channel F1 */
    __IO uint32_t F1IDAT;  /*!<ADCUI Current Register I1 for Channel F1 */
    __IO uint32_t F1VRMS;  /*!<ADCUI RMS Value Channel Voltage F1 Register */
    __IO uint32_t F1VRMS2; /*!<ADCUI RMS Squared in F1 Voltage Channel Register */
    __IO uint32_t F1IRMS;  /*!<ADCUI RMS Current Channel F1 Register */
    __IO uint32_t F1IRMS2; /*!<ADCUI RMS Squared in Current Channel F1 Register */
    __IO uint32_t F1STAT;  /*!<ADCUI Channel status F1 Register */
    __IO uint32_t F1MASK;  /*!<ADCUI F1 Channel Interrupt Mask */

    __IO uint32_t F2CTR;   /*!<ADCUI Control Register for channel F2 */
    __IO uint32_t F2WC;    /*!<ADCUI Active Power Calculation Control Register for Channel F2 */
    __IO uint32_t F2WATTP; /*!<ADCUI Positive Active Power Value (bits [57..25]) Register for Channel F2 */
    __IO uint32_t F2WATTN; /*!<ADCUI Negative Active Power Value (bits [57..25]) Register for Channel F2 */
    __IO uint32_t F2VC;    /*!<ADCUI Reactive Power Calculation Control Register for Channel F2 */
    __IO uint32_t F2VARP;  /*!<ADCUI Positive Reactive Power Value (bits [57..25]) Register for Channel F2 */
    __IO uint32_t F2VARN;  /*!<ADCUI Negative Reactive Power Value (bits [57..25]) Register for Channel F2 */
    __IO uint32_t F2AC;    /*!<ADCUI Full Power Calculation Control Register for Channel F2 */
    __IO uint32_t F2VR;    /*!<ADCUI Full Power Value (bits [57..25]) Register for Channel F2 */
    __IO uint32_t F2MD0;   /*!<ADCUI Parameters0 Register for Channel F2 */
    __IO uint32_t F2MD1;   /*!<ADCUI Parameters1 Register for Channel F2 */
    __IO uint32_t F2MD2;   /*!<ADCUI Parameters2 Register for Channel F2 */
    __IO uint32_t F2VPEAK; /*!<ADCUI Register of Peak Value in Voltage Channel in Channel F2 */
    __IO uint32_t F2IPEAK; /*!<ADCUI Register of Peak Calue in Current Channel in Channel F2 */
    __IO uint32_t F2VDAT;  /*!<ADCUI Voltage Register V2 for Channel F1 */
    __IO uint32_t F2IDAT;  /*!<ADCUI Current Register I2 for Channel F1 */
    __IO uint32_t F2VRMS;  /*!<ADCUI RMS Value Channel Voltage F2 Register */
    __IO uint32_t F2VRMS2; /*!<ADCUI RMS Squared in F2 Voltage Channel Register */
    __IO uint32_t F2IRMS;  /*!<ADCUI RMS Current Channel F2 Register */
    __IO uint32_t F2IRMS2; /*!<ADCUI RMS Squared in Current Channel F2 Register */
    __IO uint32_t F2STAT;  /*!<ADCUI Channel status F2 Register */
    __IO uint32_t F2MASK;  /*!<ADCUI F2 Channel Interrupt Mask */

    __IO uint32_t CCAL1; /*!<ADCUI Current Channel Calibration Register 1 */
    __IO uint32_t CCAL2; /*!<ADCUI Current Channel Calibration Register 2 */
    __IO uint32_t CCAL3; /*!<ADCUI Current Channel Calibration Register 3 */
    __IO uint32_t CCAL4; /*!<ADCUI Current channel calibration Register 4 */

    __IO uint32_t F0IRMS_INACTIVE; /*!<ADCUI RMS Current Inactive Channel F0 Register */

    __IO uint32_t F0WATTP_L; /*!<ADCUI Positive Active Power Value (bits [24..0]) Register for Channel F0 */
    __IO uint32_t F0WATTN_L; /*!<ADCUI Negative Active Power Value (bits [24..0]) Register for Channel F0 */
    __IO uint32_t F0VARP_L;  /*!<ADCUI Positive Reactive Power Value (bits [24..0]) Register for Channel F0 */
    __IO uint32_t F0VARN_L;  /*!<ADCUI Negative Reactive Power Value (bits [24..0]) Register for Channel F0 */
    __IO uint32_t F0VR_L;    /*!<ADCUI Full Power Value (bits [24..0]) Register for Channel F0 */
    __IO uint32_t F1WATTP_L; /*!<ADCUI Positive Active Power Value (bits [24..0]) Register for Channel F1 */
    __IO uint32_t F1WATTN_L; /*!<ADCUI Negative Active Power Value (bits [24..0]) Register for Channel F1 */
    __IO uint32_t F1VARP_L;  /*!<ADCUI Positive Reactive Power Value (bits [24..0]) Register for Channel F1 */
    __IO uint32_t F1VARN_L;  /*!<ADCUI Negative Reactive Power Value (bits [24..0]) Register for Channel F1 */
    __IO uint32_t F1VR_L;    /*!<ADCUI Full Power Value (bits [24..0]) Register for Channel F1 */
    __IO uint32_t F2WATTP_L; /*!<ADCUI Positive Active Power Value (bits [24..0]) Register for Channel F2 */
    __IO uint32_t F2WATTN_L; /*!<ADCUI Negative Active Power Value (bits [24..0]) Register for Channel F2 */
    __IO uint32_t F2VARP_L;  /*!<ADCUI Positive Reactive Power Value (bits [24..0]) Register for Channel F2 */
    __IO uint32_t F2VARN_L;  /*!<ADCUI Negative Reactive Power Value (bits [24..0]) Register for Channel F2 */
    __IO uint32_t F2VR_L;    /*!<ADCUI Full Power Value (bits [24..0]) Register for Channel F2 */
#if defined(USE_MDR32F02_REV_2) || defined(USE_MDR1206FI) || defined(USE_MDR1206AFI) || defined(USE_MDR1206)
    __IO uint32_t LOAD; /*!<ADCUI General Control of NOLOAD Mode Register */

    __IO uint32_t F0VRMS_TRUE; /*!<ADCUI Effective Value of Voltage Channel on Period Register for Channel F0 */
    __IO uint32_t F1VRMS_TRUE; /*!<ADCUI Effective Value of Voltage Channel on Period Register for Channel F1 */
    __IO uint32_t F2VRMS_TRUE; /*!<ADCUI Effective Value of Voltage Channel on Period Register for Channel F2 */
#endif
#if defined(USE_MDR1206FI) || defined(USE_MDR1206AFI) || defined(USE_MDR1206)
    __IO uint32_t MSC; /*!<ADCUI Parameters Register for Channels Fx */

    __IO uint32_t FILTERCFG; /*!<ADCUI Filters Configuration Register */

    __IO uint32_t CLKPHASE1; /*!<ADCUI Clock and Phase Shift Configuration Register */
    __IO uint32_t CLKPHASE2; /*!<ADCUI Phase Shift Configuration Register */
#endif
} MDR_ADCUI_TypeDef;

/** @defgroup MDR32VF0xI_ADCUI_Defines ADCUI Defines
 * @{
 */

/** @defgroup MDR32VF0xI_ADCUI_CTRL1 ADCUI CTRL1
 * @{
 */

#define ADCUI_CTRL1_I0EN_Pos             0
#define ADCUI_CTRL1_I0EN_Msk             (0x1UL << ADCUI_CTRL1_I0EN_Pos)
#define ADCUI_CTRL1_I0EN                 ADCUI_CTRL1_I0EN_Msk

#define ADCUI_CTRL1_V0EN_Pos             1
#define ADCUI_CTRL1_V0EN_Msk             (0x1UL << ADCUI_CTRL1_V0EN_Pos)
#define ADCUI_CTRL1_V0EN                 ADCUI_CTRL1_V0EN_Msk

#define ADCUI_CTRL1_I1EN_Pos             2
#define ADCUI_CTRL1_I1EN_Msk             (0x1UL << ADCUI_CTRL1_I1EN_Pos)
#define ADCUI_CTRL1_I1EN                 ADCUI_CTRL1_I1EN_Msk

#define ADCUI_CTRL1_V1EN_Pos             3
#define ADCUI_CTRL1_V1EN_Msk             (0x1UL << ADCUI_CTRL1_V1EN_Pos)
#define ADCUI_CTRL1_V1EN                 ADCUI_CTRL1_V1EN_Msk

#define ADCUI_CTRL1_I2EN_Pos             4
#define ADCUI_CTRL1_I2EN_Msk             (0x1UL << ADCUI_CTRL1_I2EN_Pos)
#define ADCUI_CTRL1_I2EN                 ADCUI_CTRL1_I2EN_Msk

#define ADCUI_CTRL1_V2EN_Pos             5
#define ADCUI_CTRL1_V2EN_Msk             (0x1UL << ADCUI_CTRL1_V2EN_Pos)
#define ADCUI_CTRL1_V2EN                 ADCUI_CTRL1_V2EN_Msk

#define ADCUI_CTRL1_I3EN_Pos             6
#define ADCUI_CTRL1_I3EN_Msk             (0X1UL << ADCUI_CTRL1_I3EN_Pos)
#define ADCUI_CTRL1_I3EN                 ADCUI_CTRL1_I3EN_Msk

#define ADCUI_CTRL1_RESOL_Pos            7
#define ADCUI_CTRL1_RESOL_Msk            (0x1UL << ADCUI_CTRL1_RESOL_Pos)
#define ADCUI_CTRL1_RESOL                ADCUI_CTRL1_RESOL_Msk

#define ADCUI_CTRL1_ZXLPF_Pos            8
#define ADCUI_CTRL1_ZXLPF_Msk            (0x1UL << ADCUI_CTRL1_ZXLPF_Pos)
#define ADCUI_CTRL1_ZXLPF                ADCUI_CTRL1_ZXLPF_Msk

#define ADCUI_CTRL1_PER_LENGHT_Pos       9
#define ADCUI_CTRL1_PER_LENGHT_Msk       (0x7UL << ADCUI_CTRL1_PER_LENGHT_Pos)
#define ADCUI_CTRL1_PER_LENGHT_1         (0x0UL << ADCUI_CTRL1_PER_LENGHT_Pos)
#define ADCUI_CTRL1_PER_LENGHT_2         (0x1UL << ADCUI_CTRL1_PER_LENGHT_Pos)
#define ADCUI_CTRL1_PER_LENGHT_4         (0x2UL << ADCUI_CTRL1_PER_LENGHT_Pos)
#define ADCUI_CTRL1_PER_LENGHT_8         (0x3UL << ADCUI_CTRL1_PER_LENGHT_Pos)
#define ADCUI_CTRL1_PER_LENGHT_16        (0x4UL << ADCUI_CTRL1_PER_LENGHT_Pos)
#define ADCUI_CTRL1_PER_LENGHT_32        (0x5UL << ADCUI_CTRL1_PER_LENGHT_Pos)
#define ADCUI_CTRL1_PER_LENGHT_64        (0x6UL << ADCUI_CTRL1_PER_LENGHT_Pos)
#define ADCUI_CTRL1_PER_LENGHT_128       (0x7UL << ADCUI_CTRL1_PER_LENGHT_Pos)

#define ADCUI_CTRL1_APNOLOAD_Pos         12
#define ADCUI_CTRL1_APNOLOAD_Msk         (0x3UL << ADCUI_CTRL1_APNOLOAD_Pos)
#define ADCUI_CTRL1_APNOLOAD_FULL        (0x0UL << ADCUI_CTRL1_APNOLOAD_Pos)
#define ADCUI_CTRL1_APNOLOAD_0p012pct    (0x1UL << ADCUI_CTRL1_APNOLOAD_Pos)
#define ADCUI_CTRL1_APNOLOAD_0p0061pct   (0x2UL << ADCUI_CTRL1_APNOLOAD_Pos)
#define ADCUI_CTRL1_APNOLOAD_0p00305pct  (0x3UL << ADCUI_CTRL1_APNOLOAD_Pos)

#define ADCUI_CTRL1_VARNOLOAD_Pos        15
#define ADCUI_CTRL1_VARNOLOAD_Msk        (0x3UL << ADCUI_CTRL1_VARNOLOAD_Pos)
#define ADCUI_CTRL1_VARNOLOAD_FULL       (0x0UL << ADCUI_CTRL1_VARNOLOAD_Pos)
#define ADCUI_CTRL1_VARNOLOAD_0p012pct   (0x1UL << ADCUI_CTRL1_VARNOLOAD_Pos)
#define ADCUI_CTRL1_VARNOLOAD_0p0061pct  (0x2UL << ADCUI_CTRL1_VARNOLOAD_Pos)
#define ADCUI_CTRL1_VARNOLOAD_0p00305pct (0x3UL << ADCUI_CTRL1_VARNOLOAD_Pos)

#define ADCUI_CTRL1_VANOLOAD_Pos         17
#define ADCUI_CTRL1_VANOLOAD_Msk         (0x3UL << ADCUI_CTRL1_VANOLOAD_Pos)
#define ADCUI_CTRL1_VANOLOAD_FULL        (0x0UL << ADCUI_CTRL1_VANOLOAD_Pos)
#define ADCUI_CTRL1_VANOLOAD_0p012pct    (0x1UL << ADCUI_CTRL1_VANOLOAD_Pos)
#define ADCUI_CTRL1_VANOLOAD_0p0061pct   (0x2UL << ADCUI_CTRL1_VANOLOAD_Pos)
#define ADCUI_CTRL1_VANOLOAD_0p00305pct  (0x3UL << ADCUI_CTRL1_VANOLOAD_Pos)

#define ADCUI_CTRL1_FREQSEL_Pos          19
#define ADCUI_CTRL1_FREQSEL_Msk          (0x1UL << ADCUI_CTRL1_FREQSEL_Pos)
#define ADCUI_CTRL1_FREQSEL              ADCUI_CTRL1_FREQSEL_Msk

#define ADCUI_CTRL1_VREF_SEL_Pos         20
#define ADCUI_CTRL1_VREF_SEL_Msk         (0x1UL << ADCUI_CTRL1_VREF_SEL_Pos)
#define ADCUI_CTRL1_VREF_SEL             ADCUI_CTRL1_VREF_SEL_Msk

#define ADCUI_CTRL1_TST_IN0_Pos          21
#define ADCUI_CTRL1_TST_IN0_Msk          (0x1UL << ADCUI_CTRL1_TST_IN0_Pos)
#define ADCUI_CTRL1_TST_IN0              ADCUI_CTRL1_TST_IN0_Msk

#define ADCUI_CTRL1_TST_IN1_Pos          22
#define ADCUI_CTRL1_TST_IN1_Msk          (0x1UL << ADCUI_CTRL1_TST_IN1_Pos)
#define ADCUI_CTRL1_TST_IN1              ADCUI_CTRL1_TST_IN1_Msk

#define ADCUI_CTRL1_TST_IN32_Pos         23
#define ADCUI_CTRL1_TST_IN32_Msk         (0x3UL << ADCUI_CTRL1_TST_IN32_Pos)

#define ADCUI_CTRL1_ZXRMS_Pos            27
#define ADCUI_CTRL1_ZXRMS_Msk            (0x1UL << ADCUI_CTRL1_ZXRMS_Pos)
#define ADCUI_CTRL1_ZXRMS                ADCUI_CTRL1_ZXRMS_Msk

#define ADCUI_CTRL1_RESET_DIG_Pos        28
#define ADCUI_CTRL1_RESET_DIG_Msk        (0x1UL << ADCUI_CTRL1_RESET_DIG_Pos)
#define ADCUI_CTRL1_RESET_DIG            ADCUI_CTRL1_RESET_DIG_Msk

#define ADCUI_CTRL1_TST_IN4_Pos          29
#define ADCUI_CTRL1_TST_IN4_Msk          (0x1UL << ADCUI_CTRL1_TST_IN4_Pos)
#define ADCUI_CTRL1_TST_IN4              ADCUI_CTRL1_TST_IN4_Msk

#define ADCUI_CTRL1_OSR_CONF_Pos         30
#define ADCUI_CTRL1_OSR_CONF_Msk         (0x3UL << ADCUI_CTRL1_OSR_CONF_Pos)
#if defined(USE_MDR32F02)
#define ADCUI_CTRL1_OSR_CONF_256_4kHz (0x0UL << ADCUI_CTRL1_OSR_CONF_Pos)
#define ADCUI_CTRL1_OSR_CONF_128_8kHz (0x1UL << ADCUI_CTRL1_OSR_CONF_Pos)
#define ADCUI_CTRL1_OSR_CONF_64_16kHz (0x2UL << ADCUI_CTRL1_OSR_CONF_Pos)
#elif defined(USE_MDR1206FI) || defined(USE_MDR1206AFI) || defined(USE_MDR1206)
#define ADCUI_CTRL1_OSR_CONF_1024 (0x0UL << ADCUI_CTRL1_OSR_CONF_Pos)
#define ADCUI_CTRL1_OSR_CONF_512  (0x1UL << ADCUI_CTRL1_OSR_CONF_Pos)
#define ADCUI_CTRL1_OSR_CONF_256  (0x2UL << ADCUI_CTRL1_OSR_CONF_Pos)
#endif

/** @} */ /* End of group MDR32VF0xI_ADCUI_CTRL1 */

/** @defgroup MDR32VF0xI_ADCUI_CTRL2 ADCUI CTRL2
 * @{
 */

#define ADCUI_CTRL2_SAGLVL_Pos 0
#define ADCUI_CTRL2_SAGLVL_Msk (0xFFFFUL << ADCUI_CTRL2_SAGLVL_Pos)

#define ADCUI_CTRL2_SAGCYC_Pos 16
#define ADCUI_CTRL2_SAGCYC_Msk (0xFFUL << ADCUI_CTRL2_SAGCYC_Pos)

#if defined(USE_MDR1206FI) || defined(USE_MDR1206AFI) || defined(USE_MDR1206)

#define ADCUI_CTRL2_DCHPENV0_Pos 24
#define ADCUI_CTRL2_DCHPENV0_Msk (0x1UL << ADCUI_CTRL2_DCHPENV0_Pos)
#define ADCUI_CTRL2_DCHPENV0     ADCUI_CTRL2_DCHPENV0_Msk

#define ADCUI_CTRL2_DCHPENV1_Pos 25
#define ADCUI_CTRL2_DCHPENV1_Msk (0x1UL << ADCUI_CTRL2_DCHPENV1_Pos)
#define ADCUI_CTRL2_DCHPENV1     ADCUI_CTRL2_DCHPENV1_Msk

#define ADCUI_CTRL2_DCHPENV2_Pos 26
#define ADCUI_CTRL2_DCHPENV2_Msk (0x1UL << ADCUI_CTRL2_DCHPENV2_Pos)
#define ADCUI_CTRL2_DCHPENV2     ADCUI_CTRL2_DCHPENV2_Msk

#define ADCUI_CTRL2_DCHPENI0_Pos 27
#define ADCUI_CTRL2_DCHPENI0_Msk (0x1UL << ADCUI_CTRL2_DCHPENI0_Pos)
#define ADCUI_CTRL2_DCHPENI0     ADCUI_CTRL2_DCHPENI0_Msk

#define ADCUI_CTRL2_DCHPENI1_Pos 28
#define ADCUI_CTRL2_DCHPENI1_Msk (0x1UL << ADCUI_CTRL2_DCHPENI1_Pos)
#define ADCUI_CTRL2_DCHPENI1     ADCUI_CTRL2_DCHPENI1_Msk

#define ADCUI_CTRL2_DCHPENI2_Pos 29
#define ADCUI_CTRL2_DCHPENI2_Msk (0x1UL << ADCUI_CTRL2_DCHPENI2_Pos)
#define ADCUI_CTRL2_DCHPENI2     ADCUI_CTRL2_DCHPENI2_Msk

#define ADCUI_CTRL2_DCHPENI3_Pos 30
#define ADCUI_CTRL2_DCHPENI3_Msk (0x1UL << ADCUI_CTRL2_DCHPENI3_Pos)
#define ADCUI_CTRL2_DCHPENI3     ADCUI_CTRL2_DCHPENI3_Msk

#endif

/** @} */ /* End of group MDR32VF0xI_ADCUI_CTRL2 */

/** @defgroup MDR32VF0xI_ADCUI_CTRL3 ADCUI CTRL3
 * @{
 */

#define ADCUI_CTRL3_ZXTOUT_Pos 0
#define ADCUI_CTRL3_ZXTOUT_Msk (0xFFFUL << ADCUI_CTRL3_ZXTOUT_Pos)

/** @} */ /* End of group MDR32VF0xI_ADCUI_CTRL3 */

/** @defgroup MDR32VF0xI_ADCUI_F0CTR ADCUI F0CTR for Channel 0
 * @{
 */

#define ADCUI_F0CTR_F0I0NTEN_Pos 0
#define ADCUI_F0CTR_F0I0NTEN_Msk (0x1UL << ADCUI_F0CTR_F0I0NTEN_Pos)
#define ADCUI_F0CTR_F0I0NTEN     ADCUI_F0CTR_F0I0NTEN_Msk

#define ADCUI_F0CTR_F0I3NTEN_Pos 1
#define ADCUI_F0CTR_F0I3NTEN_Msk (0x1UL << ADCUI_F0CTR_F0I3NTEN_Pos)
#define ADCUI_F0CTR_F0I3NTEN     ADCUI_F0CTR_F0I3NTEN_Msk

#define ADCUI_F0CTR_F0VASEL_Pos  2
#define ADCUI_F0CTR_F0VASEL_Msk  (0x1UL << ADCUI_F0CTR_F0VASEL_Pos)
#define ADCUI_F0CTR_F0VASEL      ADCUI_F0CTR_F0VASEL_Msk

#define ADCUI_F0CTR_F0RARS_Pos   3
#define ADCUI_F0CTR_F0RARS_Msk   (0x1UL << ADCUI_F0CTR_F0RARS_Pos)
#define ADCUI_F0CTR_F0RARS       ADCUI_F0CTR_F0RARS_Msk

#define ADCUI_F0CTR_F0RRRS_Pos   4
#define ADCUI_F0CTR_F0RRRS_Msk   (0x1UL << ADCUI_F0CTR_F0RRRS_Pos)
#define ADCUI_F0CTR_F0RRRS       ADCUI_F0CTR_F0RRRS_Msk

#define ADCUI_F0CTR_F0RVRS_Pos   5
#define ADCUI_F0CTR_F0RVRS_Msk   (0x1UL << ADCUI_F0CTR_F0RVRS_Pos)
#define ADCUI_F0CTR_F0RVRS       ADCUI_F0CTR_F0RVRS_Msk

#if defined(USE_MDR1206FI) || defined(USE_MDR1206AFI) || defined(USE_MDR1206)
#define ADCUI_F0CTR_F0I0GAIN_Pos  6
#define ADCUI_F0CTR_F0I0GAIN_Msk  (0x3UL << ADCUI_F0CTR_F0I0GAIN_Pos)
#define ADCUI_F0CTR_F0I0GAIN_6dB  (0x0UL << ADCUI_F0CTR_F0I0GAIN_Pos)
#define ADCUI_F0CTR_F0I0GAIN_12dB (0x1UL << ADCUI_F0CTR_F0I0GAIN_Pos)
#define ADCUI_F0CTR_F0I0GAIN_18dB (0x2UL << ADCUI_F0CTR_F0I0GAIN_Pos)
#define ADCUI_F0CTR_F0I0GAIN_24dB (0x3UL << ADCUI_F0CTR_F0I0GAIN_Pos)

#define ADCUI_F0CTR_F0VGAIN_Pos   8
#define ADCUI_F0CTR_F0VGAIN_Msk   (0x3UL << ADCUI_F0CTR_F0VGAIN_Pos)
#define ADCUI_F0CTR_F0VGAIN_0dB   (0x0UL << ADCUI_F0CTR_F0VGAIN_Pos)
#define ADCUI_F0CTR_F0VGAIN_6dB   (0x1UL << ADCUI_F0CTR_F0VGAIN_Pos)
#define ADCUI_F0CTR_F0VGAIN_12dB  (0x2UL << ADCUI_F0CTR_F0VGAIN_Pos)
#define ADCUI_F0CTR_F0VGAIN_18dB  (0x3UL << ADCUI_F0CTR_F0VGAIN_Pos)
#endif

#if defined(USE_MDR32F02_REV_1X) || defined(USE_MDR32F02_REV_2)
#define ADCUI_F0CTR_F0VGAIN_Pos   6
#define ADCUI_F0CTR_F0VGAIN_Msk   (0x3UL << ADCUI_F0CTR_F0VGAIN_Pos)
#define ADCUI_F0CTR_F0VGAIN_0dB   (0x0UL << ADCUI_F0CTR_F0VGAIN_Pos)
#define ADCUI_F0CTR_F0VGAIN_6dB   (0x1UL << ADCUI_F0CTR_F0VGAIN_Pos)
#define ADCUI_F0CTR_F0VGAIN_12dB  (0x2UL << ADCUI_F0CTR_F0VGAIN_Pos)
#define ADCUI_F0CTR_F0VGAIN_18dB  (0x3UL << ADCUI_F0CTR_F0VGAIN_Pos)

#define ADCUI_F0CTR_F0I0GAIN_Pos  8
#define ADCUI_F0CTR_F0I0GAIN_Msk  (0x3UL << ADCUI_F0CTR_F0I0GAIN_Pos)
#define ADCUI_F0CTR_F0I0GAIN_0dB  (0x0UL << ADCUI_F0CTR_F0I0GAIN_Pos)
#define ADCUI_F0CTR_F0I0GAIN_6dB  (0x1UL << ADCUI_F0CTR_F0I0GAIN_Pos)
#define ADCUI_F0CTR_F0I0GAIN_12dB (0x2UL << ADCUI_F0CTR_F0I0GAIN_Pos)
#define ADCUI_F0CTR_F0I0GAIN_18dB (0x3UL << ADCUI_F0CTR_F0I0GAIN_Pos)
#endif

#define ADCUI_F0CTR_F0VPHASE_Pos 10
#define ADCUI_F0CTR_F0VPHASE_Msk (0xFFUL << ADCUI_F0CTR_F0VPHASE_Pos)

#if defined(USE_MDR1206FI) || defined(USE_MDR1206AFI) || defined(USE_MDR1206)
#define ADCUI_F0CTR_F0I3GAIN_Pos  18
#define ADCUI_F0CTR_F0I3GAIN_Msk  (0x3UL << ADCUI_F0CTR_F0I3GAIN_Pos)
#define ADCUI_F0CTR_F0I3GAIN_6dB  (0x0UL << ADCUI_F0CTR_F0I3GAIN_Pos)
#define ADCUI_F0CTR_F0I3GAIN_12dB (0x1UL << ADCUI_F0CTR_F0I3GAIN_Pos)
#define ADCUI_F0CTR_F0I3GAIN_18dB (0x2UL << ADCUI_F0CTR_F0I3GAIN_Pos)
#define ADCUI_F0CTR_F0I3GAIN_24dB (0x3UL << ADCUI_F0CTR_F0I3GAIN_Pos)
#endif

#if defined(USE_MDR32F02_REV_1X) || defined(USE_MDR32F02_REV_2)
#define ADCUI_F0CTR_F0I3GAIN_Pos  18
#define ADCUI_F0CTR_F0I3GAIN_Msk  (0x3UL << ADCUI_F0CTR_F0I3GAIN_Pos)
#define ADCUI_F0CTR_F0I3GAIN_0dB  (0x0UL << ADCUI_F0CTR_F0I3GAIN_Pos)
#define ADCUI_F0CTR_F0I3GAIN_6dB  (0x1UL << ADCUI_F0CTR_F0I3GAIN_Pos)
#define ADCUI_F0CTR_F0I3GAIN_12dB (0x2UL << ADCUI_F0CTR_F0I3GAIN_Pos)
#define ADCUI_F0CTR_F0I3GAIN_18dB (0x3UL << ADCUI_F0CTR_F0I3GAIN_Pos)
#endif

#define ADCUI_F0CTR_F0IRMSOS_Pos 20
#define ADCUI_F0CTR_F0IRMSOS_Msk (0xFFFUL << ADCUI_F0CTR_F0IRMSOS_Pos)

/** @} */ /* End of group MDR32VF0xI_ADCUI_F0CTR */

/** @defgroup MDR32VF0xI_ADCUI_FxCTR ADCUI FxCTR for Channels 1 and 2
 * @{
 */

#define ADCUI_FxCTR_FxINTEN_Pos  0
#define ADCUI_FxCTR_FxINTEN_Msk  (0x1UL << ADCUI_FxCTR_FxINTEN_Pos)
#define ADCUI_FxCTR_FxINTEN      ADCUI_FxCTR_FxINTEN_Msk

#define ADCUI_FxCTR_FxVASEL_Pos  2
#define ADCUI_FxCTR_FxVASEL_Msk  (0x1UL << ADCUI_FxCTR_FxVASEL_Pos)
#define ADCUI_FxCTR_FxVASEL      ADCUI_FxCTR_FxVASEL_Msk

#define ADCUI_FxCTR_FxRARS_Pos   3
#define ADCUI_FxCTR_FxRARS_Msk   (0x1UL << ADCUI_FxCTR_FxRARS_Pos)
#define ADCUI_FxCTR_FxRARS       ADCUI_FxCTR_FxRARS_Msk

#define ADCUI_FxCTR_FxRRRS_Pos   4
#define ADCUI_FxCTR_FxRRRS_Msk   (0x1UL << ADCUI_FxCTR_FxRRRS_Pos)
#define ADCUI_FxCTR_FxRRRS       ADCUI_FxCTR_FxRRRS_Msk

#define ADCUI_FxCTR_FxRVRS_Pos   5
#define ADCUI_FxCTR_FxRVRS_Msk   (0x1UL << ADCUI_FxCTR_FxRVRS_Pos)
#define ADCUI_FxCTR_FxRVRS       ADCUI_FxCTR_FxRVRS_Msk

#define ADCUI_FxCTR_FxIGAIN_Pos  6
#define ADCUI_FxCTR_FxIGAIN_Msk  (0x3UL << ADCUI_FxCTR_FxIGAIN_Pos)
#define ADCUI_FxCTR_FxIGAIN_6dB  (0x0UL << ADCUI_FxCTR_FxIGAIN_Pos)
#define ADCUI_FxCTR_FxIGAIN_12dB (0x1UL << ADCUI_FxCTR_FxIGAIN_Pos)
#define ADCUI_FxCTR_FxIGAIN_18dB (0x2UL << ADCUI_FxCTR_FxIGAIN_Pos)
#define ADCUI_FxCTR_FxIGAIN_24dB (0x3UL << ADCUI_FxCTR_FxIGAIN_Pos)

#define ADCUI_FxCTR_FxVGAIN_Pos  8
#define ADCUI_FxCTR_FxVGAIN_Msk  (0x3UL << ADCUI_FxCTR_FxVGAIN_Pos)
#define ADCUI_FxCTR_FxVGAIN_0dB  (0x0UL << ADCUI_FxCTR_FxVGAIN_Pos)
#define ADCUI_FxCTR_FxVGAIN_6dB  (0x1UL << ADCUI_FxCTR_FxVGAIN_Pos)
#define ADCUI_FxCTR_FxVGAIN_12dB (0x2UL << ADCUI_FxCTR_FxVGAIN_Pos)
#define ADCUI_FxCTR_FxVGAIN_18dB (0x3UL << ADCUI_FxCTR_FxVGAIN_Pos)

#define ADCUI_FxCTR_FxVPHASE_Pos 10
#define ADCUI_FxCTR_FxVPHASE_Msk (0xFFUL << ADCUI_FxCTR_FxVPHASE_Pos)

#if defined(USE_MDR1206FI) || defined(USE_MDR1206AFI) || defined(USE_MDR1206)
#define ADCUI_FxCTR_FxVPHASE_LOW_Pos 18
#define ADCUI_FxCTR_FxVPHASE_LOW_Msk (0x3UL << ADCUI_FxCTR_FxVPHASE_LOW_Pos)
#endif

#define ADCUI_FxCTR_FxIRMSOS_Pos 20
#define ADCUI_FxCTR_FxIRMSOS_Msk (0xFFFUL << ADCUI_FxCTR_FxIRMSOS_Pos)

/** @} */ /* End of group MDR32VF0xI_ADCUI_FxCTR */

/** @defgroup MDR32VF0xI_ADCUI_FxWC ADCUI FxWC
 * @{
 */

#define ADCUI_FxWC_FxWATTOS_Pos 0
#define ADCUI_FxWC_FxWATTOS_Msk (0xFFFFUL << ADCUI_FxWC_FxWATTOS_Pos)

#define ADCUI_FxWC_FxWGAIN_Pos  16
#define ADCUI_FxWC_FxWGAIN_Msk  (0xFFFUL << ADCUI_FxWC_FxWGAIN_Pos)

/** @} */ /* End of group MDR32VF0xI_ADCUI_FxWC */

/** @defgroup MDR32VF0xI_ADCUI_FxWATTP ADCUI FxWATTP
 * @{
 */

#define ADCUI_FxWATTP_FxWATTHRP_Pos 0
#define ADCUI_FxWATTP_FxWATTHRP_Msk (0xFFFFFFFFUL << ADCUI_FxWATTP_FxWATTHRP_Pos)

/** @} */ /* End of group MDR32VF0xI_ADCUI_FxWATTP */

/** @defgroup MDR32VF0xI_ADCUI_FxWATTN ADCUI FxWATTN
 * @{
 */

#define ADCUI_FxWATTN_FxWATTHRN_Pos 0
#define ADCUI_FxWATTN_FxWATTHRN_Msk (0xFFFFFFFFUL << ADCUI_FxWATTN_FxWATTHRN_Pos)

/** @} */ /* End of group MDR32VF0xI_ADCUI_FxWATTN */

/** @defgroup MDR32VF0xI_ADCUI_FxVC ADCUI FxVC
 * @{
 */

#define ADCUI_FxVC_FxVAROS_Pos   0
#define ADCUI_FxVC_FxVAROS_Msk   (0xFFFFUL << ADCUI_FxVC_FxVAROS_Pos)

#define ADCUI_FxVC_FxVARGAIN_Pos 16
#define ADCUI_FxVC_FxVARGAIN_Msk (0xFFFUL << ADCUI_FxVC_FxVARGAIN_Pos)

/** @} */ /* End of group MDR32VF0xI_ADCUI_FxVC */

/** @defgroup MDR32VF0xI_ADCUI_FxVARP ADCUI FxVARP
 * @{
 */

#define ADCUI_FxVARP_FxVARHRP_Pos 0
#define ADCUI_FxVARP_FxVARHRP_Msk (0xFFFFFFFFUL << ADCUI_FxVARP_FxVARHRP_Pos)

/** @} */ /* End of group MDR32VF0xI_ADCUI_FxVARP */

/** @defgroup MDR32VF0xI_ADCUI_FxVARN ADCUI FxVARN
 * @{
 */

#define ADCUI_FxVARN_FxVARHRN_Pos 0
#define ADCUI_FxVARN_FxVARHRN_Msk (0xFFFFFFFFUL << ADCUI_FxVARN_FxVARHRN_Pos)

/** @} */ /* End of group MDR32VF0xI_ADCUI_FxVARN */

/** @defgroup MDR32VF0xI_ADCUI_F0AC ADCUI F0AC for Channel 0
 * @{
 */

#define ADCUI_F0AC_F0VRMSOS_Pos 0
#define ADCUI_F0AC_F0VRMSOS_Msk (0xFFFUL << ADCUI_F0AC_F0VRMSOS_Pos)

#define ADCUI_F0AC_F0I3SEL_Pos  12
#define ADCUI_F0AC_F0I3SEL_Msk  (0x1UL << ADCUI_F0AC_F0I3SEL_Pos)
#define ADCUI_F0AC_F0I3SEL      ADCUI_F0AC_F0I3SEL_Msk

#define ADCUI_F0AC_F0VAGAIN_Pos 16
#define ADCUI_F0AC_F0VAGAIN_Msk (0xFFFUL << ADCUI_F0AC_F0VAGAIN_Pos)

/** @} */ /* End of group MDR32VF0xI_ADCUI_F0AC */

/** @defgroup MDR32VF0xI_ADCUI_FxAC ADCUI FxAC for Channels 1 and 2
 * @{
 */

#define ADCUI_FxAC_FxVRMSOS_Pos 0
#define ADCUI_FxAC_FxVRMSOS_Msk (0xFFFUL << ADCUI_FxAC_FxVRMSOS_Pos)

#define ADCUI_FxAC_FxVAGAIN_Pos 16
#define ADCUI_FxAC_FxVAGAIN_Msk (0xFFFUL << ADCUI_FxAC_FxVAGAIN_Pos)

/** @} */ /* End of group MDR32VF0xI_ADCUI_FxAC */

/** @defgroup MDR32VF0xI_ADCUI_FxVR ADCUI FxVR
 * @{
 */

#define ADCUI_FxVR_FxVAHR_Pos 0
#define ADCUI_FxVR_FxVAHR_Msk (0xFFFFFFFFUL << ADCUI_FxVR_FxVAHR_Pos)

/** @} */ /* End of group MDR32VF0xI_ADCUI_FxVR */

/** @defgroup MDR32VF0xI_ADCUI_F0MD0 ADCUI F0MD0 for Channel 0
 * @{
 */

#define ADCUI_F0MD0_F0VSEL_Pos           0
#define ADCUI_F0MD0_F0VSEL_Msk           (0x3UL << ADCUI_F0MD0_F0VSEL_Pos)
#define ADCUI_F0MD0_F0VSEL_V_BEFORE_HPF  (0x0UL << ADCUI_F0MD0_F0VSEL_Pos)
#define ADCUI_F0MD0_F0VSEL_ACTIVEPOWER   (0x1UL << ADCUI_F0MD0_F0VSEL_Pos)
#define ADCUI_F0MD0_F0VSEL_REACTIVEPOWER (0x2UL << ADCUI_F0MD0_F0VSEL_Pos)
#define ADCUI_F0MD0_F0VSEL_V_AFTER_HPF   (0x3UL << ADCUI_F0MD0_F0VSEL_Pos)

#define ADCUI_F0MD0_F0ISEL_Pos           2
#define ADCUI_F0MD0_F0ISEL_Msk           (0x3UL << ADCUI_F0MD0_F0ISEL_Pos)
#define ADCUI_F0MD0_F0ISEL_I             (0x0UL << ADCUI_F0MD0_F0ISEL_Pos)
#define ADCUI_F0MD0_F0ISEL_ACTIVEPOWER   (0x1UL << ADCUI_F0MD0_F0ISEL_Pos)
#define ADCUI_F0MD0_F0ISEL_REACTIVEPOWER (0x2UL << ADCUI_F0MD0_F0ISEL_Pos)
#define ADCUI_F0MD0_F0ISEL_FULLPOWER     (0x3UL << ADCUI_F0MD0_F0ISEL_Pos)

#define ADCUI_F0MD0_F0ACTS_Pos           4
#define ADCUI_F0MD0_F0ACTS_Msk           (0x1UL << ADCUI_F0MD0_F0ACTS_Pos)
#define ADCUI_F0MD0_F0ACTS               ADCUI_F0MD0_F0ACTS_Msk

#define ADCUI_F0MD0_F0REACTS_Pos         5
#define ADCUI_F0MD0_F0REACTS_Msk         (0x1UL << ADCUI_F0MD0_F0REACTS_Pos)
#define ADCUI_F0MD0_F0REACTS             ADCUI_F0MD0_F0REACTS_Msk

#define ADCUI_F0MD0_I0GAIN_Pos           6
#define ADCUI_F0MD0_I0GAIN_Msk           (0x1UL << ADCUI_F0MD0_I0GAIN_Pos)
#define ADCUI_F0MD0_I0GAIN               ADCUI_F0MD0_I0GAIN_Msk

#define ADCUI_F0MD0_V0GAIN_Pos           7
#define ADCUI_F0MD0_V0GAIN_Msk           (0x1UL << ADCUI_F0MD0_V0GAIN_Pos)
#define ADCUI_F0MD0_V0GAIN               ADCUI_F0MD0_V0GAIN_Msk

#define ADCUI_F0MD0_I3GAIN_Pos           8
#define ADCUI_F0MD0_I3GAIN_Msk           (0x1UL << ADCUI_F0MD0_I3GAIN_Pos)
#define ADCUI_F0MD0_I3GAIN               ADCUI_F0MD0_I3GAIN_Msk

#define ADCUI_F0MD0_F0PER_FREQ_Pos       9
#define ADCUI_F0MD0_F0PER_FREQ_Msk       (0x1FFFFFUL << ADCUI_F0MD0_F0PER_FREQ_Pos)

#define ADCUI_F0MD0_F0SEL_I_CH_Pos       30
#define ADCUI_F0MD0_F0SEL_I_CH_Msk       (0x3UL << ADCUI_F0MD0_F0SEL_I_CH_Pos)
#define ADCUI_F0MD0_F0SEL_I_CH_AUTO      (0x0UL << ADCUI_F0MD0_F0SEL_I_CH_Pos)
#define ADCUI_F0MD0_F0SEL_I_CH_ACTIVEI0  (0x1UL << ADCUI_F0MD0_F0SEL_I_CH_Pos)
#define ADCUI_F0MD0_F0SEL_I_CH_ACTIVEI3  (0x2UL << ADCUI_F0MD0_F0SEL_I_CH_Pos)

/** @} */ /* End of group MDR32VF0xI_ADCUI_FxMD0 */

/** @defgroup MDR32VF0xI_ADCUI_FxMD0 ADCUI FxMD0 for Channels 1 and 2
 * @{
 */

#define ADCUI_FxMD0_FxVSEL_Pos           0
#define ADCUI_FxMD0_FxVSEL_Msk           (0x3UL << ADCUI_FxMD0_FxVSEL_Pos)
#define ADCUI_FxMD0_FxVSEL_V_BEFORE_HPF  (0x0UL << ADCUI_FxMD0_FxVSEL_Pos)
#define ADCUI_FxMD0_FxVSEL_ACTIVEPOWER   (0x1UL << ADCUI_FxMD0_FxVSEL_Pos)
#define ADCUI_FxMD0_FxVSEL_REACTIVEPOWER (0x2UL << ADCUI_FxMD0_FxVSEL_Pos)
#define ADCUI_FxMD0_FxVSEL_V_AFTER_HPF   (0x3UL << ADCUI_FxMD0_FxVSEL_Pos)

#define ADCUI_FxMD0_FxISEL_Pos           2
#define ADCUI_FxMD0_FxISEL_Msk           (0x3UL << ADCUI_FxMD0_FxISEL_Pos)
#define ADCUI_FxMD0_FxISEL_I             (0x0UL << ADCUI_FxMD0_FxISEL_Pos)
#define ADCUI_FxMD0_FxISEL_ACTIVEPOWER   (0x1UL << ADCUI_FxMD0_FxISEL_Pos)
#define ADCUI_FxMD0_FxISEL_REACTIVEPOWER (0x2UL << ADCUI_FxMD0_FxISEL_Pos)
#define ADCUI_FxMD0_FxISEL_FULLPOWER     (0x3UL << ADCUI_FxMD0_FxISEL_Pos)

#define ADCUI_FxMD0_FxACTS_Pos           4
#define ADCUI_FxMD0_FxACTS_Msk           (0x1UL << ADCUI_FxMD0_FxACTS_Pos)
#define ADCUI_FxMD0_FxACTS               ADCUI_FxMD0_FxACTS_Msk

#define ADCUI_FxMD0_FxREACTS_Pos         5
#define ADCUI_FxMD0_FxREACTS_Msk         (0x1UL << ADCUI_FxMD0_FxREACTS_Pos)
#define ADCUI_FxMD0_FxREACTS             ADCUI_FxMD0_FxREACTS_Msk

#define ADCUI_FxMD0_IxGAIN_Pos           6
#define ADCUI_FxMD0_IxGAIN_Msk           (0x1UL << ADCUI_FxMD0_IxGAIN_Pos)
#define ADCUI_FxMD0_IxGAIN               ADCUI_FxMD0_IxGAIN_Msk

#define ADCUI_FxMD0_VxGAIN_Pos           7
#define ADCUI_FxMD0_VxGAIN_Msk           (0x1UL << ADCUI_FxMD0_VxGAIN_Pos)
#define ADCUI_FxMD0_VxGAIN               ADCUI_FxMD0_VxGAIN_Msk

#define ADCUI_FxMD0_FxPER_FREQ_Pos       9
#define ADCUI_FxMD0_FxPER_FREQ_Msk       (0x1FFFFFUL << ADCUI_FxMD0_FxPER_FREQ_Pos)

/** @} */ /* End of group MDR32VF0xI_ADCUI_FxMD0 */

/** @defgroup MDR32VF0xI_ADCUI_FxMD1 ADCUI FxMD1
 * @{
 */

#define ADCUI_FxMD1_FxIPKLVL_Pos 0
#define ADCUI_FxMD1_FxIPKLVL_Msk (0xFFFFUL << ADCUI_FxMD1_FxIPKLVL_Pos)

#define ADCUI_FxMD1_FxVPKLVL_Pos 16
#define ADCUI_FxMD1_FxVPKLVL_Msk (0xFFFFUL << ADCUI_FxMD1_FxVPKLVL_Pos)

/** @} */ /* End of group MDR32VF0xI_ADCUI_FxMD1 */

/** @defgroup MDR32VF0xI_ADCUI_FxMD2 ADCUI FxMD2
 * @{
 */

#define ADCUI_FxMD2_FxPHASE_Pos 0
#define ADCUI_FxMD2_FxPHASE_Msk (0x1FFFFUL << ADCUI_FxMD2_FxPHASE_Pos)

/** @} */ /* End of group MDR32VF0xI_ADCUI_FxMD2 */

/** @defgroup MDR32VF0xI_ADCUI_FxVPEAK ADCUI FxVPEAK
 * @{
 */

#define ADCUI_FxVPEAK_FxVPEAK_Pos 0
#define ADCUI_FxVPEAK_FxVPEAK_Msk (0xFFFFFFUL << ADCUI_FxVPEAK_FxVPEAK_Pos)

/** @} */ /* End of group MDR32VF0xI_ADCUI_FxVPEAK */

/** @defgroup MDR32VF0xI_ADCUI_FxIPEAK ADCUI FxIPEAK
 * @{
 */

#define ADCUI_FxIPEAK_FxIPEAK_Pos 0
#define ADCUI_FxIPEAK_FxIPEAK_Msk (0xFFFFFFUL << ADCUI_FxIPEAK_FxIPEAK_Pos)

/** @} */ /* End of group MDR32VF0xI_ADCUI_FxIPEAK */

/** @defgroup MDR32VF0xI_ADCUI_FxVDAT ADCUI FxVDAT
 * @{
 */

#define ADCUI_FxVDAT_FxVDAT_Pos 0
#define ADCUI_FxVDAT_FxVDAT_Msk (0xFFFFFFUL << ADCUI_FxVDAT_FxVDAT_Pos)

/** @} */ /* End of group MDR32VF0xI_ADCUI_FxVDAT */

/** @defgroup MDR32VF0xI_ADCUI_F0I0DAT ADCUI F0I0DAT
 * @{
 */

#define ADCUI_F0I0DAT_F0I0DAT_Pos 0
#define ADCUI_F0I0DAT_F0I0DAT_Msk (0xFFFFFFUL << ADCUI_F0I0DAT_F0I0DAT_Pos)

/** @} */ /* End of group MDR32VF0xI_ADCUI_F0I0DAT */

/** @defgroup MDR32VF0xI_ADCUI_F0I3DAT ADCUI F0I3DAT
 * @{
 */

#define ADCUI_F0I3DAT_F0I3DAT_Pos 0
#define ADCUI_F0I3DAT_F0I3DAT_Msk (0xFFFFFFUL << ADCUI_F0I3DAT_F0I3DAT_Pos)

/** @} */ /* End of group MDR32VF0xI_ADCUI_F0I3DAT */

/** @defgroup MDR32VF0xI_ADCUI_FxIDAT ADCUI FxIDAT
 * @{
 */

#define ADCUI_FxIDAT_FxIDAT_Pos 0
#define ADCUI_FxIDAT_FxIDAT_Msk (0xFFFFFFUL << ADCUI_FxIDAT_FxIDAT_Pos)

/** @} */ /* End of group MDR32VF0xI_ADCUI_FxIDAT */

/** @defgroup MDR32VF0xI_ADCUI_FxVRMS ADCUI FxVRMS
 * @{
 */

#define ADCUI_FxVRMS_FxVRMS_Pos 0
#define ADCUI_FxVRMS_FxVRMS_Msk (0xFFFFFFUL << ADCUI_FxVRMS_FxVRMS_Pos)

/** @} */ /* End of group MDR32VF0xI_ADCUI_FxVRMS */

/** @defgroup MDR32VF0xI_ADCUI_FxVRMS2 ADCUI FxVRMS2
 * @{
 */

#define ADCUI_FxVRMS2_FxVRMS2_Pos 0
#define ADCUI_FxVRMS2_FxVRMS2_Msk (0xFFFFFFFFUL << ADCUI_FxVRMS2_FxVRMS2_Pos)

/** @} */ /* End of group MDR32VF0xI_ADCUI_FxVRMS2 */

/** @defgroup MDR32VF0xI_ADCUI_FxIRMS ADCUI FxIRMS
 * @{
 */

#define ADCUI_FxIRMS_FxIRMS_Pos 0
#define ADCUI_FxIRMS_FxIRMS_Msk (0xFFFFFFUL << ADCUI_FxIRMS_FxIRMS_Pos)

/** @} */ /* End of group MDR32VF0xI_ADCUI_FxIRMS */

/** @defgroup MDR32VF0xI_ADCUI_FxIRMS2 ADCUI FxIRMS2
 * @{
 */

#define ADCUI_FxIRMS2_FxIRMS2_Pos 0
#define ADCUI_FxIRMS2_FxIRMS2_Msk (0xFFFFFFFFUL << ADCUI_FxIRMS2_FxIRMS2_Pos)

/** @} */ /* End of group MDR32VF0xI_ADCUI_FxIRMS2 */

/** @defgroup MDR32VF0xI_ADCUI_F0STAT ADCUI F0STAT for Channel 0
 * @{
 */

#define ADCUI_F0STAT_F0VF_EMP_Pos   0
#define ADCUI_F0STAT_F0VF_EMP_Msk   (0x1UL << ADCUI_F0STAT_F0VF_EMP_Pos)
#define ADCUI_F0STAT_F0VF_EMP       ADCUI_F0STAT_F0VF_EMP_Msk

#define ADCUI_F0STAT_F0VF_FLL_Pos   1
#define ADCUI_F0STAT_F0VF_FLL_Msk   (0x1UL << ADCUI_F0STAT_F0VF_FLL_Pos)
#define ADCUI_F0STAT_F0VF_FLL       ADCUI_F0STAT_F0VF_FLL_Msk

#define ADCUI_F0STAT_F0VF_OVR_Pos   2
#define ADCUI_F0STAT_F0VF_OVR_Msk   (0x1UL << ADCUI_F0STAT_F0VF_OVR_Pos)
#define ADCUI_F0STAT_F0VF_OVR       ADCUI_F0STAT_F0VF_OVR_Msk

#define ADCUI_F0STAT_F0IF_EMP_Pos   3
#define ADCUI_F0STAT_F0IF_EMP_Msk   (0x1UL << ADCUI_F0STAT_F0IF_EMP_Pos)
#define ADCUI_F0STAT_F0IF_EMP       ADCUI_F0STAT_F0IF_EMP_Msk

#define ADCUI_F0STAT_F0IF_FLL_Pos   4
#define ADCUI_F0STAT_F0IF_FLL_Msk   (0x1UL << ADCUI_F0STAT_F0IF_FLL_Pos)
#define ADCUI_F0STAT_F0IF_FLL       ADCUI_F0STAT_F0IF_FLL_Msk

#define ADCUI_F0STAT_F0IF_OVR_Pos   5
#define ADCUI_F0STAT_F0IF_OVR_Msk   (0x1UL << ADCUI_F0STAT_F0IF_OVR_Pos)
#define ADCUI_F0STAT_F0IF_OVR       ADCUI_F0STAT_F0IF_OVR_Msk

#define ADCUI_F0STAT_F0SAGF_Pos     6
#define ADCUI_F0STAT_F0SAGF_Msk     (0x1UL << ADCUI_F0STAT_F0SAGF_Pos)
#define ADCUI_F0STAT_F0SAGF         ADCUI_F0STAT_F0SAGF_Msk

#define ADCUI_F0STAT_F0PEAKVF_Pos   7
#define ADCUI_F0STAT_F0PEAKVF_Msk   (0x1UL << ADCUI_F0STAT_F0PEAKVF_Pos)
#define ADCUI_F0STAT_F0PEAKVF       ADCUI_F0STAT_F0PEAKVF_Msk

#define ADCUI_F0STAT_F0PEAKIF_Pos   8
#define ADCUI_F0STAT_F0PEAKIF_Msk   (0x1UL << ADCUI_F0STAT_F0PEAKIF_Pos)
#define ADCUI_F0STAT_F0PEAKIF       ADCUI_F0STAT_F0PEAKIF_Msk

#define ADCUI_F0STAT_F0WATTOVP_Pos  9
#define ADCUI_F0STAT_F0WATTOVP_Msk  (0x1UL << ADCUI_F0STAT_F0WATTOVP_Pos)
#define ADCUI_F0STAT_F0WATTOVP      ADCUI_F0STAT_F0WATTOVP_Msk

#define ADCUI_F0STAT_F0VAROVP_Pos   10
#define ADCUI_F0STAT_F0VAROVP_Msk   (0x1UL << ADCUI_F0STAT_F0VAROVP_Pos)
#define ADCUI_F0STAT_F0VAROVP       ADCUI_F0STAT_F0VAROVP_Msk

#define ADCUI_F0STAT_F0VAOV_Pos     11
#define ADCUI_F0STAT_F0VAOV_Msk     (0x1UL << ADCUI_F0STAT_F0VAOV_Pos)
#define ADCUI_F0STAT_F0VAOV         ADCUI_F0STAT_F0VAOV_Msk

#define ADCUI_F0STAT_F0ZXTOF_Pos    12
#define ADCUI_F0STAT_F0ZXTOF_Msk    (0x1UL << ADCUI_F0STAT_F0ZXTOF_Pos)
#define ADCUI_F0STAT_F0ZXTOF        ADCUI_F0STAT_F0ZXTOF_Msk

#define ADCUI_F0STAT_F0ICHANNEL_Pos 13
#define ADCUI_F0STAT_F0ICHANNEL_Msk (0x1UL << ADCUI_F0STAT_F0ICHANNEL_Pos)
#define ADCUI_F0STAT_F0ICHANNEL     ADCUI_F0STAT_F0ICHANNEL_Msk

#define ADCUI_F0STAT_F0APSIGN_Pos   15
#define ADCUI_F0STAT_F0APSIGN_Msk   (0x1UL << ADCUI_F0STAT_F0APSIGN_Pos)
#define ADCUI_F0STAT_F0APSIGN       ADCUI_F0STAT_F0APSIGN_Msk

#define ADCUI_F0STAT_F0APNLDFL_Pos  16
#define ADCUI_F0STAT_F0APNLDFL_Msk  (0x1UL << ADCUI_F0STAT_F0APNLDFL_Pos)
#define ADCUI_F0STAT_F0APNLDFL      ADCUI_F0STAT_F0APNLDFL_Msk

#define ADCUI_F0STAT_F0VARSIGN_Pos  17
#define ADCUI_F0STAT_F0VARSIGN_Msk  (0x1UL << ADCUI_F0STAT_F0VARSIGN_Pos)
#define ADCUI_F0STAT_F0VARSIGN      ADCUI_F0STAT_F0VARSIGN_Msk

#define ADCUI_F0STAT_F0VARNLDFL_Pos 18
#define ADCUI_F0STAT_F0VARNLDFL_Msk (0x1UL << ADCUI_F0STAT_F0VARNLDFL_Pos)
#define ADCUI_F0STAT_F0VARNLDFL     ADCUI_F0STAT_F0VARNLDFL_Msk

#define ADCUI_F0STAT_F0VANLDFL_Pos  20
#define ADCUI_F0STAT_F0VANLDFL_Msk  (0x1UL << ADCUI_F0STAT_F0VANLDFL_Pos)
#define ADCUI_F0STAT_F0VANLDFL      ADCUI_F0STAT_F0VANLDFL_Msk

#define ADCUI_F0STAT_F0ZEROCRS_Pos  21
#define ADCUI_F0STAT_F0ZEROCRS_Msk  (0x1UL << ADCUI_F0STAT_F0ZEROCRS_Pos)
#define ADCUI_F0STAT_F0ZEROCRS      ADCUI_F0STAT_F0ZEROCRS_Msk

#define ADCUI_F0STAT_C3IF_EMP_Pos   22
#define ADCUI_F0STAT_C3IF_EMP_Msk   (0x1UL << ADCUI_F0STAT_C3IF_EMP_Pos)
#define ADCUI_F0STAT_C3IF_EMP       ADCUI_F0STAT_C3IF_EMP_Msk

#define ADCUI_F0STAT_C3IF_FLL_Pos   23
#define ADCUI_F0STAT_C3IF_FLL_Msk   (0x1UL << ADCUI_F0STAT_C3IF_FLL_Pos)
#define ADCUI_F0STAT_C3IF_FLL       ADCUI_F0STAT_C3IF_FLL_Msk

#define ADCUI_F0STAT_C3IF_OVR_Pos   24
#define ADCUI_F0STAT_C3IF_OVR_Msk   (0x1UL << ADCUI_F0STAT_C3IF_OVR_Pos)
#define ADCUI_F0STAT_C3IF_OVR       ADCUI_F0STAT_C3IF_OVR_Msk

#define ADCUI_F0STAT_F0WATTOVN_Pos  25
#define ADCUI_F0STAT_F0WATTOVN_Msk  (0x1UL << ADCUI_F0STAT_F0WATTOVN_Pos)
#define ADCUI_F0STAT_F0WATTOVN      ADCUI_F0STAT_F0WATTOVN_Msk

#define ADCUI_F0STAT_F0VAROVN_Pos   26
#define ADCUI_F0STAT_F0VAROVN_Msk   (0x1UL << ADCUI_F0STAT_F0VAROVN_Pos)
#define ADCUI_F0STAT_F0VAROVN       ADCUI_F0STAT_F0VAROVN_Msk

/** @} */ /* End of group MDR32VF0xI_ADCUI_F0STAT */

/** @defgroup MDR32VF0xI_ADCUI_FxSTAT ADCUI FxSTAT for Channels 1 and 2
 * @{
 */

#define ADCUI_FxSTAT_FxVF_EMP_Pos   0
#define ADCUI_FxSTAT_FxVF_EMP_Msk   (0x1UL << ADCUI_FxSTAT_FxVF_EMP_Pos)
#define ADCUI_FxSTAT_FxVF_EMP       ADCUI_FxSTAT_FxVF_EMP_Msk

#define ADCUI_FxSTAT_FxVF_FLL_Pos   1
#define ADCUI_FxSTAT_FxVF_FLL_Msk   (0x1UL << ADCUI_FxSTAT_FxVF_FLL_Pos)
#define ADCUI_FxSTAT_FxVF_FLL       ADCUI_FxSTAT_FxVF_FLL_Msk

#define ADCUI_FxSTAT_FxVF_OVR_Pos   2
#define ADCUI_FxSTAT_FxVF_OVR_Msk   (0x1UL << ADCUI_FxSTAT_FxVF_OVR_Pos)
#define ADCUI_FxSTAT_FxVF_OVR       ADCUI_FxSTAT_FxVF_OVR_Msk

#define ADCUI_FxSTAT_FxIF_EMP_Pos   3
#define ADCUI_FxSTAT_FxIF_EMP_Msk   (0x1UL << ADCUI_FxSTAT_FxIF_EMP_Pos)
#define ADCUI_FxSTAT_FxIF_EMP       ADCUI_FxSTAT_FxIF_EMP_Msk

#define ADCUI_FxSTAT_FxIF_FLL_Pos   4
#define ADCUI_FxSTAT_FxIF_FLL_Msk   (0x1UL << ADCUI_FxSTAT_FxIF_FLL_Pos)
#define ADCUI_FxSTAT_FxIF_FLL       ADCUI_FxSTAT_FxIF_FLL_Msk

#define ADCUI_FxSTAT_FxIF_OVR_Pos   5
#define ADCUI_FxSTAT_FxIF_OVR_Msk   (0x1UL << ADCUI_FxSTAT_FxIF_OVR_Pos)
#define ADCUI_FxSTAT_FxIF_OVR       ADCUI_FxSTAT_FxIF_OVR_Msk

#define ADCUI_FxSTAT_FxSAGF_Pos     6
#define ADCUI_FxSTAT_FxSAGF_Msk     (0x1UL << ADCUI_FxSTAT_FxSAGF_Pos)
#define ADCUI_FxSTAT_FxSAGF         ADCUI_FxSTAT_FxSAGF_Msk

#define ADCUI_FxSTAT_FxPEAKVF_Pos   7
#define ADCUI_FxSTAT_FxPEAKVF_Msk   (0x1UL << ADCUI_FxSTAT_FxPEAKVF_Pos)
#define ADCUI_FxSTAT_FxPEAKVF       ADCUI_FxSTAT_FxPEAKVF_Msk

#define ADCUI_FxSTAT_FxPEAKIF_Pos   8
#define ADCUI_FxSTAT_FxPEAKIF_Msk   (0x1UL << ADCUI_FxSTAT_FxPEAKIF_Pos)
#define ADCUI_FxSTAT_FxPEAKIF       ADCUI_FxSTAT_FxPEAKIF_Msk

#define ADCUI_FxSTAT_FxWATTOVP_Pos  9
#define ADCUI_FxSTAT_FxWATTOVP_Msk  (0x1UL << ADCUI_FxSTAT_FxWATTOVP_Pos)
#define ADCUI_FxSTAT_FxWATTOVP      ADCUI_FxSTAT_FxWATTOVP_Msk

#define ADCUI_FxSTAT_FxVAROVP_Pos   10
#define ADCUI_FxSTAT_FxVAROVP_Msk   (0x1UL << ADCUI_FxSTAT_FxVAROVP_Pos)
#define ADCUI_FxSTAT_FxVAROVP       ADCUI_FxSTAT_FxVAROVP_Msk

#define ADCUI_FxSTAT_FxVAOV_Pos     11
#define ADCUI_FxSTAT_FxVAOV_Msk     (0x1UL << ADCUI_FxSTAT_FxVAOV_Pos)
#define ADCUI_FxSTAT_FxVAOV         ADCUI_FxSTAT_FxVAOV_Msk

#define ADCUI_FxSTAT_FxZXTOF_Pos    12
#define ADCUI_FxSTAT_FxZXTOF_Msk    (0x1UL << ADCUI_FxSTAT_FxZXTOF_Pos)
#define ADCUI_FxSTAT_FxZXTOF        ADCUI_FxSTAT_FxZXTOF_Msk

#define ADCUI_FxSTAT_FxAPSIGN_Pos   15
#define ADCUI_FxSTAT_FxAPSIGN_Msk   (0x1UL << ADCUI_FxSTAT_FxAPSIGN_Pos)
#define ADCUI_FxSTAT_FxAPSIGN       ADCUI_FxSTAT_FxAPSIGN_Msk

#define ADCUI_FxSTAT_FxAPNLDFL_Pos  16
#define ADCUI_FxSTAT_FxAPNLDFL_Msk  (0x1UL << ADCUI_FxSTAT_FxAPNLDFL_Pos)
#define ADCUI_FxSTAT_FxAPNLDFL      ADCUI_FxSTAT_FxAPNLDFL_Msk

#define ADCUI_FxSTAT_FxVARSIGN_Pos  17
#define ADCUI_FxSTAT_FxVARSIGN_Msk  (0x1UL << ADCUI_FxSTAT_FxVARSIGN_Pos)
#define ADCUI_FxSTAT_FxVARSIGN      ADCUI_FxSTAT_FxVARSIGN_Msk

#define ADCUI_FxSTAT_FxVARNLDFL_Pos 18
#define ADCUI_FxSTAT_FxVARNLDFL_Msk (0x1UL << ADCUI_FxSTAT_FxVARNLDFL_Pos)
#define ADCUI_FxSTAT_FxVARNLDFL     ADCUI_FxSTAT_FxVARNLDFL_Msk

#define ADCUI_FxSTAT_FxVANLDFL_Pos  20
#define ADCUI_FxSTAT_FxVANLDFL_Msk  (0x1UL << ADCUI_FxSTAT_FxVANLDFL_Pos)
#define ADCUI_FxSTAT_FxVANLDFL      ADCUI_FxSTAT_FxVANLDFL_Msk

#define ADCUI_FxSTAT_FxZEROCRS_Pos  21
#define ADCUI_FxSTAT_FxZEROCRS_Msk  (0x1UL << ADCUI_FxSTAT_FxZEROCRS_Pos)
#define ADCUI_FxSTAT_FxZEROCRS      ADCUI_FxSTAT_FxZEROCRS_Msk

#define ADCUI_FxSTAT_FxWATTOVN_Pos  25
#define ADCUI_FxSTAT_FxWATTOVN_Msk  (0x1UL << ADCUI_FxSTAT_FxWATTOVN_Pos)
#define ADCUI_FxSTAT_FxWATTOVN      ADCUI_FxSTAT_FxWATTOVN_Msk

#define ADCUI_FxSTAT_FxVAROVN_Pos   26
#define ADCUI_FxSTAT_FxVAROVN_Msk   (0x1UL << ADCUI_FxSTAT_FxVAROVN_Pos)
#define ADCUI_FxSTAT_FxVAROVN       ADCUI_FxSTAT_FxVAROVN_Msk

/** @} */ /* End of group MDR32VF0xI_ADCUI_FxSTAT */

/** @defgroup MDR32VF0xI_ADCUI_F0MASK ADCUI F0MASK for Channel 0
 * @{
 */

#define ADCUI_F0MASK_F0VF_EMPM_Pos   0
#define ADCUI_F0MASK_F0VF_EMPM_Msk   (0x1UL << ADCUI_F0MASK_F0VF_EMPM_Pos)
#define ADCUI_F0MASK_F0VF_EMPM       ADCUI_F0MASK_F0VF_EMPM_Msk

#define ADCUI_F0MASK_F0VF_FLLM_Pos   1
#define ADCUI_F0MASK_F0VF_FLLM_Msk   (0x1UL << ADCUI_F0MASK_F0VF_FLLM_Pos)
#define ADCUI_F0MASK_F0VF_FLLM       ADCUI_F0MASK_F0VF_FLLM_Msk

#define ADCUI_F0MASK_F0VF_OVRM_Pos   2
#define ADCUI_F0MASK_F0VF_OVRM_Msk   (0x1UL << ADCUI_F0MASK_F0VF_OVRM_Pos)
#define ADCUI_F0MASK_F0VF_OVRM       ADCUI_F0MASK_F0VF_OVRM_Msk

#define ADCUI_F0MASK_F0IF_EMPM_Pos   3
#define ADCUI_F0MASK_F0IF_EMPM_Msk   (0x1UL << ADCUI_F0MASK_F0IF_EMPM_Pos)
#define ADCUI_F0MASK_F0IF_EMPM       ADCUI_F0MASK_F0IF_EMPM_Msk

#define ADCUI_F0MASK_F0IF_FLLM_Pos   4
#define ADCUI_F0MASK_F0IF_FLLM_Msk   (0x1UL << ADCUI_F0MASK_F0IF_FLLM_Pos)
#define ADCUI_F0MASK_F0IF_FLLM       ADCUI_F0MASK_F0IF_FLLM_Msk

#define ADCUI_F0MASK_F0IF_OVRM_Pos   5
#define ADCUI_F0MASK_F0IF_OVRM_Msk   (0x1UL << ADCUI_F0MASK_F0IF_OVRM_Pos)
#define ADCUI_F0MASK_F0IF_OVRM       ADCUI_F0MASK_F0IF_OVRM_Msk

#define ADCUI_F0MASK_F0SAGFM_Pos     6
#define ADCUI_F0MASK_F0SAGFM_Msk     (0x1UL << ADCUI_F0MASK_F0SAGFM_Pos)
#define ADCUI_F0MASK_F0SAGFM         ADCUI_F0MASK_F0SAGFM_Msk

#define ADCUI_F0MASK_F0PEAKVFM_Pos   7
#define ADCUI_F0MASK_F0PEAKVFM_Msk   (0x1UL << ADCUI_F0MASK_F0PEAKVFM_Pos)
#define ADCUI_F0MASK_F0PEAKVFM       ADCUI_F0MASK_F0PEAKVFM_Msk

#define ADCUI_F0MASK_F0PEAKIFM_Pos   8
#define ADCUI_F0MASK_F0PEAKIFM_Msk   (0x1UL << ADCUI_F0MASK_F0PEAKIFM_Pos)
#define ADCUI_F0MASK_F0PEAKIFM       ADCUI_F0MASK_F0PEAKIFM_Msk

#define ADCUI_F0MASK_F0WATTOVPM_Pos  9
#define ADCUI_F0MASK_F0WATTOVPM_Msk  (0x1UL << ADCUI_F0MASK_F0WATTOVPM_Pos)
#define ADCUI_F0MASK_F0WATTOVPM      ADCUI_F0MASK_F0WATTOVPM_Msk

#define ADCUI_F0MASK_F0VAROVPM_Pos   10
#define ADCUI_F0MASK_F0VAROVPM_Msk   (0x1UL << ADCUI_F0MASK_F0VAROVPM_Pos)
#define ADCUI_F0MASK_F0VAROVPM       ADCUI_F0MASK_F0VAROVPM_Msk

#define ADCUI_F0MASK_F0VAOVM_Pos     11
#define ADCUI_F0MASK_F0VAOVM_Msk     (0x1UL << ADCUI_F0MASK_F0VAOVM_Pos)
#define ADCUI_F0MASK_F0VAOVM         ADCUI_F0MASK_F0VAOVM_Msk

#define ADCUI_F0MASK_F0ZXTOFM_Pos    12
#define ADCUI_F0MASK_F0ZXTOFM_Msk    (0x1UL << ADCUI_F0MASK_F0ZXTOFM_Pos)
#define ADCUI_F0MASK_F0ZXTOFM        ADCUI_F0MASK_F0ZXTOFM_Msk

#define ADCUI_F0MASK_F0APSIGNM_Pos   15
#define ADCUI_F0MASK_F0APSIGNM_Msk   (0x1UL << ADCUI_F0MASK_F0APSIGNM_Pos)
#define ADCUI_F0MASK_F0APSIGNM       ADCUI_F0MASK_F0APSIGNM_Msk

#define ADCUI_F0MASK_F0APNLDFLM_Pos  16
#define ADCUI_F0MASK_F0APNLDFLM_Msk  (0x1UL << ADCUI_F0MASK_F0APNLDFLM_Pos)
#define ADCUI_F0MASK_F0APNLDFLM      ADCUI_F0MASK_F0APNLDFLM_Msk

#define ADCUI_F0MASK_F0VARSIGNM_Pos  17
#define ADCUI_F0MASK_F0VARSIGNM_Msk  (0x1UL << ADCUI_F0MASK_F0VARSIGNM_Pos)
#define ADCUI_F0MASK_F0VARSIGNM      ADCUI_F0MASK_F0VARSIGNM_Msk

#define ADCUI_F0MASK_F0VARNLDFLM_Pos 18
#define ADCUI_F0MASK_F0VARNLDFLM_Msk (0x1UL << ADCUI_F0MASK_F0VARNLDFLM_Pos)
#define ADCUI_F0MASK_F0VARNLDFLM     ADCUI_F0MASK_F0VARNLDFLM_Msk

#define ADCUI_F0MASK_F0VANLDFLM_Pos  20
#define ADCUI_F0MASK_F0VANLDFLM_Msk  (0x1UL << ADCUI_F0MASK_F0VANLDFLM_Pos)
#define ADCUI_F0MASK_F0VANLDFLM      ADCUI_F0MASK_F0VANLDFLM_Msk

#define ADCUI_F0MASK_F0ZEROCRSM_Pos  21
#define ADCUI_F0MASK_F0ZEROCRSM_Msk  (0x1UL << ADCUI_F0MASK_F0ZEROCRSM_Pos)
#define ADCUI_F0MASK_F0ZEROCRSM      ADCUI_F0MASK_F0ZEROCRSM_Msk

#define ADCUI_F0MASK_C3IF_EMPM_Pos   22
#define ADCUI_F0MASK_C3IF_EMPM_Msk   (0x1UL << ADCUI_F0MASK_C3IF_EMPM_Pos)
#define ADCUI_F0MASK_C3IF_EMPM       ADCUI_F0MASK_C3IF_EMPM_Msk

#define ADCUI_F0MASK_C3IF_FLLM_Pos   23
#define ADCUI_F0MASK_C3IF_FLLM_Msk   (0x1UL << ADCUI_F0MASK_C3IF_FLLM_Pos)
#define ADCUI_F0MASK_C3IF_FLLM       ADCUI_F0MASK_C3IF_FLLM_Msk

#define ADCUI_F0MASK_C3IF_OVRM_Pos   24
#define ADCUI_F0MASK_C3IF_OVRM_Msk   (0x1UL << ADCUI_F0MASK_C3IF_OVRM_Pos)
#define ADCUI_F0MASK_C3IF_OVRM       ADCUI_F0MASK_C3IF_OVRM_Msk

#define ADCUI_F0MASK_F0WATTOVNM_Pos  25
#define ADCUI_F0MASK_F0WATTOVNM_Msk  (0x1UL << ADCUI_F0MASK_F0WATTOVNM_Pos)
#define ADCUI_F0MASK_F0WATTOVNM      ADCUI_F0MASK_F0WATTOVNM_Msk

#define ADCUI_F0MASK_F0VAROVNM_Pos   26
#define ADCUI_F0MASK_F0VAROVNM_Msk   (0x1UL << ADCUI_F0MASK_F0VAROVNM_Pos)
#define ADCUI_F0MASK_F0VAROVNM       ADCUI_F0MASK_F0VAROVNM_Msk

/** @} */ /* End of group MDR32VF0xI_ADCUI_F0MASK */

/** @defgroup MDR32VF0xI_ADCUI_FxMASK ADCUI FxMASK for Channels 1 and 2
 * @{
 */

#define ADCUI_FxMASK_FxVF_EMPM_Pos   0
#define ADCUI_FxMASK_FxVF_EMPM_Msk   (0x1UL << ADCUI_FxMASK_FxVF_EMPM_Pos)
#define ADCUI_FxMASK_FxVF_EMPM       ADCUI_FxMASK_FxVF_EMPM_Msk

#define ADCUI_FxMASK_FxVF_FLLM_Pos   1
#define ADCUI_FxMASK_FxVF_FLLM_Msk   (0x1UL << ADCUI_FxMASK_FxVF_FLLM_Pos)
#define ADCUI_FxMASK_FxVF_FLLM       ADCUI_FxMASK_FxVF_FLLM_Msk

#define ADCUI_FxMASK_FxVF_OVRM_Pos   2
#define ADCUI_FxMASK_FxVF_OVRM_Msk   (0x1UL << ADCUI_FxMASK_FxVF_OVRM_Pos)
#define ADCUI_FxMASK_FxVF_OVRM       ADCUI_FxMASK_FxVF_OVRM_Msk

#define ADCUI_FxMASK_FxIF_EMPM_Pos   3
#define ADCUI_FxMASK_FxIF_EMPM_Msk   (0x1UL << ADCUI_FxMASK_FxIF_EMPM_Pos)
#define ADCUI_FxMASK_FxIF_EMPM       ADCUI_FxMASK_FxIF_EMPM_Msk

#define ADCUI_FxMASK_FxIF_FLLM_Pos   4
#define ADCUI_FxMASK_FxIF_FLLM_Msk   (0x1UL << ADCUI_FxMASK_FxIF_FLLM_Pos)
#define ADCUI_FxMASK_FxIF_FLLM       ADCUI_FxMASK_FxIF_FLLM_Msk

#define ADCUI_FxMASK_FxIF_OVRM_Pos   5
#define ADCUI_FxMASK_FxIF_OVRM_Msk   (0x1UL << ADCUI_FxMASK_FxIF_OVRM_Pos)
#define ADCUI_FxMASK_FxIF_OVRM       ADCUI_FxMASK_FxIF_OVRM_Msk

#define ADCUI_FxMASK_FxSAGFM_Pos     6
#define ADCUI_FxMASK_FxSAGFM_Msk     (0x1UL << ADCUI_FxMASK_FxSAGFM_Pos)
#define ADCUI_FxMASK_FxSAGFM         ADCUI_FxMASK_FxSAGFM_Msk

#define ADCUI_FxMASK_FxPEAKVFM_Pos   7
#define ADCUI_FxMASK_FxPEAKVFM_Msk   (0x1UL << ADCUI_FxMASK_FxPEAKVFM_Pos)
#define ADCUI_FxMASK_FxPEAKVFM       ADCUI_FxMASK_FxPEAKVFM_Msk

#define ADCUI_FxMASK_FxPEAKIFM_Pos   8
#define ADCUI_FxMASK_FxPEAKIFM_Msk   (0x1UL << ADCUI_FxMASK_FxPEAKIFM_Pos)
#define ADCUI_FxMASK_FxPEAKIFM       ADCUI_FxMASK_FxPEAKIFM_Msk

#define ADCUI_FxMASK_FxWATTOVPM_Pos  9
#define ADCUI_FxMASK_FxWATTOVPM_Msk  (0x1UL << ADCUI_FxMASK_FxWATTOVPM_Pos)
#define ADCUI_FxMASK_FxWATTOVPM      ADCUI_FxMASK_FxWATTOVPM_Msk

#define ADCUI_FxMASK_FxVAROVPM_Pos   10
#define ADCUI_FxMASK_FxVAROVPM_Msk   (0x1UL << ADCUI_FxMASK_FxVAROVPM_Pos)
#define ADCUI_FxMASK_FxVAROVPM       ADCUI_FxMASK_FxVAROVPM_Msk

#define ADCUI_FxMASK_FxVAOVM_Pos     11
#define ADCUI_FxMASK_FxVAOVM_Msk     (0x1UL << ADCUI_FxMASK_FxVAOVM_Pos)
#define ADCUI_FxMASK_FxVAOVM         ADCUI_FxMASK_FxVAOVM_Msk

#define ADCUI_FxMASK_FxZXTOFM_Pos    12
#define ADCUI_FxMASK_FxZXTOFM_Msk    (0x1UL << ADCUI_FxMASK_FxZXTOFM_Pos)
#define ADCUI_FxMASK_FxZXTOFM        ADCUI_FxMASK_FxZXTOFM_Msk

#define ADCUI_FxMASK_FxAPSIGNM_Pos   15
#define ADCUI_FxMASK_FxAPSIGNM_Msk   (0x1UL << ADCUI_FxMASK_FxAPSIGNM_Pos)
#define ADCUI_FxMASK_FxAPSIGNM       ADCUI_FxMASK_FxAPSIGNM_Msk

#define ADCUI_FxMASK_FxAPNLDFLM_Pos  16
#define ADCUI_FxMASK_FxAPNLDFLM_Msk  (0x1UL << ADCUI_FxMASK_FxAPNLDFLM_Pos)
#define ADCUI_FxMASK_FxAPNLDFLM      ADCUI_FxMASK_FxAPNLDFLM_Msk

#define ADCUI_FxMASK_FxVARSIGNM_Pos  17
#define ADCUI_FxMASK_FxVARSIGNM_Msk  (0x1UL << ADCUI_FxMASK_FxVARSIGNM_Pos)
#define ADCUI_FxMASK_FxVARSIGNM      ADCUI_FxMASK_FxVARSIGNM_Msk

#define ADCUI_FxMASK_FxVARNLDFLM_Pos 18
#define ADCUI_FxMASK_FxVARNLDFLM_Msk (0x1UL << ADCUI_FxMASK_FxVARNLDFLM_Pos)
#define ADCUI_FxMASK_FxVARNLDFLM     ADCUI_FxMASK_FxVARNLDFLM_Msk

#define ADCUI_FxMASK_FxVANLDFLM_Pos  20
#define ADCUI_FxMASK_FxVANLDFLM_Msk  (0x1UL << ADCUI_FxMASK_FxVANLDFLM_Pos)
#define ADCUI_FxMASK_FxVANLDFLM      ADCUI_FxMASK_FxVANLDFLM_Msk

#define ADCUI_FxMASK_FxZEROCRSM_Pos  21
#define ADCUI_FxMASK_FxZEROCRSM_Msk  (0x1UL << ADCUI_FxMASK_FxZEROCRSM_Pos)
#define ADCUI_FxMASK_FxZEROCRSM      ADCUI_FxMASK_FxZEROCRSM_Msk

#define ADCUI_FxMASK_FxWATTOVNM_Pos  25
#define ADCUI_FxMASK_FxWATTOVNM_Msk  (0x1UL << ADCUI_FxMASK_FxWATTOVNM_Pos)
#define ADCUI_FxMASK_FxWATTOVNM      ADCUI_FxMASK_FxWATTOVNM_Msk

#define ADCUI_FxMASK_FxVAROVNM_Pos   26
#define ADCUI_FxMASK_FxVAROVNM_Msk   (0x1UL << ADCUI_FxMASK_FxVAROVNM_Pos)
#define ADCUI_FxMASK_FxVAROVNM       ADCUI_FxMASK_FxVAROVNM_Msk

/** @} */ /* End of group MDR32VF0xI_ADCUI_FxMASK */

/** @defgroup MDR32VF0xI_ADCUI_CCALx ADCUI CCALx
 * @{
 */

#if defined(USE_MDR32F02_REV_1X)

#define ADCUI_CCALx_VxBGAIN_Pos 0
#define ADCUI_CCALx_VxBGAIN_Msk (0xFFFUL << ADCUI_CCALx_VxBGAIN_Pos)

#define ADCUI_CCALx_IxBGAIN_Pos 12
#define ADCUI_CCALx_IxBGAIN_Msk (0xFFFUL << ADCUI_CCALx_IxBGAIN_Pos)

#else

#define ADCUI_CCALx_VxBGAIN_Pos 0
#define ADCUI_CCALx_VxBGAIN_Msk (0xFFFFUL << ADCUI_CCALx_VxBGAIN_Pos)

#define ADCUI_CCALx_IxBGAIN_Pos 16
#define ADCUI_CCALx_IxBGAIN_Msk (0xFFFFUL << ADCUI_CCALx_IxBGAIN_Pos)

#endif

/** @} */ /* End of group MDR32VF0xI_ADCUI_CCALx */

/** @defgroup MDR32VF0xI_ADCUI_CCAL4 ADCUI CCAL4
 * @{
 */

#define ADCUI_CCAL4_I3BGAIN_Pos 0
#if defined(USE_MDR32F02_REV_1X)
#define ADCUI_CCAL4_I3BGAIN_Msk (0xFFFUL << ADCUI_CCAL4_I3BGAIN_Pos)
#else
#define ADCUI_CCAL4_I3BGAIN_Msk (0xFFFFUL << ADCUI_CCAL4_I3BGAIN_Pos)
#endif

/** @} */ /* End of group MDR32VF0xI_ADCUI_CCAL4 */

/** @defgroup MDR32VF0xI_ADCUI_F0IRMS_INACTIVE ADCUI F0IRMS_INACTIVE
 * @{
 */

#define ADCUI_F0IRMS_INACTIVE_F0IRMS_INACTIVE_Pos 0
#define ADCUI_F0IRMS_INACTIVE_F0IRMS_INACTIVE_Msk (0xFFFFFFUL << ADCUI_F0IRMS_INACTIVE_F0IRMS_INACTIVE_Pos)

/** @} */ /* End of group MDR32VF0xI_ADCUI_F0IRMS_INACTIVE */

/** @defgroup MDR32VF0xI_ADCUI_FxWATTP_L ADCUI FxWATTP_L
 * @{
 */

#define ADCUI_FxWATTP_L_FxWATTHRP_L_Pos 0
#define ADCUI_FxWATTP_L_FxWATTHRP_L_Msk (0x1FFFFFFUL << ADCUI_FxWATTP_L_FxWATTHRP_L_Pos)

/** @} */ /* End of group MDR32VF0xI_ADCUI_FxWATTP_L */

/** @defgroup MDR32VF0xI_ADCUI_FxWATTN_L ADCUI FxWATTN_L
 * @{
 */

#define ADCUI_FxWATTN_L_FxWATTHRN_L_Pos 0
#define ADCUI_FxWATTN_L_FxWATTHRN_L_Msk (0x1FFFFFFUL << ADCUI_FxWATTN_L_FxWATTHRN_L_Pos)

/** @} */ /* End of group MDR32VF0xI_ADCUI_FxWATTN_L */

/** @defgroup MDR32VF0xI_ADCUI_FxVARP_L ADCUI FxVARP_L
 * @{
 */

#define ADCUI_FxVARP_L_FxVARHRP_L_Pos 0
#define ADCUI_FxVARP_L_FxVARHRP_L_Msk (0x1FFFFFFUL << ADCUI_FxVARP_L_FxVARHRP_L_Pos)

/** @} */ /* End of group MDR32VF0xI_ADCUI_FxVARP_L */

/** @defgroup MDR32VF0xI_ADCUI_FxVARN_L ADCUI FxVARN_L
 * @{
 */

#define ADCUI_FxVARN_L_FxVARHRN_L_Pos 0
#define ADCUI_FxVARN_L_FxVARHRN_L_Msk (0x1FFFFFFUL << ADCUI_FxVARN_L_FxVARHRN_L_Pos)

/** @} */ /* End of group MDR32VF0xI_ADCUI_FxVARN_L */

/** @defgroup MDR32VF0xI_ADCUI_FxVR_L ADCUI FxVR_L
 * @{
 */

#define ADCUI_FxVR_L_FxVAHR_L_Pos 0
#define ADCUI_FxVR_L_FxVAHR_L_Msk (0x1FFFFFFUL << ADCUI_FxVR_L_FxVAHR_L_Pos)

/** @} */ /* End of group MDR32VF0xI_ADCUI_FxVR_L */

#if defined(USE_MDR32F02_REV_2) || defined(USE_MDR1206FI) || defined(USE_MDR1206AFI) || defined(USE_MDR1206)

/** @defgroup MDR32VF0xI_ADCUI_LOAD ADCUI LOAD
 * @{
 */

#define ADCUI_LOAD_APNOLOAD_Pos  0
#define ADCUI_LOAD_APNOLOAD_Msk  (0x3FUL << ADCUI_LOAD_APNOLOAD_Pos)

#define ADCUI_LOAD_VARNOLOAD_Pos 6
#define ADCUI_LOAD_VARNOLOAD_Msk (0x3FUL << ADCUI_LOAD_VARNOLOAD_Pos)

#define ADCUI_LOAD_VANOLOAD_Pos  12
#define ADCUI_LOAD_VANOLOAD_Msk  (0x3FUL << ADCUI_LOAD_VANOLOAD_Pos)

/** @} */ /* End of group MDR32VF0xI_ADCUI_LOAD */

/** @defgroup MDR32VF0xI_ADCUI_FxVRMS_TRUE ADCUI FxVRMS_TRUE
 * @{
 */

#define ADCUI_FxVRMS_TRUE_FxVRMS_TRUE_Pos 0
#define ADCUI_FxVRMS_TRUE_FxVRMS_TRUE_Msk (0x3FUL << ADCUI_FxVRMS_TRUE_FxVRMS_TRUE_Pos)

/** @} */ /* End of group MDR32VF0xI_ADCUI_FxVRMS_TRUE */

#endif

#if defined(USE_MDR1206FI) || defined(USE_MDR1206AFI) || defined(USE_MDR1206)

/** @defgroup MDR32VF0xI_ADCUI_MSC ADCUI MSC
 * @{
 */

#define ADCUI_MSC_DMA_EN_RQ_B_Pos  0
#define ADCUI_MSC_DMA_EN_RQ_B_Msk  (0x7FUL << ADCUI_MSC_DMA_EN_RQ_B_Pos)
#define ADCUI_MSC_DMA_EN_RQ_B1     (0x01UL << ADCUI_MSC_DMA_EN_RQ_B_Pos)
#define ADCUI_MSC_DMA_EN_RQ_B2     (0x02UL << ADCUI_MSC_DMA_EN_RQ_B_Pos)
#define ADCUI_MSC_DMA_EN_RQ_B3     (0x04UL << ADCUI_MSC_DMA_EN_RQ_B_Pos)
#define ADCUI_MSC_DMA_EN_RQ_B4     (0x08UL << ADCUI_MSC_DMA_EN_RQ_B_Pos)
#define ADCUI_MSC_DMA_EN_RQ_B5     (0x10UL << ADCUI_MSC_DMA_EN_RQ_B_Pos)
#define ADCUI_MSC_DMA_EN_RQ_B6     (0x20UL << ADCUI_MSC_DMA_EN_RQ_B_Pos)
#define ADCUI_MSC_DMA_EN_RQ_B7     (0x40UL << ADCUI_MSC_DMA_EN_RQ_B_Pos)

#define ADCUI_MSC_DMA_EN_RQ_S_Pos  7
#define ADCUI_MSC_DMA_EN_RQ_S_Msk  (0x7FUL << ADCUI_MSC_DMA_EN_RQ_S_Pos)
#define ADCUI_MSC_DMA_EN_RQ_S1     (0x01UL << ADCUI_MSC_DMA_EN_RQ_S_Pos)
#define ADCUI_MSC_DMA_EN_RQ_S2     (0x02UL << ADCUI_MSC_DMA_EN_RQ_S_Pos)
#define ADCUI_MSC_DMA_EN_RQ_S3     (0x04UL << ADCUI_MSC_DMA_EN_RQ_S_Pos)
#define ADCUI_MSC_DMA_EN_RQ_S4     (0x08UL << ADCUI_MSC_DMA_EN_RQ_S_Pos)
#define ADCUI_MSC_DMA_EN_RQ_S5     (0x10UL << ADCUI_MSC_DMA_EN_RQ_S_Pos)
#define ADCUI_MSC_DMA_EN_RQ_S6     (0x20UL << ADCUI_MSC_DMA_EN_RQ_S_Pos)
#define ADCUI_MSC_DMA_EN_RQ_S7     (0x40UL << ADCUI_MSC_DMA_EN_RQ_S_Pos)

#define ADCUI_MSC_F3IRMSOS_Pos     14
#define ADCUI_MSC_F3IRMSOS_Msk     (0xFFFUL << ADCUI_MSC_F3IRMSOS_Pos)

#define ADCUI_MSC_F0ISEL_HPF_Pos   26
#define ADCUI_MSC_F0ISEL_HPF_Msk   (0x1UL << ADCUI_MSC_F0ISEL_HPF_Pos)
#define ADCUI_MSC_F0ISEL_HPF       ADCUI_MSC_F0ISEL_HPF_Msk

#define ADCUI_MSC_F1ISEL_HPF_Pos   27
#define ADCUI_MSC_F1ISEL_HPF_Msk   (0x1UL << ADCUI_MSC_F1ISEL_HPF_Pos)
#define ADCUI_MSC_F1ISEL_HPF       ADCUI_MSC_F1ISEL_HPF_Msk

#define ADCUI_MSC_F2ISEL_HPF_Pos   28
#define ADCUI_MSC_F2ISEL_HPF_Msk   (0x1UL << ADCUI_MSC_F2ISEL_HPF_Pos)
#define ADCUI_MSC_F2ISEL_HPF       ADCUI_MSC_F2ISEL_HPF_Msk

#define ADCUI_MSC_F0VPHASE_LOW_Pos 29
#define ADCUI_MSC_F0VPHASE_LOW_Msk (0x3UL << ADCUI_MSC_F0VPHASE_LOW_Pos)

#define ADCUI_MSC_F90_CONF_Pos     31
#define ADCUI_MSC_F90_CONF_Msk     (0x1UL << ADCUI_MSC_F90_CONF_Pos)
#define ADCUI_MSC_F90_CONF         ADCUI_MSC_F90_CONF_Msk

/** @} */ /* End of group MDR32VF0xI_ADCUI_MSC */

/** @defgroup MDR32VF0xI_ADCUI_FILTERCFG ADCUI FILTERCFG
 * @{
 */

#define ADCUI_FILTERCFG_CHPF_COEFF_Pos  0
#define ADCUI_FILTERCFG_CHPF_COEFF_Msk  (0xFFFUL << ADCUI_FILTERCFG_CHPF_COEFF_Pos)

#define ADCUI_FILTERCFG_CLRMS_COEFF_Pos 12
#define ADCUI_FILTERCFG_CLRMS_COEFF_Msk (0xFFFUL << ADCUI_FILTERCFG_CLRMS_COEFF_Pos)

#define ADCUI_FILTERCFG_CHP_ADC_CFG_Pos 24
#define ADCUI_FILTERCFG_CHP_ADC_CFG_Msk (0x3UL << ADCUI_FILTERCFG_CHP_ADC_CFG_Pos)

/** @} */ /* End of group MDR32VF0xI_ADCUI_FILTERCFG */

/** @defgroup MDR32VF0xI_ADCUI_CLKPHASE1 ADCUI CLKPHASE1
 * @{
 */

#define ADCUI_CLKPHASE1_I0PHASE_Pos     0
#define ADCUI_CLKPHASE1_I0PHASE_Msk     (0x3FFUL << ADCUI_CLKPHASE1_I0PHASE_Pos)

#define ADCUI_CLKPHASE1_I1PHASE_Pos     10
#define ADCUI_CLKPHASE1_I1PHASE_Msk     (0x3FFUL << ADCUI_CLKPHASE1_I1PHASE_Pos)

#define ADCUI_CLKPHASE1_I2PHASE_Pos     20
#define ADCUI_CLKPHASE1_I2PHASE_Msk     (0x3FFUL << ADCUI_CLKPHASE1_I2PHASE_Pos)

#define ADCUI_CLKPHASE1_CLC_ADC_CFG_Pos 30
#define ADCUI_CLKPHASE1_CLC_ADC_CFG_Msk (0x3UL << ADCUI_CLKPHASE1_CLC_ADC_CFG_Pos)

/** @} */ /* End of group MDR32VF0xI_ADCUI_CLKPHASE1 */

/** @defgroup MDR32VF0xI_ADCUI_CLKPHASE2 ADCUI CLKPHASE2
 * @{
 */

#define ADCUI_CLKPHASE2_I3PHASE_Pos 0
#define ADCUI_CLKPHASE2_I3PHASE_Msk (0x3FFUL << ADCUI_CLKPHASE2_I3PHASE_Pos)

/** @} */ /* End of group MDR32VF0xI_ADCUI_CLKPHASE2 */

#endif

/** @} */ /* End of group MDR32VF0xI_ADCUI_Defines */

/** @} */ /* End of group MDR32VF0xI_ADCUI */

/** @defgroup MDR32VF0xI_TIMER MDR_TIMER
 * @{
 */

/**
 * @brief MDR32VF0xI TIMER control structure.
 */
typedef struct {
    __IO uint32_t CNT;          /*!<TIMER Counter Register */
    __IO uint32_t PSG;          /*!<TIMER Clock Prescaler Register */
    __IO uint32_t ARR;          /*!<TIMER Auto-Reload Register */
    __IO uint32_t CNTRL;        /*!<TIMER Control Register */
    __IO uint32_t CH1_CCR;      /*!<TIMER Channel 1 Capture/Compare Register */
    __IO uint32_t CH2_CCR;      /*!<TIMER Channel 2 Capture/Compare Register */
    __IO uint32_t CH3_CCR;      /*!<TIMER Channel 3 Capture/Compare Register */
    __IO uint32_t CH4_CCR;      /*!<TIMER Channel 4 Capture/Compare Register */
    __IO uint32_t CH1_CNTRL0;   /*!<TIMER Channel 1 Control0 Register */
    __IO uint32_t CH2_CNTRL0;   /*!<TIMER Channel 2 Control0 Register */
    __IO uint32_t CH3_CNTRL0;   /*!<TIMER Channel 3 Control0 Register */
    __IO uint32_t CH4_CNTRL0;   /*!<TIMER Channel 4 Control0 Register */
    __IO uint32_t CH1_CNTRL1;   /*!<TIMER Channel 1 Control1 Register */
    __IO uint32_t CH2_CNTRL1;   /*!<TIMER Channel 2 Control1 Register */
    __IO uint32_t CH3_CNTRL1;   /*!<TIMER Channel 3 Control1 Register */
    __IO uint32_t CH4_CNTRL1;   /*!<TIMER Channel 4 Control1 Register */
    __IO uint32_t CH1_DTG;      /*!<TIMER Channel 1 DTG Register */
    __IO uint32_t CH2_DTG;      /*!<TIMER Channel 2 DTG Register */
    __IO uint32_t CH3_DTG;      /*!<TIMER Channel 3 DTG Register */
    __IO uint32_t CH4_DTG;      /*!<TIMER Channel 4 DTG Register */
    __IO uint32_t BRKETR_CNTRL; /*!<TIMER BRK/ETR Control Register */
    __IO uint32_t STATUS;       /*!<TIMER Status Register */
    __IO uint32_t IE;           /*!<TIMER Interrupt Enable Register */
    __IO uint32_t DMA_RE;       /*!<TIMER DMA Request Enable Register */
    __IO uint32_t CH1_CNTRL2;   /*!<TIMER Channel 1 Control2 Register */
    __IO uint32_t CH2_CNTRL2;   /*!<TIMER Channel 2 Control2 Register */
    __IO uint32_t CH3_CNTRL2;   /*!<TIMER Channel 3 Control2 Register */
    __IO uint32_t CH4_CNTRL2;   /*!<TIMER Channel 4 Control2 Register */
    __IO uint32_t CH1_CCR1;     /*!<TIMER Channel 1 Capture/Compare1 Register */
    __IO uint32_t CH2_CCR1;     /*!<TIMER Channel 2 Capture/Compare1 Register */
    __IO uint32_t CH3_CCR1;     /*!<TIMER Channel 3 Capture/Compare1 Register */
    __IO uint32_t CH4_CCR1;     /*!<TIMER Channel 4 Capture/Compare1 Register */
} MDR_TIMER_TypeDef;

/** @defgroup MDR32VF0xI_TIMER_Defines TIMER Defines
 * @{
 */

/** @defgroup MDR32VF0xI_TIMER_CNT TIMER CNT
 * @{
 */

#define TIMER_CNT_CNT_Pos 0
#define TIMER_CNT_CNT_Msk (0xFFFFFFFFUL << TIMER_CNT_CNT_Pos)

/** @} */ /* End of group MDR32VF0xI_TIMER_CNT */

/** @defgroup MDR32VF0xI_TIMER_PSG TIMER PSG
 * @{
 */

#define TIMER_PSG_PSG_Pos 0
#define TIMER_PSG_PSG_Msk (0xFFFFFFFFUL << TIMER_PSG_PSG_Pos)

/** @} */ /* End of group MDR32VF0xI_TIMER_PSG */

/** @defgroup MDR32VF0xI_TIMER_ARR TIMER ARR
 * @{
 */

#define TIMER_ARR_ARR_Pos 0
#define TIMER_ARR_ARR_Msk (0xFFFFFFFFUL << TIMER_ARR_ARR_Pos)

/** @} */ /* End of group MDR32VF0xI_TIMER_ARR */

/** @defgroup MDR32VF0xI_TIMER_CNTRL TIMER CNTRL
 * @{
 */

#define TIMER_CNTRL_CNT_EN_Pos              0
#define TIMER_CNTRL_CNT_EN_Msk              (0x1UL << TIMER_CNTRL_CNT_EN_Pos)
#define TIMER_CNTRL_CNT_EN                  TIMER_CNTRL_CNT_EN_Msk

#define TIMER_CNTRL_ARRB_EN_Pos             1
#define TIMER_CNTRL_ARRB_EN_Msk             (0x1UL << TIMER_CNTRL_ARRB_EN_Pos)
#define TIMER_CNTRL_ARRB_EN                 TIMER_CNTRL_ARRB_EN_Msk

#define TIMER_CNTRL_WR_CMPL_Pos             2
#define TIMER_CNTRL_WR_CMPL_Msk             (0x1UL << TIMER_CNTRL_WR_CMPL_Pos)
#define TIMER_CNTRL_WR_CMPL                 TIMER_CNTRL_WR_CMPL_Msk

#define TIMER_CNTRL_DIR_Pos                 3
#define TIMER_CNTRL_DIR_Msk                 (0x1UL << TIMER_CNTRL_DIR_Pos)
#define TIMER_CNTRL_DIR                     TIMER_CNTRL_DIR_Msk

#define TIMER_CNTRL_FDTS_Pos                4
#define TIMER_CNTRL_FDTS_Msk                (0x3UL << TIMER_CNTRL_FDTS_Pos)
#define TIMER_CNTRL_FDTS_DIV_1              (0x0UL << TIMER_CNTRL_FDTS_Pos)
#define TIMER_CNTRL_FDTS_DIV_2              (0x1UL << TIMER_CNTRL_FDTS_Pos)
#define TIMER_CNTRL_FDTS_DIV_3              (0x2UL << TIMER_CNTRL_FDTS_Pos)
#define TIMER_CNTRL_FDTS_DIV_4              (0x3UL << TIMER_CNTRL_FDTS_Pos)

#define TIMER_CNTRL_CNT_MODE_Pos            6
#define TIMER_CNTRL_CNT_MODE_Msk            (0x3UL << TIMER_CNTRL_CNT_MODE_Pos)
#define TIMER_CNTRL_CNT_MODE_CLK_FIXED_DIR  (0x0UL << TIMER_CNTRL_CNT_MODE_Pos)
#define TIMER_CNTRL_CNT_MODE_CLK_CHANGE_DIR (0x1UL << TIMER_CNTRL_CNT_MODE_Pos)
#define TIMER_CNTRL_CNT_MODE_EVNT_FIXED_DIR (0x2UL << TIMER_CNTRL_CNT_MODE_Pos)

#define TIMER_CNTRL_EVNT_SEL_Pos            8
#define TIMER_CNTRL_EVNT_SEL_Msk            (0xFUL << TIMER_CNTRL_EVNT_SEL_Pos)
#define TIMER_CNTRL_EVNT_SEL_TIM_CLK        (0x0UL << TIMER_CNTRL_EVNT_SEL_Pos)
#define TIMER_CNTRL_EVNT_SEL_TMR1           (0x1UL << TIMER_CNTRL_EVNT_SEL_Pos)
#define TIMER_CNTRL_EVNT_SEL_TMR2           (0x2UL << TIMER_CNTRL_EVNT_SEL_Pos)
#define TIMER_CNTRL_EVNT_SEL_TMR3           (0x3UL << TIMER_CNTRL_EVNT_SEL_Pos)
#define TIMER_CNTRL_EVNT_SEL_CH1_RE         (0x4UL << TIMER_CNTRL_EVNT_SEL_Pos)
#define TIMER_CNTRL_EVNT_SEL_CH2_RE         (0x5UL << TIMER_CNTRL_EVNT_SEL_Pos)
#define TIMER_CNTRL_EVNT_SEL_CH3_RE         (0x6UL << TIMER_CNTRL_EVNT_SEL_Pos)
#define TIMER_CNTRL_EVNT_SEL_CH4_RE         (0x7UL << TIMER_CNTRL_EVNT_SEL_Pos)
#define TIMER_CNTRL_EVNT_SEL_ETR_RE         (0x8UL << TIMER_CNTRL_EVNT_SEL_Pos)
#define TIMER_CNTRL_EVNT_SEL_ETR_FE         (0x9UL << TIMER_CNTRL_EVNT_SEL_Pos)
#define TIMER_CNTRL_EVNT_SEL_TMR4           (0xAUL << TIMER_CNTRL_EVNT_SEL_Pos)

/** @} */ /* End of group MDR32VF0xI_TIMER_CNTRL */

/** @defgroup MDR32VF0xI_TIMER_CH_CCR TIMER CH_CCR
 * @{
 */

#define TIMER_CH_CCR_CCR_Pos 0
#define TIMER_CH_CCR_CCR_Msk (0xFFFFFFFFUL << TIMER_CH_CCR_CCR_Pos)

/** @} */ /* End of group MDR32VF0xI_TIMER_CH_CCR */

/** @defgroup MDR32VF0xI_TIMER_CH_CNTRL0 TIMER CH_CNTRL0
 * @{
 */

#define TIMER_CH_CNTRL0_CH_FLTR_Pos           0
#define TIMER_CH_CNTRL0_CH_FLTR_Msk           (0xFUL << TIMER_CH_CNTRL0_CH_FLTR_Pos)
#define TIMER_CH_CNTRL0_CH_FLTR_1_FDTS        (0x0UL << TIMER_CH_CNTRL0_CH_FLTR_Pos)
#define TIMER_CH_CNTRL0_CH_FLTR_2_TIM_CLK     (0x1UL << TIMER_CH_CNTRL0_CH_FLTR_Pos)
#define TIMER_CH_CNTRL0_CH_FLTR_4_TIM_CLK     (0x2UL << TIMER_CH_CNTRL0_CH_FLTR_Pos)
#define TIMER_CH_CNTRL0_CH_FLTR_8_TIM_CLK     (0x3UL << TIMER_CH_CNTRL0_CH_FLTR_Pos)
#define TIMER_CH_CNTRL0_CH_FLTR_6_FDTS_DIV_2  (0x4UL << TIMER_CH_CNTRL0_CH_FLTR_Pos)
#define TIMER_CH_CNTRL0_CH_FLTR_8_FDTS_DIV_2  (0x5UL << TIMER_CH_CNTRL0_CH_FLTR_Pos)
#define TIMER_CH_CNTRL0_CH_FLTR_6_FDTS_DIV_4  (0x6UL << TIMER_CH_CNTRL0_CH_FLTR_Pos)
#define TIMER_CH_CNTRL0_CH_FLTR_8_FDTS_DIV_4  (0x7UL << TIMER_CH_CNTRL0_CH_FLTR_Pos)
#define TIMER_CH_CNTRL0_CH_FLTR_6_FDTS_DIV_8  (0x8UL << TIMER_CH_CNTRL0_CH_FLTR_Pos)
#define TIMER_CH_CNTRL0_CH_FLTR_8_FDTS_DIV_8  (0x9UL << TIMER_CH_CNTRL0_CH_FLTR_Pos)
#define TIMER_CH_CNTRL0_CH_FLTR_5_FDTS_DIV_16 (0xAUL << TIMER_CH_CNTRL0_CH_FLTR_Pos)
#define TIMER_CH_CNTRL0_CH_FLTR_6_FDTS_DIV_16 (0xBUL << TIMER_CH_CNTRL0_CH_FLTR_Pos)
#define TIMER_CH_CNTRL0_CH_FLTR_8_FDTS_DIV_16 (0xCUL << TIMER_CH_CNTRL0_CH_FLTR_Pos)
#define TIMER_CH_CNTRL0_CH_FLTR_5_FDTS_DIV_32 (0xDUL << TIMER_CH_CNTRL0_CH_FLTR_Pos)
#define TIMER_CH_CNTRL0_CH_FLTR_6_FDTS_DIV_32 (0xEUL << TIMER_CH_CNTRL0_CH_FLTR_Pos)
#define TIMER_CH_CNTRL0_CH_FLTR_8_FDTS_DIV_32 (0xFUL << TIMER_CH_CNTRL0_CH_FLTR_Pos)

#define TIMER_CH_CNTRL0_CH_SEL_Pos            4
#define TIMER_CH_CNTRL0_CH_SEL_Msk            (0x3UL << TIMER_CH_CNTRL0_CH_SEL_Pos)
#define TIMER_CH_CNTRL0_CH_SEL_CH_RE          (0x0UL << TIMER_CH_CNTRL0_CH_SEL_Pos)
#define TIMER_CH_CNTRL0_CH_SEL_CH_FE          (0x1UL << TIMER_CH_CNTRL0_CH_SEL_Pos)
#define TIMER_CH_CNTRL0_CH_SEL_CHp1_RE        (0x2UL << TIMER_CH_CNTRL0_CH_SEL_Pos)
#define TIMER_CH_CNTRL0_CH_SEL_CHp2_RE        (0x3UL << TIMER_CH_CNTRL0_CH_SEL_Pos)

#define TIMER_CH_CNTRL0_CH_PSC_Pos            6
#define TIMER_CH_CNTRL0_CH_PSC_Msk            (0x3UL << TIMER_CH_CNTRL0_CH_PSC_Pos)
#define TIMER_CH_CNTRL0_CH_PSC_DIV_1          (0x0UL << TIMER_CH_CNTRL0_CH_PSC_Pos)
#define TIMER_CH_CNTRL0_CH_PSC_DIV_2          (0x1UL << TIMER_CH_CNTRL0_CH_PSC_Pos)
#define TIMER_CH_CNTRL0_CH_PSC_DIV_4          (0x2UL << TIMER_CH_CNTRL0_CH_PSC_Pos)
#define TIMER_CH_CNTRL0_CH_PSC_DIV_8          (0x3UL << TIMER_CH_CNTRL0_CH_PSC_Pos)

#define TIMER_CH_CNTRL0_OCCE_Pos              8
#define TIMER_CH_CNTRL0_OCCE_Msk              (0x1UL << TIMER_CH_CNTRL0_OCCE_Pos)
#define TIMER_CH_CNTRL0_OCCE                  TIMER_CH_CNTRL0_OCCE_Msk

#define TIMER_CH_CNTRL0_OCCM_Pos              9
#define TIMER_CH_CNTRL0_OCCM_Msk              (0x7UL << TIMER_CH_CNTRL0_OCCM_Pos)
#define TIMER_CH_CNTRL0_OCCM_FORMAT_0         (0x0UL << TIMER_CH_CNTRL0_OCCM_Pos)
#define TIMER_CH_CNTRL0_OCCM_FORMAT_1         (0x1UL << TIMER_CH_CNTRL0_OCCM_Pos)
#define TIMER_CH_CNTRL0_OCCM_FORMAT_2         (0x2UL << TIMER_CH_CNTRL0_OCCM_Pos)
#define TIMER_CH_CNTRL0_OCCM_FORMAT_3         (0x3UL << TIMER_CH_CNTRL0_OCCM_Pos)
#define TIMER_CH_CNTRL0_OCCM_FORMAT_4         (0x4UL << TIMER_CH_CNTRL0_OCCM_Pos)
#define TIMER_CH_CNTRL0_OCCM_FORMAT_5         (0x5UL << TIMER_CH_CNTRL0_OCCM_Pos)
#define TIMER_CH_CNTRL0_OCCM_FORMAT_6         (0x6UL << TIMER_CH_CNTRL0_OCCM_Pos)
#define TIMER_CH_CNTRL0_OCCM_FORMAT_7         (0x7UL << TIMER_CH_CNTRL0_OCCM_Pos)

#define TIMER_CH_CNTRL0_BRK_EN_Pos            12
#define TIMER_CH_CNTRL0_BRK_EN_Msk            (0x1UL << TIMER_CH_CNTRL0_BRK_EN_Pos)
#define TIMER_CH_CNTRL0_BRK_EN                TIMER_CH_CNTRL0_BRK_EN_Msk

#define TIMER_CH_CNTRL0_ETR_EN_Pos            13
#define TIMER_CH_CNTRL0_ETR_EN_Msk            (0x1UL << TIMER_CH_CNTRL0_ETR_EN_Pos)
#define TIMER_CH_CNTRL0_ETR_EN                TIMER_CH_CNTRL0_ETR_EN_Msk

#define TIMER_CH_CNTRL0_WR_CMPL_Pos           14
#define TIMER_CH_CNTRL0_WR_CMPL_Msk           (0x1UL << TIMER_CH_CNTRL0_WR_CMPL_Pos)
#define TIMER_CH_CNTRL0_WR_CMPL               TIMER_CH_CNTRL0_WR_CMPL_Msk

#define TIMER_CH_CNTRL0_CAP_NPWM_Pos          15
#define TIMER_CH_CNTRL0_CAP_NPWM_Msk          (0x1UL << TIMER_CH_CNTRL0_CAP_NPWM_Pos)
#define TIMER_CH_CNTRL0_CAP_NPWM              TIMER_CH_CNTRL0_CAP_NPWM_Msk

#define TIMER_CH_CNTRL0_WR_CMPL1_Pos          16
#define TIMER_CH_CNTRL0_WR_CMPL1_Msk          (0x1UL << TIMER_CH_CNTRL0_WR_CMPL1_Pos)
#define TIMER_CH_CNTRL0_WR_CMPL1              TIMER_CH_CNTRL0_WR_CMPL1_Msk

/** @} */ /* End of group MDR32VF0xI_TIMER_CH_CNTRL0 */

/** @defgroup MDR32VF0xI_TIMER_CH_CNTRL1 TIMER CH_CNTRL1
 * @{
 */

#define TIMER_CH_CNTRL1_SELOE_Pos   0
#define TIMER_CH_CNTRL1_SELOE_Msk   (0x3UL << TIMER_CH_CNTRL1_SELOE_Pos)
#define TIMER_CH_CNTRL1_SELOE_ALW0  (0x0UL << TIMER_CH_CNTRL1_SELOE_Pos)
#define TIMER_CH_CNTRL1_SELOE_ALW1  (0x1UL << TIMER_CH_CNTRL1_SELOE_Pos)
#define TIMER_CH_CNTRL1_SELOE_REF   (0x2UL << TIMER_CH_CNTRL1_SELOE_Pos)
#define TIMER_CH_CNTRL1_SELOE_DTG   (0x3UL << TIMER_CH_CNTRL1_SELOE_Pos)

#define TIMER_CH_CNTRL1_SELO_Pos    2
#define TIMER_CH_CNTRL1_SELO_Msk    (0x3UL << TIMER_CH_CNTRL1_SELO_Pos)
#define TIMER_CH_CNTRL1_SELO_ALW0   (0x0UL << TIMER_CH_CNTRL1_SELO_Pos)
#define TIMER_CH_CNTRL1_SELO_ALW1   (0x1UL << TIMER_CH_CNTRL1_SELO_Pos)
#define TIMER_CH_CNTRL1_SELO_REF    (0x2UL << TIMER_CH_CNTRL1_SELO_Pos)
#define TIMER_CH_CNTRL1_SELO_DTG    (0x3UL << TIMER_CH_CNTRL1_SELO_Pos)

#define TIMER_CH_CNTRL1_INV_Pos     4
#define TIMER_CH_CNTRL1_INV_Msk     (0x1UL << TIMER_CH_CNTRL1_INV_Pos)
#define TIMER_CH_CNTRL1_INV         TIMER_CH_CNTRL1_INV_Msk

#define TIMER_CH_CNTRL1_NSELOE_Pos  8
#define TIMER_CH_CNTRL1_NSELOE_Msk  (0x3UL << TIMER_CH_CNTRL1_NSELOE_Pos)
#define TIMER_CH_CNTRL1_NSELOE_ALW0 (0x0UL << TIMER_CH_CNTRL1_NSELOE_Pos)
#define TIMER_CH_CNTRL1_NSELOE_ALW1 (0x1UL << TIMER_CH_CNTRL1_NSELOE_Pos)
#define TIMER_CH_CNTRL1_NSELOE_REF  (0x2UL << TIMER_CH_CNTRL1_NSELOE_Pos)
#define TIMER_CH_CNTRL1_NSELOE_DTG  (0x3UL << TIMER_CH_CNTRL1_NSELOE_Pos)

#define TIMER_CH_CNTRL1_NSELO_Pos   10
#define TIMER_CH_CNTRL1_NSELO_Msk   (0x3UL << TIMER_CH_CNTRL1_NSELO_Pos)
#define TIMER_CH_CNTRL1_NSELO_ALW0  (0x0UL << TIMER_CH_CNTRL1_NSELO_Pos)
#define TIMER_CH_CNTRL1_NSELO_ALW1  (0x1UL << TIMER_CH_CNTRL1_NSELO_Pos)
#define TIMER_CH_CNTRL1_NSELO_REF   (0x2UL << TIMER_CH_CNTRL1_NSELO_Pos)
#define TIMER_CH_CNTRL1_NSELO_DTG   (0x3UL << TIMER_CH_CNTRL1_NSELO_Pos)

#define TIMER_CH_CNTRL1_NINV_Pos    12
#define TIMER_CH_CNTRL1_NINV_Msk    (0x1UL << TIMER_CH_CNTRL1_NINV_Pos)
#define TIMER_CH_CNTRL1_NINV        TIMER_CH_CNTRL1_NINV_Msk

/** @} */ /* End of group MDR32VF0xI_TIMER_CH_CNTRL1 */

/** @defgroup MDR32VF0xI_TIMER_CH_DTG TIMER CH_DTG
 * @{
 */

#define TIMER_CH_DTG_DTGX_Pos 0
#define TIMER_CH_DTG_DTGX_Msk (0xFUL << TIMER_CH_DTG_DTGX_Pos)

#define TIMER_CH_DTG_EDTS_Pos 4
#define TIMER_CH_DTG_EDTS_Msk (0x1UL << TIMER_CH_DTG_EDTS_Pos)
#define TIMER_CH_DTG_EDTS     TIMER_CH_DTG_EDTS_Msk

#define TIMER_CH_DTG_DTG_Pos  8
#define TIMER_CH_DTG_DTG_Msk  (0xFFUL << TIMER_CH_DTG_DTG_Pos)

/** @} */ /* End of group MDR32VF0xI_TIMER_CH_DTG */

/** @defgroup MDR32VF0xI_TIMER_BRKETR_CNTRL TIMER BRKETR_CNTRL
 * @{
 */

#define TIMER_BRKETR_CNTRL_BRK_INV_Pos            0
#define TIMER_BRKETR_CNTRL_BRK_INV_Msk            (0x1UL << TIMER_BRKETR_CNTRL_BRK_INV_Pos)
#define TIMER_BRKETR_CNTRL_BRK_INV                TIMER_BRKETR_CNTRL_BRK_INV_Msk

#define TIMER_BRKETR_CNTRL_ETR_INV_Pos            1
#define TIMER_BRKETR_CNTRL_ETR_INV_Msk            (0x1UL << TIMER_BRKETR_CNTRL_ETR_INV_Pos)
#define TIMER_BRKETR_CNTRL_ETR_INV                TIMER_BRKETR_CNTRL_ETR_INV_Msk

#define TIMER_BRKETR_CNTRL_ETR_PSC_Pos            2
#define TIMER_BRKETR_CNTRL_ETR_PSC_Msk            (0x3UL << TIMER_BRKETR_CNTRL_ETR_PSC_Pos)
#define TIMER_BRKETR_CNTRL_ETR_PSC_DIV_1          (0x0UL << TIMER_BRKETR_CNTRL_ETR_PSC_Pos)
#define TIMER_BRKETR_CNTRL_ETR_PSC_DIV_2          (0x1UL << TIMER_BRKETR_CNTRL_ETR_PSC_Pos)
#define TIMER_BRKETR_CNTRL_ETR_PSC_DIV_4          (0x2UL << TIMER_BRKETR_CNTRL_ETR_PSC_Pos)
#define TIMER_BRKETR_CNTRL_ETR_PSC_DIV_8          (0x3UL << TIMER_BRKETR_CNTRL_ETR_PSC_Pos)

#define TIMER_BRKETR_CNTRL_ETR_FLTR_Pos           4
#define TIMER_BRKETR_CNTRL_ETR_FLTR_Msk           (0xFUL << TIMER_BRKETR_CNTRL_ETR_FLTR_Pos)
#define TIMER_BRKETR_CNTRL_ETR_FLTR_1_FDTS        (0x0UL << TIMER_BRKETR_CNTRL_ETR_FLTR_Pos)
#define TIMER_BRKETR_CNTRL_ETR_FLTR_2_TIM_CLK     (0x1UL << TIMER_BRKETR_CNTRL_ETR_FLTR_Pos)
#define TIMER_BRKETR_CNTRL_ETR_FLTR_4_TIM_CLK     (0x2UL << TIMER_BRKETR_CNTRL_ETR_FLTR_Pos)
#define TIMER_BRKETR_CNTRL_ETR_FLTR_8_TIM_CLK     (0x3UL << TIMER_BRKETR_CNTRL_ETR_FLTR_Pos)
#define TIMER_BRKETR_CNTRL_ETR_FLTR_6_FDTS_DIV_2  (0x4UL << TIMER_BRKETR_CNTRL_ETR_FLTR_Pos)
#define TIMER_BRKETR_CNTRL_ETR_FLTR_8_FDTS_DIV_2  (0x5UL << TIMER_BRKETR_CNTRL_ETR_FLTR_Pos)
#define TIMER_BRKETR_CNTRL_ETR_FLTR_6_FDTS_DIV_4  (0x6UL << TIMER_BRKETR_CNTRL_ETR_FLTR_Pos)
#define TIMER_BRKETR_CNTRL_ETR_FLTR_8_FDTS_DIV_4  (0x7UL << TIMER_BRKETR_CNTRL_ETR_FLTR_Pos)
#define TIMER_BRKETR_CNTRL_ETR_FLTR_6_FDTS_DIV_8  (0x8UL << TIMER_BRKETR_CNTRL_ETR_FLTR_Pos)
#define TIMER_BRKETR_CNTRL_ETR_FLTR_8_FDTS_DIV_8  (0x9UL << TIMER_BRKETR_CNTRL_ETR_FLTR_Pos)
#define TIMER_BRKETR_CNTRL_ETR_FLTR_5_FDTS_DIV_16 (0xAUL << TIMER_BRKETR_CNTRL_ETR_FLTR_Pos)
#define TIMER_BRKETR_CNTRL_ETR_FLTR_6_FDTS_DIV_16 (0xBUL << TIMER_BRKETR_CNTRL_ETR_FLTR_Pos)
#define TIMER_BRKETR_CNTRL_ETR_FLTR_8_FDTS_DIV_16 (0xCUL << TIMER_BRKETR_CNTRL_ETR_FLTR_Pos)
#define TIMER_BRKETR_CNTRL_ETR_FLTR_5_FDTS_DIV_32 (0xDUL << TIMER_BRKETR_CNTRL_ETR_FLTR_Pos)
#define TIMER_BRKETR_CNTRL_ETR_FLTR_6_FDTS_DIV_32 (0xEUL << TIMER_BRKETR_CNTRL_ETR_FLTR_Pos)
#define TIMER_BRKETR_CNTRL_ETR_FLTR_8_FDTS_DIV_32 (0xFUL << TIMER_BRKETR_CNTRL_ETR_FLTR_Pos)

/** @} */ /* End of group MDR32VF0xI_TIMER_BRKETR_CNTRL */

/** @defgroup MDR32VF0xI_TIMER_STATUS TIMER STATUS
 * @{
 */

#define TIMER_STATUS_CNT_ZERO_EVNT_Pos     0
#define TIMER_STATUS_CNT_ZERO_EVNT_Msk     (0x1UL << TIMER_STATUS_CNT_ZERO_EVNT_Pos)
#define TIMER_STATUS_CNT_ZERO_EVNT         TIMER_STATUS_CNT_ZERO_EVNT_Msk

#define TIMER_STATUS_CNT_ARR_EVNT_Pos      1
#define TIMER_STATUS_CNT_ARR_EVNT_Msk      (0x1UL << TIMER_STATUS_CNT_ARR_EVNT_Pos)
#define TIMER_STATUS_CNT_ARR_EVNT          TIMER_STATUS_CNT_ARR_EVNT_Msk

#define TIMER_STATUS_ETR_RE_EVNT_Pos       2
#define TIMER_STATUS_ETR_RE_EVNT_Msk       (0x1UL << TIMER_STATUS_ETR_RE_EVNT_Pos)
#define TIMER_STATUS_ETR_RE_EVNT           TIMER_STATUS_ETR_RE_EVNT_Msk

#define TIMER_STATUS_ETR_FE_EVNT_Pos       3
#define TIMER_STATUS_ETR_FE_EVNT_Msk       (0x1UL << TIMER_STATUS_ETR_FE_EVNT_Pos)
#define TIMER_STATUS_ETR_FE_EVNT           TIMER_STATUS_ETR_FE_EVNT_Msk

#define TIMER_STATUS_BRK_EVNT_Pos          4
#define TIMER_STATUS_BRK_EVNT_Msk          (0x1UL << TIMER_STATUS_BRK_EVNT_Pos)
#define TIMER_STATUS_BRK_EVNT              TIMER_STATUS_BRK_EVNT_Msk

#define TIMER_STATUS_CCR_CAP_EVNT_CH1_Pos  5
#define TIMER_STATUS_CCR_CAP_EVNT_CH1_Msk  (0x1UL << TIMER_STATUS_CCR_CAP_EVNT_CH1_Pos)
#define TIMER_STATUS_CCR_CAP_EVNT_CH1      TIMER_STATUS_CCR_CAP_EVNT_CH1_Msk

#define TIMER_STATUS_CCR_CAP_EVNT_CH2_Pos  6
#define TIMER_STATUS_CCR_CAP_EVNT_CH2_Msk  (0x1UL << TIMER_STATUS_CCR_CAP_EVNT_CH2_Pos)
#define TIMER_STATUS_CCR_CAP_EVNT_CH2      TIMER_STATUS_CCR_CAP_EVNT_CH2_Msk

#define TIMER_STATUS_CCR_CAP_EVNT_CH3_Pos  7
#define TIMER_STATUS_CCR_CAP_EVNT_CH3_Msk  (0x1UL << TIMER_STATUS_CCR_CAP_EVNT_CH3_Pos)
#define TIMER_STATUS_CCR_CAP_EVNT_CH3      TIMER_STATUS_CCR_CAP_EVNT_CH3_Msk

#define TIMER_STATUS_CCR_CAP_EVNT_CH4_Pos  8
#define TIMER_STATUS_CCR_CAP_EVNT_CH4_Msk  (0x1UL << TIMER_STATUS_CCR_CAP_EVNT_CH4_Pos)
#define TIMER_STATUS_CCR_CAP_EVNT_CH4      TIMER_STATUS_CCR_CAP_EVNT_CH4_Msk

#define TIMER_STATUS_CCR_REF_EVNT_CH1_Pos  9
#define TIMER_STATUS_CCR_REF_EVNT_CH1_Msk  (0x1UL << TIMER_STATUS_CCR_REF_EVNT_CH1_Pos)
#define TIMER_STATUS_CCR_REF_EVNT_CH1      TIMER_STATUS_CCR_REF_EVNT_CH1_Msk

#define TIMER_STATUS_CCR_REF_EVNT_CH2_Pos  10
#define TIMER_STATUS_CCR_REF_EVNT_CH2_Msk  (0x1UL << TIMER_STATUS_CCR_REF_EVNT_CH2_Pos)
#define TIMER_STATUS_CCR_REF_EVNT_CH2      TIMER_STATUS_CCR_REF_EVNT_CH2_Msk

#define TIMER_STATUS_CCR_REF_EVNT_CH3_Pos  11
#define TIMER_STATUS_CCR_REF_EVNT_CH3_Msk  (0x1UL << TIMER_STATUS_CCR_REF_EVNT_CH3_Pos)
#define TIMER_STATUS_CCR_REF_EVNT_CH3      TIMER_STATUS_CCR_REF_EVNT_CH3_Msk

#define TIMER_STATUS_CCR_REF_EVNT_CH4_Pos  12
#define TIMER_STATUS_CCR_REF_EVNT_CH4_Msk  (0x1UL << TIMER_STATUS_CCR_REF_EVNT_CH4_Pos)
#define TIMER_STATUS_CCR_REF_EVNT_CH4      TIMER_STATUS_CCR_REF_EVNT_CH4_Msk

#define TIMER_STATUS_CCR_CAP1_EVNT_CH1_Pos 13
#define TIMER_STATUS_CCR_CAP1_EVNT_CH1_Msk (0x1UL << TIMER_STATUS_CCR_CAP1_EVNT_CH1_Pos)
#define TIMER_STATUS_CCR_CAP1_EVNT_CH1     TIMER_STATUS_CCR_CAP1_EVNT_CH1_Msk

#define TIMER_STATUS_CCR_CAP1_EVNT_CH2_Pos 14
#define TIMER_STATUS_CCR_CAP1_EVNT_CH2_Msk (0x1UL << TIMER_STATUS_CCR_CAP1_EVNT_CH2_Pos)
#define TIMER_STATUS_CCR_CAP1_EVNT_CH2     TIMER_STATUS_CCR_CAP1_EVNT_CH2_Msk

#define TIMER_STATUS_CCR_CAP1_EVNT_CH3_Pos 15
#define TIMER_STATUS_CCR_CAP1_EVNT_CH3_Msk (0x1UL << TIMER_STATUS_CCR_CAP1_EVNT_CH3_Pos)
#define TIMER_STATUS_CCR_CAP1_EVNT_CH3     TIMER_STATUS_CCR_CAP1_EVNT_CH3_Msk

#define TIMER_STATUS_CCR_CAP1_EVNT_CH4_Pos 16
#define TIMER_STATUS_CCR_CAP1_EVNT_CH4_Msk (0x1UL << TIMER_STATUS_CCR_CAP1_EVNT_CH4_Pos)
#define TIMER_STATUS_CCR_CAP1_EVNT_CH4     TIMER_STATUS_CCR_CAP1_EVNT_CH4_Msk

/** @} */ /* End of group MDR32VF0xI_TIMER_STATUS */

/** @defgroup MDR32VF0xI_TIMER_IE TIMER IE
 * @{
 */

#define TIMER_IE_CNT_ZERO_EVNT_IE_Pos     0
#define TIMER_IE_CNT_ZERO_EVNT_IE_Msk     (0x1UL << TIMER_IE_CNT_ZERO_EVNT_IE_Pos)
#define TIMER_IE_CNT_ZERO_EVNT_IE         TIMER_IE_CNT_ZERO_EVNT_IE_Msk

#define TIMER_IE_CNT_ARR_EVNT_IE_Pos      1
#define TIMER_IE_CNT_ARR_EVNT_IE_Msk      (0x1UL << TIMER_IE_CNT_ARR_EVNT_IE_Pos)
#define TIMER_IE_CNT_ARR_EVNT_IE          TIMER_IE_CNT_ARR_EVNT_IE_Msk

#define TIMER_IE_ETR_RE_EVNT_IE_Pos       2
#define TIMER_IE_ETR_RE_EVNT_IE_Msk       (0x1UL << TIMER_IE_ETR_RE_EVNT_IE_Pos)
#define TIMER_IE_ETR_RE_EVNT_IE           TIMER_IE_ETR_RE_EVNT_IE_Msk

#define TIMER_IE_ETR_FE_EVNT_IE_Pos       3
#define TIMER_IE_ETR_FE_EVNT_IE_Msk       (0x1UL << TIMER_IE_ETR_FE_EVNT_IE_Pos)
#define TIMER_IE_ETR_FE_EVNT_IE           TIMER_IE_ETR_FE_EVNT_IE_Msk

#define TIMER_IE_BRK_EVNT_IE_Pos          4
#define TIMER_IE_BRK_EVNT_IE_Msk          (0x1UL << TIMER_IE_BRK_EVNT_IE_Pos)
#define TIMER_IE_BRK_EVNT_IE              TIMER_IE_BRK_EVNT_IE_Msk

#define TIMER_IE_CCR_CAP_EVNT_IE_CH1_Pos  5
#define TIMER_IE_CCR_CAP_EVNT_IE_CH1_Msk  (0x1UL << TIMER_IE_CCR_CAP_EVNT_IE_CH1_Pos)
#define TIMER_IE_CCR_CAP_EVNT_IE_CH1      TIMER_IE_CCR_CAP_EVNT_IE_CH1_Msk

#define TIMER_IE_CCR_CAP_EVNT_IE_CH2_Pos  6
#define TIMER_IE_CCR_CAP_EVNT_IE_CH2_Msk  (0x1UL << TIMER_IE_CCR_CAP_EVNT_IE_CH2_Pos)
#define TIMER_IE_CCR_CAP_EVNT_IE_CH2      TIMER_IE_CCR_CAP_EVNT_IE_CH2_Msk

#define TIMER_IE_CCR_CAP_EVNT_IE_CH3_Pos  7
#define TIMER_IE_CCR_CAP_EVNT_IE_CH3_Msk  (0x1UL << TIMER_IE_CCR_CAP_EVNT_IE_CH3_Pos)
#define TIMER_IE_CCR_CAP_EVNT_IE_CH3      TIMER_IE_CCR_CAP_EVNT_IE_CH3_Msk

#define TIMER_IE_CCR_CAP_EVNT_IE_CH4_Pos  8
#define TIMER_IE_CCR_CAP_EVNT_IE_CH4_Msk  (0x1UL << TIMER_IE_CCR_CAP_EVNT_IE_CH4_Pos)
#define TIMER_IE_CCR_CAP_EVNT_IE_CH4      TIMER_IE_CCR_CAP_EVNT_IE_CH4_Msk

#define TIMER_IE_CCR_REF_EVNT_IE_CH1_Pos  9
#define TIMER_IE_CCR_REF_EVNT_IE_CH1_Msk  (0x1UL << TIMER_IE_CCR_REF_EVNT_IE_CH1_Pos)
#define TIMER_IE_CCR_REF_EVNT_IE_CH1      TIMER_IE_CCR_REF_EVNT_IE_CH1_Msk

#define TIMER_IE_CCR_REF_EVNT_IE_CH2_Pos  10
#define TIMER_IE_CCR_REF_EVNT_IE_CH2_Msk  (0x1UL << TIMER_IE_CCR_REF_EVNT_IE_CH2_Pos)
#define TIMER_IE_CCR_REF_EVNT_IE_CH2      TIMER_IE_CCR_REF_EVNT_IE_CH2_Msk

#define TIMER_IE_CCR_REF_EVNT_IE_CH3_Pos  11
#define TIMER_IE_CCR_REF_EVNT_IE_CH3_Msk  (0x1UL << TIMER_IE_CCR_REF_EVNT_IE_CH3_Pos)
#define TIMER_IE_CCR_REF_EVNT_IE_CH3      TIMER_IE_CCR_REF_EVNT_IE_CH3_Msk

#define TIMER_IE_CCR_REF_EVNT_IE_CH4_Pos  12
#define TIMER_IE_CCR_REF_EVNT_IE_CH4_Msk  (0x1UL << TIMER_IE_CCR_REF_EVNT_IE_CH4_Pos)
#define TIMER_IE_CCR_REF_EVNT_IE_CH4      TIMER_IE_CCR_REF_EVNT_IE_CH4_Msk

#define TIMER_IE_CCR_CAP1_EVNT_IE_CH1_Pos 13
#define TIMER_IE_CCR_CAP1_EVNT_IE_CH1_Msk (0x1UL << TIMER_IE_CCR_CAP1_EVNT_IE_CH1_Pos)
#define TIMER_IE_CCR_CAP1_EVNT_IE_CH1     TIMER_IE_CCR_CAP1_EVNT_IE_CH1_Msk

#define TIMER_IE_CCR_CAP1_EVNT_IE_CH2_Pos 14
#define TIMER_IE_CCR_CAP1_EVNT_IE_CH2_Msk (0x1UL << TIMER_IE_CCR_CAP1_EVNT_IE_CH2_Pos)
#define TIMER_IE_CCR_CAP1_EVNT_IE_CH2     TIMER_IE_CCR_CAP1_EVNT_IE_CH2_Msk

#define TIMER_IE_CCR_CAP1_EVNT_IE_CH3_Pos 15
#define TIMER_IE_CCR_CAP1_EVNT_IE_CH3_Msk (0x1UL << TIMER_IE_CCR_CAP1_EVNT_IE_CH3_Pos)
#define TIMER_IE_CCR_CAP1_EVNT_IE_CH3     TIMER_IE_CCR_CAP1_EVNT_IE_CH3_Msk

#define TIMER_IE_CCR_CAP1_EVNT_IE_CH4_Pos 16
#define TIMER_IE_CCR_CAP1_EVNT_IE_CH4_Msk (0x1UL << TIMER_IE_CCR_CAP1_EVNT_IE_CH4_Pos)
#define TIMER_IE_CCR_CAP1_EVNT_IE_CH4     TIMER_IE_CCR_CAP1_EVNT_IE_CH4_Msk

/** @} */ /* End of group MDR32VF0xI_TIMER_IE */

/** @defgroup MDR32VF0xI_TIMER_DMA_RE TIMER DMA_RE
 * @{
 */

#define TIMER_DMA_RE_CNT_ZERO_EVNT_RE_Pos     0
#define TIMER_DMA_RE_CNT_ZERO_EVNT_RE_Msk     (0x1UL << TIMER_DMA_RE_CNT_ZERO_EVNT_RE_Pos)
#define TIMER_DMA_RE_CNT_ZERO_EVNT_RE         TIMER_DMA_RE_CNT_ZERO_EVNT_RE_Msk

#define TIMER_DMA_RE_CNT_ARR_EVNT_RE_Pos      1
#define TIMER_DMA_RE_CNT_ARR_EVNT_RE_Msk      (0x1UL << TIMER_DMA_RE_CNT_ARR_EVNT_RE_Pos)
#define TIMER_DMA_RE_CNT_ARR_EVNT_RE          TIMER_DMA_RE_CNT_ARR_EVNT_RE_Msk

#define TIMER_DMA_RE_ETR_RE_EVNT_RE_Pos       2
#define TIMER_DMA_RE_ETR_RE_EVNT_RE_Msk       (0x1UL << TIMER_DMA_RE_ETR_RE_EVNT_RE_Pos)
#define TIMER_DMA_RE_ETR_RE_EVNT_RE           TIMER_DMA_RE_ETR_RE_EVNT_RE_Msk

#define TIMER_DMA_RE_ETR_FE_EVNT_RE_Pos       3
#define TIMER_DMA_RE_ETR_FE_EVNT_RE_Msk       (0x1UL << TIMER_DMA_RE_ETR_FE_EVNT_RE_Pos)
#define TIMER_DMA_RE_ETR_FE_EVNT_RE           TIMER_DMA_RE_ETR_FE_EVNT_RE_Msk

#define TIMER_DMA_RE_BRK_EVNT_RE_Pos          4
#define TIMER_DMA_RE_BRK_EVNT_RE_Msk          (0x1UL << TIMER_DMA_RE_BRK_EVNT_RE_Pos)
#define TIMER_DMA_RE_BRK_EVNT_RE              TIMER_DMA_RE_BRK_EVNT_RE_Msk

#define TIMER_DMA_RE_CCR_CAP_EVNT_RE_CH1_Pos  5
#define TIMER_DMA_RE_CCR_CAP_EVNT_RE_CH1_Msk  (0x1UL << TIMER_DMA_RE_CCR_CAP_EVNT_RE_CH1_Pos)
#define TIMER_DMA_RE_CCR_CAP_EVNT_RE_CH1      TIMER_DMA_RE_CCR_CAP_EVNT_RE_CH1_Msk

#define TIMER_DMA_RE_CCR_CAP_EVNT_RE_CH2_Pos  6
#define TIMER_DMA_RE_CCR_CAP_EVNT_RE_CH2_Msk  (0x1UL << TIMER_DMA_RE_CCR_CAP_EVNT_RE_CH2_Pos)
#define TIMER_DMA_RE_CCR_CAP_EVNT_RE_CH2      TIMER_DMA_RE_CCR_CAP_EVNT_RE_CH2_Msk

#define TIMER_DMA_RE_CCR_CAP_EVNT_RE_CH3_Pos  7
#define TIMER_DMA_RE_CCR_CAP_EVNT_RE_CH3_Msk  (0x1UL << TIMER_DMA_RE_CCR_CAP_EVNT_RE_CH3_Pos)
#define TIMER_DMA_RE_CCR_CAP_EVNT_RE_CH3      TIMER_DMA_RE_CCR_CAP_EVNT_RE_CH3_Msk

#define TIMER_DMA_RE_CCR_CAP_EVNT_RE_CH4_Pos  8
#define TIMER_DMA_RE_CCR_CAP_EVNT_RE_CH4_Msk  (0x1UL << TIMER_DMA_RE_CCR_CAP_EVNT_RE_CH4_Pos)
#define TIMER_DMA_RE_CCR_CAP_EVNT_RE_CH4      TIMER_DMA_RE_CCR_CAP_EVNT_RE_CH4_Msk

#define TIMER_DMA_RE_CCR_REF_EVNT_RE_CH1_Pos  9
#define TIMER_DMA_RE_CCR_REF_EVNT_RE_CH1_Msk  (0x1UL << TIMER_DMA_RE_CCR_REF_EVNT_RE_CH1_Pos)
#define TIMER_DMA_RE_CCR_REF_EVNT_RE_CH1      TIMER_DMA_RE_CCR_REF_EVNT_RE_CH1_Msk

#define TIMER_DMA_RE_CCR_REF_EVNT_RE_CH2_Pos  10
#define TIMER_DMA_RE_CCR_REF_EVNT_RE_CH2_Msk  (0x1UL << TIMER_DMA_RE_CCR_REF_EVNT_RE_CH2_Pos)
#define TIMER_DMA_RE_CCR_REF_EVNT_RE_CH2      TIMER_DMA_RE_CCR_REF_EVNT_RE_CH2_Msk

#define TIMER_DMA_RE_CCR_REF_EVNT_RE_CH3_Pos  11
#define TIMER_DMA_RE_CCR_REF_EVNT_RE_CH3_Msk  (0x1UL << TIMER_DMA_RE_CCR_REF_EVNT_RE_CH3_Pos)
#define TIMER_DMA_RE_CCR_REF_EVNT_RE_CH3      TIMER_DMA_RE_CCR_REF_EVNT_RE_CH3_Msk

#define TIMER_DMA_RE_CCR_REF_EVNT_RE_CH4_Pos  12
#define TIMER_DMA_RE_CCR_REF_EVNT_RE_CH4_Msk  (0x1UL << TIMER_DMA_RE_CCR_REF_EVNT_RE_CH4_Pos)
#define TIMER_DMA_RE_CCR_REF_EVNT_RE_CH4      TIMER_DMA_RE_CCR_REF_EVNT_RE_CH4_Msk

#define TIMER_DMA_RE_CCR_CAP1_EVNT_RE_CH1_Pos 13
#define TIMER_DMA_RE_CCR_CAP1_EVNT_RE_CH1_Msk (0x1UL << TIMER_DMA_RE_CCR_CAP1_EVNT_RE_CH1_Pos)
#define TIMER_DMA_RE_CCR_CAP1_EVNT_RE_CH1     TIMER_DMA_RE_CCR_CAP1_EVNT_RE_CH1_Msk

#define TIMER_DMA_RE_CCR_CAP1_EVNT_RE_CH2_Pos 14
#define TIMER_DMA_RE_CCR_CAP1_EVNT_RE_CH2_Msk (0x1UL << TIMER_DMA_RE_CCR_CAP1_EVNT_RE_CH2_Pos)
#define TIMER_DMA_RE_CCR_CAP1_EVNT_RE_CH2     TIMER_DMA_RE_CCR_CAP1_EVNT_RE_CH2_Msk

#define TIMER_DMA_RE_CCR_CAP1_EVNT_RE_CH3_Pos 15
#define TIMER_DMA_RE_CCR_CAP1_EVNT_RE_CH3_Msk (0x1UL << TIMER_DMA_RE_CCR_CAP1_EVNT_RE_CH3_Pos)
#define TIMER_DMA_RE_CCR_CAP1_EVNT_RE_CH3     TIMER_DMA_RE_CCR_CAP1_EVNT_RE_CH3_Msk

#define TIMER_DMA_RE_CCR_CAP1_EVNT_RE_CH4_Pos 16
#define TIMER_DMA_RE_CCR_CAP1_EVNT_RE_CH4_Msk (0x1UL << TIMER_DMA_RE_CCR_CAP1_EVNT_RE_CH4_Pos)
#define TIMER_DMA_RE_CCR_CAP1_EVNT_RE_CH4     TIMER_DMA_RE_CCR_CAP1_EVNT_RE_CH4_Msk

/** @} */ /* End of group MDR32VF0xI_TIMER_DMA_RE */

/** @defgroup MDR32VF0xI_TIMER_CH_CNTRL2 TIMER CH_CNTRL2
 * @{
 */

#define TIMER_CH_CNTRL2_CH_SEL1_Pos     0
#define TIMER_CH_CNTRL2_CH_SEL1_Msk     (0x3UL << TIMER_CH_CNTRL2_CH_SEL1_Pos)
#define TIMER_CH_CNTRL2_CH_SEL1_CH_RE   (0x0UL << TIMER_CH_CNTRL2_CH_SEL1_Pos)
#define TIMER_CH_CNTRL2_CH_SEL1_CH_FE   (0x1UL << TIMER_CH_CNTRL2_CH_SEL1_Pos)
#define TIMER_CH_CNTRL2_CH_SEL1_CHp1_FE (0x2UL << TIMER_CH_CNTRL2_CH_SEL1_Pos)
#define TIMER_CH_CNTRL2_CH_SEL1_CHp2_FE (0x3UL << TIMER_CH_CNTRL2_CH_SEL1_Pos)

#define TIMER_CH_CNTRL2_CCR1_EN_Pos     2
#define TIMER_CH_CNTRL2_CCR1_EN_Msk     (0x1UL << TIMER_CH_CNTRL2_CCR1_EN_Pos)
#define TIMER_CH_CNTRL2_CCR1_EN         TIMER_CH_CNTRL2_CCR1_EN_Msk

#define TIMER_CH_CNTRL2_CCR_RLD_Pos     3
#define TIMER_CH_CNTRL2_CCR_RLD_Msk     (0x1UL << TIMER_CH_CNTRL2_CCR_RLD_Pos)
#define TIMER_CH_CNTRL2_CCR_RLD         TIMER_CH_CNTRL2_CCR_RLD_Msk

#define TIMER_CH_CNTRL2_EVNT_DLY_Pos    4
#define TIMER_CH_CNTRL2_EVNT_DLY_Msk    (0x1UL << TIMER_CH_CNTRL2_EVNT_DLY_Pos)
#define TIMER_CH_CNTRL2_EVNT_DLY        TIMER_CH_CNTRL2_EVNT_DLY_Msk

/** @} */ /* End of group MDR32VF0xI_TIMER_CH_CNTRL2 */

/** @defgroup MDR32VF0xI_TIMER_CH_CCR1 TIMER CH_CCR1
 * @{
 */

#define TIMER_CH_CCR1_CCR1_Pos 0
#define TIMER_CH_CCR1_CCR1_Msk (0xFFFFFFFFUL << TIMER_CH_CCR1_CCR1_Pos)

/** @} */ /* End of group MDR32VF0xI_TIMER_CH_CCR1 */

/** @} */ /* End of group MDR32VF0xI_TIMER_Defines */

/** @} */ /* End of group MDR32VF0xI_TIMER */

/** @defgroup MDR32VF0xI_PORT MDR_PORT
 * @{
 */

/**
 * @brief MDR32VF0xI PORT control structure.
 */
typedef struct {
    __IO uint32_t RXTX;      /*!<PORT Data Register */
    __IO uint32_t OE;        /*!<PORT Output Enable Register */
    __IO uint32_t FUNC;      /*!<PORT Function Register */
    __IO uint32_t ANALOG;    /*!<PORT Analog Function Register */
    __IO uint32_t PULL;      /*!<PORT Pull Up/Down Register */
    __I uint32_t  RESERVED0; /*!<Reserved */
    __IO uint32_t PWR;       /*!<PORT Power Register */
    __I uint32_t  RESERVED1; /*!<Reserved */
    __IO uint32_t SETTX;     /*!<PORT Set Output Register */
    __IO uint32_t CLRTX;     /*!<PORT Reset Output Register */
    __I uint32_t  RDTX;      /*!<PORT Read Output Register */
} MDR_PORT_TypeDef;

/** @defgroup MDR32VF0xI_PORT_Defines PORT Defines
 * @{
 */

/** @defgroup MDR32VF0xI_PORT_RXTX PORT RXTX
 * @{
 */

#define PORT_RXTX_RXTX_Pos   0
#define PORT_RXTX_RXTX_Msk   (0xFFFFUL << PORT_RXTX_RXTX_Pos)

#define PORT_RXTX_RXTX0_Pos  0
#define PORT_RXTX_RXTX0_Msk  (0x1UL << PORT_RXTX_RXTX0_Pos)
#define PORT_RXTX_RXTX0      PORT_RXTX_RXTX0_Msk

#define PORT_RXTX_RXTX1_Pos  1
#define PORT_RXTX_RXTX1_Msk  (0x1UL << PORT_RXTX_RXTX1_Pos)
#define PORT_RXTX_RXTX1      PORT_RXTX_RXTX1_Msk

#define PORT_RXTX_RXTX2_Pos  2
#define PORT_RXTX_RXTX2_Msk  (0x1UL << PORT_RXTX_RXTX2_Pos)
#define PORT_RXTX_RXTX2      PORT_RXTX_RXTX2_Msk

#define PORT_RXTX_RXTX3_Pos  3
#define PORT_RXTX_RXTX3_Msk  (0x1UL << PORT_RXTX_RXTX3_Pos)
#define PORT_RXTX_RXTX3      PORT_RXTX_RXTX3_Msk

#define PORT_RXTX_RXTX4_Pos  4
#define PORT_RXTX_RXTX4_Msk  (0x1UL << PORT_RXTX_RXTX4_Pos)
#define PORT_RXTX_RXTX4      PORT_RXTX_RXTX4_Msk

#define PORT_RXTX_RXTX5_Pos  5
#define PORT_RXTX_RXTX5_Msk  (0x1UL << PORT_RXTX_RXTX5_Pos)
#define PORT_RXTX_RXTX5      PORT_RXTX_RXTX5_Msk

#define PORT_RXTX_RXTX6_Pos  6
#define PORT_RXTX_RXTX6_Msk  (0x1UL << PORT_RXTX_RXTX6_Pos)
#define PORT_RXTX_RXTX6      PORT_RXTX_RXTX6_Msk

#define PORT_RXTX_RXTX7_Pos  7
#define PORT_RXTX_RXTX7_Msk  (0x1UL << PORT_RXTX_RXTX7_Pos)
#define PORT_RXTX_RXTX7      PORT_RXTX_RXTX7_Msk

#define PORT_RXTX_RXTX8_Pos  8
#define PORT_RXTX_RXTX8_Msk  (0x1UL << PORT_RXTX_RXTX8_Pos)
#define PORT_RXTX_RXTX8      PORT_RXTX_RXTX8_Msk

#define PORT_RXTX_RXTX9_Pos  9
#define PORT_RXTX_RXTX9_Msk  (0x1UL << PORT_RXTX_RXTX9_Pos)
#define PORT_RXTX_RXTX9      PORT_RXTX_RXTX9_Msk

#define PORT_RXTX_RXTX10_Pos 10
#define PORT_RXTX_RXTX10_Msk (0x1UL << PORT_RXTX_RXTX10_Pos)
#define PORT_RXTX_RXTX10     PORT_RXTX_RXTX10_Msk

#define PORT_RXTX_RXTX11_Pos 11
#define PORT_RXTX_RXTX11_Msk (0x1UL << PORT_RXTX_RXTX11_Pos)
#define PORT_RXTX_RXTX11     PORT_RXTX_RXTX11_Msk

#define PORT_RXTX_RXTX12_Pos 12
#define PORT_RXTX_RXTX12_Msk (0x1UL << PORT_RXTX_RXTX12_Pos)
#define PORT_RXTX_RXTX12     PORT_RXTX_RXTX12_Msk

#define PORT_RXTX_RXTX13_Pos 13
#define PORT_RXTX_RXTX13_Msk (0x1UL << PORT_RXTX_RXTX13_Pos)
#define PORT_RXTX_RXTX13     PORT_RXTX_RXTX13_Msk

#define PORT_RXTX_RXTX14_Pos 14
#define PORT_RXTX_RXTX14_Msk (0x1UL << PORT_RXTX_RXTX14_Pos)
#define PORT_RXTX_RXTX14     PORT_RXTX_RXTX14_Msk

#define PORT_RXTX_RXTX15_Pos 15
#define PORT_RXTX_RXTX15_Msk (0x1UL << PORT_RXTX_RXTX15_Pos)
#define PORT_RXTX_RXTX15     PORT_RXTX_RXTX15_Msk

/** @} */ /* End of group MDR32VF0xI_PORT_RXTX */

/** @defgroup MDR32VF0xI_PORT_OE PORT OE
 * @{
 */

#define PORT_OE_OE_Pos   0
#define PORT_OE_OE_Msk   (0xFFFFUL << PORT_OE_OE_Pos)

#define PORT_OE_OE0_Pos  0
#define PORT_OE_OE0_Msk  (0x1UL << PORT_OE_OE0_Pos)
#define PORT_OE_OE0      PORT_OE_OE0_Msk

#define PORT_OE_OE1_Pos  1
#define PORT_OE_OE1_Msk  (0x1UL << PORT_OE_OE1_Pos)
#define PORT_OE_OE1      PORT_OE_OE1_Msk

#define PORT_OE_OE2_Pos  2
#define PORT_OE_OE2_Msk  (0x1UL << PORT_OE_OE2_Pos)
#define PORT_OE_OE2      PORT_OE_OE2_Msk

#define PORT_OE_OE3_Pos  3
#define PORT_OE_OE3_Msk  (0x1UL << PORT_OE_OE3_Pos)
#define PORT_OE_OE3      PORT_OE_OE3_Msk

#define PORT_OE_OE4_Pos  4
#define PORT_OE_OE4_Msk  (0x1UL << PORT_OE_OE4_Pos)
#define PORT_OE_OE4      PORT_OE_OE4_Msk

#define PORT_OE_OE5_Pos  5
#define PORT_OE_OE5_Msk  (0x1UL << PORT_OE_OE5_Pos)
#define PORT_OE_OE5      PORT_OE_OE5_Msk

#define PORT_OE_OE6_Pos  6
#define PORT_OE_OE6_Msk  (0x1UL << PORT_OE_OE6_Pos)
#define PORT_OE_OE6      PORT_OE_OE6_Msk

#define PORT_OE_OE7_Pos  7
#define PORT_OE_OE7_Msk  (0x1UL << PORT_OE_OE7_Pos)
#define PORT_OE_OE7      PORT_OE_OE7_Msk

#define PORT_OE_OE8_Pos  8
#define PORT_OE_OE8_Msk  (0x1UL << PORT_OE_OE8_Pos)
#define PORT_OE_OE8      PORT_OE_OE8_Msk

#define PORT_OE_OE9_Pos  9
#define PORT_OE_OE9_Msk  (0x1UL << PORT_OE_OE9_Pos)
#define PORT_OE_OE9      PORT_OE_OE9_Msk

#define PORT_OE_OE10_Pos 10
#define PORT_OE_OE10_Msk (0x1UL << PORT_OE_OE10_Pos)
#define PORT_OE_OE10     PORT_OE_OE10_Msk

#define PORT_OE_OE11_Pos 11
#define PORT_OE_OE11_Msk (0x1UL << PORT_OE_OE11_Pos)
#define PORT_OE_OE11     PORT_OE_OE11_Msk

#define PORT_OE_OE12_Pos 12
#define PORT_OE_OE12_Msk (0x1UL << PORT_OE_OE12_Pos)
#define PORT_OE_OE12     PORT_OE_OE12_Msk

#define PORT_OE_OE13_Pos 13
#define PORT_OE_OE13_Msk (0x1UL << PORT_OE_OE13_Pos)
#define PORT_OE_OE13     PORT_OE_OE13_Msk

#define PORT_OE_OE14_Pos 14
#define PORT_OE_OE14_Msk (0x1UL << PORT_OE_OE14_Pos)
#define PORT_OE_OE14     PORT_OE_OE14_Msk

#define PORT_OE_OE15_Pos 15
#define PORT_OE_OE15_Msk (0x1UL << PORT_OE_OE15_Pos)
#define PORT_OE_OE15     PORT_OE_OE15_Msk

/** @} */ /* End of group MDR32VF0xI_PORT_OE */

/** @defgroup MDR32VF0xI_PORT_FUNC PORT FUNC
 * @{
 */

#define PORT_FUNC_MODE_Pos           0
#define PORT_FUNC_MODE_Msk           (0xFFFFFFFFUL)

#define PORT_FUNC_MODE0_Pos          0
#define PORT_FUNC_MODE0_Msk          (0x3UL << PORT_FUNC_MODE0_Pos)
#define PORT_FUNC_MODE0_PORT         (0x0UL << PORT_FUNC_MODE0_Pos)
#define PORT_FUNC_MODE0_MAIN         (0x1UL << PORT_FUNC_MODE0_Pos)
#define PORT_FUNC_MODE0_ALTERNATIVE  (0x2UL << PORT_FUNC_MODE0_Pos)
#define PORT_FUNC_MODE0_OVERRIDDEN   (0x3UL << PORT_FUNC_MODE0_Pos)

#define PORT_FUNC_MODE1_Pos          2
#define PORT_FUNC_MODE1_Msk          (0x3UL << PORT_FUNC_MODE1_Pos)
#define PORT_FUNC_MODE1_PORT         (0x0UL << PORT_FUNC_MODE1_Pos)
#define PORT_FUNC_MODE1_MAIN         (0x1UL << PORT_FUNC_MODE1_Pos)
#define PORT_FUNC_MODE1_ALTERNATIVE  (0x2UL << PORT_FUNC_MODE1_Pos)
#define PORT_FUNC_MODE1_OVERRIDDEN   (0x3UL << PORT_FUNC_MODE1_Pos)

#define PORT_FUNC_MODE2_Pos          4
#define PORT_FUNC_MODE2_Msk          (0x3UL << PORT_FUNC_MODE2_Pos)
#define PORT_FUNC_MODE2_PORT         (0x0UL << PORT_FUNC_MODE2_Pos)
#define PORT_FUNC_MODE2_MAIN         (0x1UL << PORT_FUNC_MODE2_Pos)
#define PORT_FUNC_MODE2_ALTERNATIVE  (0x2UL << PORT_FUNC_MODE2_Pos)
#define PORT_FUNC_MODE2_OVERRIDDEN   (0x3UL << PORT_FUNC_MODE2_Pos)

#define PORT_FUNC_MODE3_Pos          6
#define PORT_FUNC_MODE3_Msk          (0x3UL << PORT_FUNC_MODE3_Pos)
#define PORT_FUNC_MODE3_PORT         (0x0UL << PORT_FUNC_MODE3_Pos)
#define PORT_FUNC_MODE3_MAIN         (0x1UL << PORT_FUNC_MODE3_Pos)
#define PORT_FUNC_MODE3_ALTERNATIVE  (0x2UL << PORT_FUNC_MODE3_Pos)
#define PORT_FUNC_MODE3_OVERRIDDEN   (0x3UL << PORT_FUNC_MODE3_Pos)

#define PORT_FUNC_MODE4_Pos          8
#define PORT_FUNC_MODE4_Msk          (0x3UL << PORT_FUNC_MODE4_Pos)
#define PORT_FUNC_MODE4_PORT         (0x0UL << PORT_FUNC_MODE4_Pos)
#define PORT_FUNC_MODE4_MAIN         (0x1UL << PORT_FUNC_MODE4_Pos)
#define PORT_FUNC_MODE4_ALTERNATIVE  (0x2UL << PORT_FUNC_MODE4_Pos)
#define PORT_FUNC_MODE4_OVERRIDDEN   (0x3UL << PORT_FUNC_MODE4_Pos)

#define PORT_FUNC_MODE5_Pos          10
#define PORT_FUNC_MODE5_Msk          (0x3UL << PORT_FUNC_MODE5_Pos)
#define PORT_FUNC_MODE5_PORT         (0x0UL << PORT_FUNC_MODE5_Pos)
#define PORT_FUNC_MODE5_MAIN         (0x1UL << PORT_FUNC_MODE5_Pos)
#define PORT_FUNC_MODE5_ALTERNATIVE  (0x2UL << PORT_FUNC_MODE5_Pos)
#define PORT_FUNC_MODE5_OVERRIDDEN   (0x3UL << PORT_FUNC_MODE5_Pos)

#define PORT_FUNC_MODE6_Pos          12
#define PORT_FUNC_MODE6_Msk          (0x3UL << PORT_FUNC_MODE6_Pos)
#define PORT_FUNC_MODE6_PORT         (0x0UL << PORT_FUNC_MODE6_Pos)
#define PORT_FUNC_MODE6_MAIN         (0x1UL << PORT_FUNC_MODE6_Pos)
#define PORT_FUNC_MODE6_ALTERNATIVE  (0x2UL << PORT_FUNC_MODE6_Pos)
#define PORT_FUNC_MODE6_OVERRIDDEN   (0x3UL << PORT_FUNC_MODE6_Pos)

#define PORT_FUNC_MODE7_Pos          14
#define PORT_FUNC_MODE7_Msk          (0x3UL << PORT_FUNC_MODE7_Pos)
#define PORT_FUNC_MODE7_PORT         (0x0UL << PORT_FUNC_MODE7_Pos)
#define PORT_FUNC_MODE7_MAIN         (0x1UL << PORT_FUNC_MODE7_Pos)
#define PORT_FUNC_MODE7_ALTERNATIVE  (0x2UL << PORT_FUNC_MODE7_Pos)
#define PORT_FUNC_MODE7_OVERRIDDEN   (0x3UL << PORT_FUNC_MODE7_Pos)

#define PORT_FUNC_MODE8_Pos          16
#define PORT_FUNC_MODE8_Msk          (0x3UL << PORT_FUNC_MODE8_Pos)
#define PORT_FUNC_MODE8_PORT         (0x0UL << PORT_FUNC_MODE8_Pos)
#define PORT_FUNC_MODE8_MAIN         (0x1UL << PORT_FUNC_MODE8_Pos)
#define PORT_FUNC_MODE8_ALTERNATIVE  (0x2UL << PORT_FUNC_MODE8_Pos)
#define PORT_FUNC_MODE8_OVERRIDDEN   (0x3UL << PORT_FUNC_MODE8_Pos)

#define PORT_FUNC_MODE9_Pos          18
#define PORT_FUNC_MODE9_Msk          (0x3UL << PORT_FUNC_MODE9_Pos)
#define PORT_FUNC_MODE9_PORT         (0x0UL << PORT_FUNC_MODE9_Pos)
#define PORT_FUNC_MODE9_MAIN         (0x1UL << PORT_FUNC_MODE9_Pos)
#define PORT_FUNC_MODE9_ALTERNATIVE  (0x2UL << PORT_FUNC_MODE9_Pos)
#define PORT_FUNC_MODE9_OVERRIDDEN   (0x3UL << PORT_FUNC_MODE9_Pos)

#define PORT_FUNC_MODE10_Pos         20
#define PORT_FUNC_MODE10_Msk         (0x3UL << PORT_FUNC_MODE10_Pos)
#define PORT_FUNC_MODE10_PORT        (0x0UL << PORT_FUNC_MODE10_Pos)
#define PORT_FUNC_MODE10_MAIN        (0x1UL << PORT_FUNC_MODE10_Pos)
#define PORT_FUNC_MODE10_ALTERNATIVE (0x2UL << PORT_FUNC_MODE10_Pos)
#define PORT_FUNC_MODE10_OVERRIDDEN  (0x3UL << PORT_FUNC_MODE10_Pos)

#define PORT_FUNC_MODE11_Pos         22
#define PORT_FUNC_MODE11_Msk         (0x3UL << PORT_FUNC_MODE11_Pos)
#define PORT_FUNC_MODE11_PORT        (0x0UL << PORT_FUNC_MODE11_Pos)
#define PORT_FUNC_MODE11_MAIN        (0x1UL << PORT_FUNC_MODE11_Pos)
#define PORT_FUNC_MODE11_ALTERNATIVE (0x2UL << PORT_FUNC_MODE11_Pos)
#define PORT_FUNC_MODE11_OVERRIDDEN  (0x3UL << PORT_FUNC_MODE11_Pos)

#define PORT_FUNC_MODE12_Pos         24
#define PORT_FUNC_MODE12_Msk         (0x3UL << PORT_FUNC_MODE12_Pos)
#define PORT_FUNC_MODE12_PORT        (0x0UL << PORT_FUNC_MODE12_Pos)
#define PORT_FUNC_MODE12_MAIN        (0x1UL << PORT_FUNC_MODE12_Pos)
#define PORT_FUNC_MODE12_ALTERNATIVE (0x2UL << PORT_FUNC_MODE12_Pos)
#define PORT_FUNC_MODE12_OVERRIDDEN  (0x3UL << PORT_FUNC_MODE12_Pos)

#define PORT_FUNC_MODE13_Pos         26
#define PORT_FUNC_MODE13_Msk         (0x3UL << PORT_FUNC_MODE13_Pos)
#define PORT_FUNC_MODE13_PORT        (0x0UL << PORT_FUNC_MODE13_Pos)
#define PORT_FUNC_MODE13_MAIN        (0x1UL << PORT_FUNC_MODE13_Pos)
#define PORT_FUNC_MODE13_ALTERNATIVE (0x2UL << PORT_FUNC_MODE13_Pos)
#define PORT_FUNC_MODE13_OVERRIDDEN  (0x3UL << PORT_FUNC_MODE13_Pos)

#define PORT_FUNC_MODE14_Pos         28
#define PORT_FUNC_MODE14_Msk         (0x3UL << PORT_FUNC_MODE14_Pos)
#define PORT_FUNC_MODE14_PORT        (0x0UL << PORT_FUNC_MODE14_Pos)
#define PORT_FUNC_MODE14_MAIN        (0x1UL << PORT_FUNC_MODE14_Pos)
#define PORT_FUNC_MODE14_ALTERNATIVE (0x2UL << PORT_FUNC_MODE14_Pos)
#define PORT_FUNC_MODE14_OVERRIDDEN  (0x3UL << PORT_FUNC_MODE14_Pos)

#define PORT_FUNC_MODE15_Pos         30
#define PORT_FUNC_MODE15_Msk         (0x3UL << PORT_FUNC_MODE15_Pos)
#define PORT_FUNC_MODE15_PORT        (0x0UL << PORT_FUNC_MODE15_Pos)
#define PORT_FUNC_MODE15_MAIN        (0x1UL << PORT_FUNC_MODE15_Pos)
#define PORT_FUNC_MODE15_ALTERNATIVE (0x2UL << PORT_FUNC_MODE15_Pos)
#define PORT_FUNC_MODE15_OVERRIDDEN  (0x3UL << PORT_FUNC_MODE15_Pos)

/** @} */ /* End of group MDR32VF0xI_PORT_FUNC */

/** @defgroup MDR32VF0xI_PORT_ANALOG PORT ANALOG
 * @{
 */

#define PORT_ANALOG_ANALOG_EN_Pos   0
#define PORT_ANALOG_ANALOG_EN_Msk   (0xFFFFUL << PORT_ANALOG_ANALOG_EN_Pos)

#define PORT_ANALOG_ANALOG_EN0_Pos  0
#define PORT_ANALOG_ANALOG_EN0_Msk  (0x1UL << PORT_ANALOG_ANALOG_EN0_Pos)
#define PORT_ANALOG_ANALOG_EN0      PORT_ANALOG_ANALOG_EN0_Msk

#define PORT_ANALOG_ANALOG_EN1_Pos  1
#define PORT_ANALOG_ANALOG_EN1_Msk  (0x1UL << PORT_ANALOG_ANALOG_EN1_Pos)
#define PORT_ANALOG_ANALOG_EN1      PORT_ANALOG_ANALOG_EN1_Msk

#define PORT_ANALOG_ANALOG_EN2_Pos  2
#define PORT_ANALOG_ANALOG_EN2_Msk  (0x1UL << PORT_ANALOG_ANALOG_EN2_Pos)
#define PORT_ANALOG_ANALOG_EN2      PORT_ANALOG_ANALOG_EN2_Msk

#define PORT_ANALOG_ANALOG_EN3_Pos  3
#define PORT_ANALOG_ANALOG_EN3_Msk  (0x1UL << PORT_ANALOG_ANALOG_EN3_Pos)
#define PORT_ANALOG_ANALOG_EN3      PORT_ANALOG_ANALOG_EN3_Msk

#define PORT_ANALOG_ANALOG_EN4_Pos  4
#define PORT_ANALOG_ANALOG_EN4_Msk  (0x1UL << PORT_ANALOG_ANALOG_EN4_Pos)
#define PORT_ANALOG_ANALOG_EN4      PORT_ANALOG_ANALOG_EN4_Msk

#define PORT_ANALOG_ANALOG_EN5_Pos  5
#define PORT_ANALOG_ANALOG_EN5_Msk  (0x1UL << PORT_ANALOG_ANALOG_EN5_Pos)
#define PORT_ANALOG_ANALOG_EN5      PORT_ANALOG_ANALOG_EN5_Msk

#define PORT_ANALOG_ANALOG_EN6_Pos  6
#define PORT_ANALOG_ANALOG_EN6_Msk  (0x1UL << PORT_ANALOG_ANALOG_EN6_Pos)
#define PORT_ANALOG_ANALOG_EN6      PORT_ANALOG_ANALOG_EN6_Msk

#define PORT_ANALOG_ANALOG_EN7_Pos  7
#define PORT_ANALOG_ANALOG_EN7_Msk  (0x1UL << PORT_ANALOG_ANALOG_EN7_Pos)
#define PORT_ANALOG_ANALOG_EN7      PORT_ANALOG_ANALOG_EN7_Msk

#define PORT_ANALOG_ANALOG_EN8_Pos  8
#define PORT_ANALOG_ANALOG_EN8_Msk  (0x1UL << PORT_ANALOG_ANALOG_EN8_Pos)
#define PORT_ANALOG_ANALOG_EN8      PORT_ANALOG_ANALOG_EN8_Msk

#define PORT_ANALOG_ANALOG_EN9_Pos  9
#define PORT_ANALOG_ANALOG_EN9_Msk  (0x1UL << PORT_ANALOG_ANALOG_EN9_Pos)
#define PORT_ANALOG_ANALOG_EN9      PORT_ANALOG_ANALOG_EN9_Msk

#define PORT_ANALOG_ANALOG_EN10_Pos 10
#define PORT_ANALOG_ANALOG_EN10_Msk (0x1UL << PORT_ANALOG_ANALOG_EN10_Pos)
#define PORT_ANALOG_ANALOG_EN10     PORT_ANALOG_ANALOG_EN10_Msk

#define PORT_ANALOG_ANALOG_EN11_Pos 11
#define PORT_ANALOG_ANALOG_EN11_Msk (0x1UL << PORT_ANALOG_ANALOG_EN11_Pos)
#define PORT_ANALOG_ANALOG_EN11     PORT_ANALOG_ANALOG_EN11_Msk

#define PORT_ANALOG_ANALOG_EN12_Pos 12
#define PORT_ANALOG_ANALOG_EN12_Msk (0x1UL << PORT_ANALOG_ANALOG_EN12_Pos)
#define PORT_ANALOG_ANALOG_EN12     PORT_ANALOG_ANALOG_EN12_Msk

#define PORT_ANALOG_ANALOG_EN13_Pos 13
#define PORT_ANALOG_ANALOG_EN13_Msk (0x1UL << PORT_ANALOG_ANALOG_EN13_Pos)
#define PORT_ANALOG_ANALOG_EN13     PORT_ANALOG_ANALOG_EN13_Msk

#define PORT_ANALOG_ANALOG_EN14_Pos 14
#define PORT_ANALOG_ANALOG_EN14_Msk (0x1UL << PORT_ANALOG_ANALOG_EN14_Pos)
#define PORT_ANALOG_ANALOG_EN14     PORT_ANALOG_ANALOG_EN14_Msk

#define PORT_ANALOG_ANALOG_EN15_Pos 15
#define PORT_ANALOG_ANALOG_EN15_Msk (0x1UL << PORT_ANALOG_ANALOG_EN15_Pos)
#define PORT_ANALOG_ANALOG_EN15     PORT_ANALOG_ANALOG_EN15_Msk

/** @} */ /* End of group MDR32VF0xI_PORT_ANALOG */

/** @defgroup MDR32VF0xI_PORT_PULL PORT PULL
 * @{
 */

#define PORT_PULL_PULL_DOWN_Pos   0
#define PORT_PULL_PULL_DOWN_Msk   (0xFFFFUL << PORT_PULL_PULL_DOWN_Pos)

#define PORT_PULL_PULL_DOWN0_Pos  0
#define PORT_PULL_PULL_DOWN0_Msk  (0x1UL << PORT_PULL_PULL_DOWN0_Pos)
#define PORT_PULL_PULL_DOWN0      PORT_PULL_PULL_DOWN0_Msk

#define PORT_PULL_PULL_DOWN1_Pos  1
#define PORT_PULL_PULL_DOWN1_Msk  (0x1UL << PORT_PULL_PULL_DOWN1_Pos)
#define PORT_PULL_PULL_DOWN1      PORT_PULL_PULL_DOWN1_Msk

#define PORT_PULL_PULL_DOWN2_Pos  2
#define PORT_PULL_PULL_DOWN2_Msk  (0x1UL << PORT_PULL_PULL_DOWN2_Pos)
#define PORT_PULL_PULL_DOWN2      PORT_PULL_PULL_DOWN2_Msk

#define PORT_PULL_PULL_DOWN3_Pos  3
#define PORT_PULL_PULL_DOWN3_Msk  (0x1UL << PORT_PULL_PULL_DOWN3_Pos)
#define PORT_PULL_PULL_DOWN3      PORT_PULL_PULL_DOWN3_Msk

#define PORT_PULL_PULL_DOWN4_Pos  4
#define PORT_PULL_PULL_DOWN4_Msk  (0x1UL << PORT_PULL_PULL_DOWN4_Pos)
#define PORT_PULL_PULL_DOWN4      PORT_PULL_PULL_DOWN4_Msk

#define PORT_PULL_PULL_DOWN5_Pos  5
#define PORT_PULL_PULL_DOWN5_Msk  (0x1UL << PORT_PULL_PULL_DOWN5_Pos)
#define PORT_PULL_PULL_DOWN5      PORT_PULL_PULL_DOWN5_Msk

#define PORT_PULL_PULL_DOWN6_Pos  6
#define PORT_PULL_PULL_DOWN6_Msk  (0x1UL << PORT_PULL_PULL_DOWN6_Pos)
#define PORT_PULL_PULL_DOWN6      PORT_PULL_PULL_DOWN6_Msk

#define PORT_PULL_PULL_DOWN7_Pos  7
#define PORT_PULL_PULL_DOWN7_Msk  (0x1UL << PORT_PULL_PULL_DOWN7_Pos)
#define PORT_PULL_PULL_DOWN7      PORT_PULL_PULL_DOWN7_Msk

#define PORT_PULL_PULL_DOWN8_Pos  8
#define PORT_PULL_PULL_DOWN8_Msk  (0x1UL << PORT_PULL_PULL_DOWN8_Pos)
#define PORT_PULL_PULL_DOWN8      PORT_PULL_PULL_DOWN8_Msk

#define PORT_PULL_PULL_DOWN9_Pos  9
#define PORT_PULL_PULL_DOWN9_Msk  (0x1UL << PORT_PULL_PULL_DOWN9_Pos)
#define PORT_PULL_PULL_DOWN9      PORT_PULL_PULL_DOWN9_Msk

#define PORT_PULL_PULL_DOWN10_Pos 10
#define PORT_PULL_PULL_DOWN10_Msk (0x1UL << PORT_PULL_PULL_DOWN10_Pos)
#define PORT_PULL_PULL_DOWN10     PORT_PULL_PULL_DOWN10_Msk

#define PORT_PULL_PULL_DOWN11_Pos 11
#define PORT_PULL_PULL_DOWN11_Msk (0x1UL << PORT_PULL_PULL_DOWN11_Pos)
#define PORT_PULL_PULL_DOWN11     PORT_PULL_PULL_DOWN11_Msk

#define PORT_PULL_PULL_DOWN12_Pos 12
#define PORT_PULL_PULL_DOWN12_Msk (0x1UL << PORT_PULL_PULL_DOWN12_Pos)
#define PORT_PULL_PULL_DOWN12     PORT_PULL_PULL_DOWN12_Msk

#define PORT_PULL_PULL_DOWN13_Pos 13
#define PORT_PULL_PULL_DOWN13_Msk (0x1UL << PORT_PULL_PULL_DOWN13_Pos)
#define PORT_PULL_PULL_DOWN13     PORT_PULL_PULL_DOWN13_Msk

#define PORT_PULL_PULL_DOWN14_Pos 14
#define PORT_PULL_PULL_DOWN14_Msk (0x1UL << PORT_PULL_PULL_DOWN14_Pos)
#define PORT_PULL_PULL_DOWN14     PORT_PULL_PULL_DOWN14_Msk

#define PORT_PULL_PULL_DOWN15_Pos 15
#define PORT_PULL_PULL_DOWN15_Msk (0x1UL << PORT_PULL_PULL_DOWN15_Pos)
#define PORT_PULL_PULL_DOWN15     PORT_PULL_PULL_DOWN15_Msk

#if defined(USE_MDR32F02_REV_2) || defined(USE_MDR1206FI) || defined(USE_MDR1206AFI) || defined(USE_MDR1206)

#define PORT_PULL_PULL_UP_Pos   16
#define PORT_PULL_PULL_UP_Msk   (0xFFFFUL << PORT_PULL_PULL_UP_Pos)

#define PORT_PULL_PULL_UP0_Pos  16
#define PORT_PULL_PULL_UP0_Msk  (0x1UL << PORT_PULL_PULL_UP0_Pos)
#define PORT_PULL_PULL_UP0      PORT_PULL_PULL_UP0_Msk

#define PORT_PULL_PULL_UP1_Pos  17
#define PORT_PULL_PULL_UP1_Msk  (0x1UL << PORT_PULL_PULL_UP1_Pos)
#define PORT_PULL_PULL_UP1      PORT_PULL_PULL_UP1_Msk

#define PORT_PULL_PULL_UP2_Pos  18
#define PORT_PULL_PULL_UP2_Msk  (0x1UL << PORT_PULL_PULL_UP2_Pos)
#define PORT_PULL_PULL_UP2      PORT_PULL_PULL_UP2_Msk

#define PORT_PULL_PULL_UP3_Pos  19
#define PORT_PULL_PULL_UP3_Msk  (0x1UL << PORT_PULL_PULL_UP3_Pos)
#define PORT_PULL_PULL_UP3      PORT_PULL_PULL_UP3_Msk

#define PORT_PULL_PULL_UP4_Pos  20
#define PORT_PULL_PULL_UP4_Msk  (0x1UL << PORT_PULL_PULL_UP4_Pos)
#define PORT_PULL_PULL_UP4      PORT_PULL_PULL_UP4_Msk

#define PORT_PULL_PULL_UP5_Pos  21
#define PORT_PULL_PULL_UP5_Msk  (0x1UL << PORT_PULL_PULL_UP5_Pos)
#define PORT_PULL_PULL_UP5      PORT_PULL_PULL_UP5_Msk

#define PORT_PULL_PULL_UP6_Pos  22
#define PORT_PULL_PULL_UP6_Msk  (0x1UL << PORT_PULL_PULL_UP6_Pos)
#define PORT_PULL_PULL_UP6      PORT_PULL_PULL_UP6_Msk

#define PORT_PULL_PULL_UP7_Pos  23
#define PORT_PULL_PULL_UP7_Msk  (0x1UL << PORT_PULL_PULL_UP7_Pos)
#define PORT_PULL_PULL_UP7      PORT_PULL_PULL_UP7_Msk

#define PORT_PULL_PULL_UP8_Pos  24
#define PORT_PULL_PULL_UP8_Msk  (0x1UL << PORT_PULL_PULL_UP8_Pos)
#define PORT_PULL_PULL_UP8      PORT_PULL_PULL_UP8_Msk

#define PORT_PULL_PULL_UP9_Pos  25
#define PORT_PULL_PULL_UP9_Msk  (0x1UL << PORT_PULL_PULL_UP9_Pos)
#define PORT_PULL_PULL_UP9      PORT_PULL_PULL_UP9_Msk

#define PORT_PULL_PULL_UP10_Pos 26
#define PORT_PULL_PULL_UP10_Msk (0x1UL << PORT_PULL_PULL_UP10_Pos)
#define PORT_PULL_PULL_UP10     PORT_PULL_PULL_UP10_Msk

#define PORT_PULL_PULL_UP11_Pos 27
#define PORT_PULL_PULL_UP11_Msk (0x1UL << PORT_PULL_PULL_UP11_Pos)
#define PORT_PULL_PULL_UP11     PORT_PULL_PULL_UP11_Msk

#define PORT_PULL_PULL_UP12_Pos 28
#define PORT_PULL_PULL_UP12_Msk (0x1UL << PORT_PULL_PULL_UP12_Pos)
#define PORT_PULL_PULL_UP12     PORT_PULL_PULL_UP12_Msk

#define PORT_PULL_PULL_UP13_Pos 29
#define PORT_PULL_PULL_UP13_Msk (0x1UL << PORT_PULL_PULL_UP13_Pos)
#define PORT_PULL_PULL_UP13     PORT_PULL_PULL_UP13_Msk

#define PORT_PULL_PULL_UP14_Pos 30
#define PORT_PULL_PULL_UP14_Msk (0x1UL << PORT_PULL_PULL_UP14_Pos)
#define PORT_PULL_PULL_UP14     PORT_PULL_PULL_UP14_Msk

#define PORT_PULL_PULL_UP15_Pos 31
#define PORT_PULL_PULL_UP15_Msk (0x1UL << PORT_PULL_PULL_UP15_Pos)
#define PORT_PULL_PULL_UP15     PORT_PULL_PULL_UP15_Msk

#endif

/** @} */ /* End of group MDR32VF0xI_PORT_PULL */

/** @defgroup MDR32VF0xI_PORT_PWR PORT PWR
 * @{
 */

#define PORT_PWR_PWR_Pos   0
#define PORT_PWR_PWR_Msk   (0xFFFFUL << PORT_PWR_PWR_Pos)

#define PORT_PWR_PWR0_Pos  0
#define PORT_PWR_PWR0_Msk  (0x1UL << PORT_PWR_PWR0_Pos)
#define PORT_PWR_PWR0      PORT_PWR_PWR0_Msk

#define PORT_PWR_PWR1_Pos  1
#define PORT_PWR_PWR1_Msk  (0x1UL << PORT_PWR_PWR1_Pos)
#define PORT_PWR_PWR1      PORT_PWR_PWR1_Msk

#define PORT_PWR_PWR2_Pos  2
#define PORT_PWR_PWR2_Msk  (0x1UL << PORT_PWR_PWR2_Pos)
#define PORT_PWR_PWR2      PORT_PWR_PWR2_Msk

#define PORT_PWR_PWR3_Pos  3
#define PORT_PWR_PWR3_Msk  (0x1UL << PORT_PWR_PWR3_Pos)
#define PORT_PWR_PWR3      PORT_PWR_PWR3_Msk

#define PORT_PWR_PWR4_Pos  4
#define PORT_PWR_PWR4_Msk  (0x1UL << PORT_PWR_PWR4_Pos)
#define PORT_PWR_PWR4      PORT_PWR_PWR4_Msk

#define PORT_PWR_PWR5_Pos  5
#define PORT_PWR_PWR5_Msk  (0x1UL << PORT_PWR_PWR5_Pos)
#define PORT_PWR_PWR5      PORT_PWR_PWR5_Msk

#define PORT_PWR_PWR6_Pos  6
#define PORT_PWR_PWR6_Msk  (0x1UL << PORT_PWR_PWR6_Pos)
#define PORT_PWR_PWR6      PORT_PWR_PWR6_Msk

#define PORT_PWR_PWR7_Pos  7
#define PORT_PWR_PWR7_Msk  (0x1UL << PORT_PWR_PWR7_Pos)
#define PORT_PWR_PWR7      PORT_PWR_PWR7_Msk

#define PORT_PWR_PWR8_Pos  8
#define PORT_PWR_PWR8_Msk  (0x1UL << PORT_PWR_PWR8_Pos)
#define PORT_PWR_PWR8      PORT_PWR_PWR8_Msk

#define PORT_PWR_PWR9_Pos  9
#define PORT_PWR_PWR9_Msk  (0x1UL << PORT_PWR_PWR9_Pos)
#define PORT_PWR_PWR9      PORT_PWR_PWR9_Msk

#define PORT_PWR_PWR10_Pos 10
#define PORT_PWR_PWR10_Msk (0x1UL << PORT_PWR_PWR10_Pos)
#define PORT_PWR_PWR10     PORT_PWR_PWR10_Msk

#define PORT_PWR_PWR11_Pos 11
#define PORT_PWR_PWR11_Msk (0x1UL << PORT_PWR_PWR11_Pos)
#define PORT_PWR_PWR11     PORT_PWR_PWR11_Msk

#define PORT_PWR_PWR12_Pos 12
#define PORT_PWR_PWR12_Msk (0x1UL << PORT_PWR_PWR12_Pos)
#define PORT_PWR_PWR12     PORT_PWR_PWR12_Msk

#define PORT_PWR_PWR13_Pos 13
#define PORT_PWR_PWR13_Msk (0x1UL << PORT_PWR_PWR13_Pos)
#define PORT_PWR_PWR13     PORT_PWR_PWR13_Msk

#define PORT_PWR_PWR14_Pos 14
#define PORT_PWR_PWR14_Msk (0x1UL << PORT_PWR_PWR14_Pos)
#define PORT_PWR_PWR14     PORT_PWR_PWR14_Msk

#define PORT_PWR_PWR15_Pos 15
#define PORT_PWR_PWR15_Msk (0x1UL << PORT_PWR_PWR15_Pos)
#define PORT_PWR_PWR15     PORT_PWR_PWR15_Msk

/** @} */ /* End of group MDR32VF0xI_PORT_PWR */

/** @defgroup MDR32VF0xI_PORT_SETTX PORT SETTX
 * @{
 */

#define PORT_SETTX_SETTX_Pos   0
#define PORT_SETTX_SETTX_Msk   (0xFFFFUL << PORT_SETTX_SETTX_Pos)

#define PORT_SETTX_SETTX0_Pos  0
#define PORT_SETTX_SETTX0_Msk  (0x1UL << PORT_SETTX_SETTX0_Pos)
#define PORT_SETTX_SETTX0      PORT_SETTX_SETTX0_Msk

#define PORT_SETTX_SETTX1_Pos  1
#define PORT_SETTX_SETTX1_Msk  (0x1UL << PORT_SETTX_SETTX1_Pos)
#define PORT_SETTX_SETTX1      PORT_SETTX_SETTX1_Msk

#define PORT_SETTX_SETTX2_Pos  2
#define PORT_SETTX_SETTX2_Msk  (0x1UL << PORT_SETTX_SETTX2_Pos)
#define PORT_SETTX_SETTX2      PORT_SETTX_SETTX2_Msk

#define PORT_SETTX_SETTX3_Pos  3
#define PORT_SETTX_SETTX3_Msk  (0x1UL << PORT_SETTX_SETTX3_Pos)
#define PORT_SETTX_SETTX3      PORT_SETTX_SETTX3_Msk

#define PORT_SETTX_SETTX4_Pos  4
#define PORT_SETTX_SETTX4_Msk  (0x1UL << PORT_SETTX_SETTX4_Pos)
#define PORT_SETTX_SETTX4      PORT_SETTX_SETTX4_Msk

#define PORT_SETTX_SETTX5_Pos  5
#define PORT_SETTX_SETTX5_Msk  (0x1UL << PORT_SETTX_SETTX5_Pos)
#define PORT_SETTX_SETTX5      PORT_SETTX_SETTX5_Msk

#define PORT_SETTX_SETTX6_Pos  6
#define PORT_SETTX_SETTX6_Msk  (0x1UL << PORT_SETTX_SETTX6_Pos)
#define PORT_SETTX_SETTX6      PORT_SETTX_SETTX6_Msk

#define PORT_SETTX_SETTX7_Pos  7
#define PORT_SETTX_SETTX7_Msk  (0x1UL << PORT_SETTX_SETTX7_Pos)
#define PORT_SETTX_SETTX7      PORT_SETTX_SETTX7_Msk

#define PORT_SETTX_SETTX8_Pos  8
#define PORT_SETTX_SETTX8_Msk  (0x1UL << PORT_SETTX_SETTX8_Pos)
#define PORT_SETTX_SETTX8      PORT_SETTX_SETTX8_Msk

#define PORT_SETTX_SETTX9_Pos  9
#define PORT_SETTX_SETTX9_Msk  (0x1UL << PORT_SETTX_SETTX9_Pos)
#define PORT_SETTX_SETTX9      PORT_SETTX_SETTX9_Msk

#define PORT_SETTX_SETTX10_Pos 10
#define PORT_SETTX_SETTX10_Msk (0x1UL << PORT_SETTX_SETTX10_Pos)
#define PORT_SETTX_SETTX10     PORT_SETTX_SETTX10_Msk

#define PORT_SETTX_SETTX11_Pos 11
#define PORT_SETTX_SETTX11_Msk (0x1UL << PORT_SETTX_SETTX11_Pos)
#define PORT_SETTX_SETTX11     PORT_SETTX_SETTX11_Msk

#define PORT_SETTX_SETTX12_Pos 12
#define PORT_SETTX_SETTX12_Msk (0x1UL << PORT_SETTX_SETTX12_Pos)
#define PORT_SETTX_SETTX12     PORT_SETTX_SETTX12_Msk

#define PORT_SETTX_SETTX13_Pos 13
#define PORT_SETTX_SETTX13_Msk (0x1UL << PORT_SETTX_SETTX13_Pos)
#define PORT_SETTX_SETTX13     PORT_SETTX_SETTX13_Msk

#define PORT_SETTX_SETTX14_Pos 14
#define PORT_SETTX_SETTX14_Msk (0x1UL << PORT_SETTX_SETTX14_Pos)
#define PORT_SETTX_SETTX14     PORT_SETTX_SETTX14_Msk

#define PORT_SETTX_SETTX15_Pos 15
#define PORT_SETTX_SETTX15_Msk (0x1UL << PORT_SETTX_SETTX15_Pos)
#define PORT_SETTX_SETTX15     PORT_SETTX_SETTX15_Msk

/** @} */ /* End of group MDR32VF0xI_PORT_SETTX */

/** @defgroup MDR32VF0xI_PORT_CLRTX PORT CLRTX
 * @{
 */

#define PORT_CLRTX_CLRTX_Pos   0
#define PORT_CLRTX_CLRTX_Msk   (0xFFFFUL << PORT_CLRTX_CLRTX_Pos)

#define PORT_CLRTX_CLRTX0_Pos  0
#define PORT_CLRTX_CLRTX0_Msk  (0x1UL << PORT_CLRTX_CLRTX0_Pos)
#define PORT_CLRTX_CLRTX0      PORT_CLRTX_CLRTX0_Msk

#define PORT_CLRTX_CLRTX1_Pos  1
#define PORT_CLRTX_CLRTX1_Msk  (0x1UL << PORT_CLRTX_CLRTX1_Pos)
#define PORT_CLRTX_CLRTX1      PORT_CLRTX_CLRTX1_Msk

#define PORT_CLRTX_CLRTX2_Pos  2
#define PORT_CLRTX_CLRTX2_Msk  (0x1UL << PORT_CLRTX_CLRTX2_Pos)
#define PORT_CLRTX_CLRTX2      PORT_CLRTX_CLRTX2_Msk

#define PORT_CLRTX_CLRTX3_Pos  3
#define PORT_CLRTX_CLRTX3_Msk  (0x1UL << PORT_CLRTX_CLRTX3_Pos)
#define PORT_CLRTX_CLRTX3      PORT_CLRTX_CLRTX3_Msk

#define PORT_CLRTX_CLRTX4_Pos  4
#define PORT_CLRTX_CLRTX4_Msk  (0x1UL << PORT_CLRTX_CLRTX4_Pos)
#define PORT_CLRTX_CLRTX4      PORT_CLRTX_CLRTX4_Msk

#define PORT_CLRTX_CLRTX5_Pos  5
#define PORT_CLRTX_CLRTX5_Msk  (0x1UL << PORT_CLRTX_CLRTX5_Pos)
#define PORT_CLRTX_CLRTX5      PORT_CLRTX_CLRTX5_Msk

#define PORT_CLRTX_CLRTX6_Pos  6
#define PORT_CLRTX_CLRTX6_Msk  (0x1UL << PORT_CLRTX_CLRTX6_Pos)
#define PORT_CLRTX_CLRTX6      PORT_CLRTX_CLRTX6_Msk

#define PORT_CLRTX_CLRTX7_Pos  7
#define PORT_CLRTX_CLRTX7_Msk  (0x1UL << PORT_CLRTX_CLRTX7_Pos)
#define PORT_CLRTX_CLRTX7      PORT_CLRTX_CLRTX7_Msk

#define PORT_CLRTX_CLRTX8_Pos  8
#define PORT_CLRTX_CLRTX8_Msk  (0x1UL << PORT_CLRTX_CLRTX8_Pos)
#define PORT_CLRTX_CLRTX8      PORT_CLRTX_CLRTX8_Msk

#define PORT_CLRTX_CLRTX9_Pos  9
#define PORT_CLRTX_CLRTX9_Msk  (0x1UL << PORT_CLRTX_CLRTX9_Pos)
#define PORT_CLRTX_CLRTX9      PORT_CLRTX_CLRTX9_Msk

#define PORT_CLRTX_CLRTX10_Pos 10
#define PORT_CLRTX_CLRTX10_Msk (0x1UL << PORT_CLRTX_CLRTX10_Pos)
#define PORT_CLRTX_CLRTX10     PORT_CLRTX_CLRTX10_Msk

#define PORT_CLRTX_CLRTX11_Pos 11
#define PORT_CLRTX_CLRTX11_Msk (0x1UL << PORT_CLRTX_CLRTX11_Pos)
#define PORT_CLRTX_CLRTX11     PORT_CLRTX_CLRTX11_Msk

#define PORT_CLRTX_CLRTX12_Pos 12
#define PORT_CLRTX_CLRTX12_Msk (0x1UL << PORT_CLRTX_CLRTX12_Pos)
#define PORT_CLRTX_CLRTX12     PORT_CLRTX_CLRTX12_Msk

#define PORT_CLRTX_CLRTX13_Pos 13
#define PORT_CLRTX_CLRTX13_Msk (0x1UL << PORT_CLRTX_CLRTX13_Pos)
#define PORT_CLRTX_CLRTX13     PORT_CLRTX_CLRTX13_Msk

#define PORT_CLRTX_CLRTX14_Pos 14
#define PORT_CLRTX_CLRTX14_Msk (0x1UL << PORT_CLRTX_CLRTX14_Pos)
#define PORT_CLRTX_CLRTX14     PORT_CLRTX_CLRTX14_Msk

#define PORT_CLRTX_CLRTX15_Pos 15
#define PORT_CLRTX_CLRTX15_Msk (0x1UL << PORT_CLRTX_CLRTX15_Pos)
#define PORT_CLRTX_CLRTX15     PORT_CLRTX_CLRTX15_Msk

/** @} */ /* End of group MDR32VF0xI_PORT_CLRTX */

/** @defgroup MDR32VF0xI_PORT_RDTX PORT RDTX
 * @{
 */

#define PORT_RDTX_RDTX_Pos   0
#define PORT_RDTX_RDTX_Msk   (0xFFFFUL << PORT_RDTX_RDTX_Pos)

#define PORT_RDTX_RDTX0_Pos  0
#define PORT_RDTX_RDTX0_Msk  (0x1UL << PORT_RDTX_RDTX0_Pos)
#define PORT_RDTX_RDTX0      PORT_RDTX_RDTX0_Msk

#define PORT_RDTX_RDTX1_Pos  1
#define PORT_RDTX_RDTX1_Msk  (0x1UL << PORT_RDTX_RDTX1_Pos)
#define PORT_RDTX_RDTX1      PORT_RDTX_RDTX1_Msk

#define PORT_RDTX_RDTX2_Pos  2
#define PORT_RDTX_RDTX2_Msk  (0x1UL << PORT_RDTX_RDTX2_Pos)
#define PORT_RDTX_RDTX2      PORT_RDTX_RDTX2_Msk

#define PORT_RDTX_RDTX3_Pos  3
#define PORT_RDTX_RDTX3_Msk  (0x1UL << PORT_RDTX_RDTX3_Pos)
#define PORT_RDTX_RDTX3      PORT_RDTX_RDTX3_Msk

#define PORT_RDTX_RDTX4_Pos  4
#define PORT_RDTX_RDTX4_Msk  (0x1UL << PORT_RDTX_RDTX4_Pos)
#define PORT_RDTX_RDTX4      PORT_RDTX_RDTX4_Msk

#define PORT_RDTX_RDTX5_Pos  5
#define PORT_RDTX_RDTX5_Msk  (0x1UL << PORT_RDTX_RDTX5_Pos)
#define PORT_RDTX_RDTX5      PORT_RDTX_RDTX5_Msk

#define PORT_RDTX_RDTX6_Pos  6
#define PORT_RDTX_RDTX6_Msk  (0x1UL << PORT_RDTX_RDTX6_Pos)
#define PORT_RDTX_RDTX6      PORT_RDTX_RDTX6_Msk

#define PORT_RDTX_RDTX7_Pos  7
#define PORT_RDTX_RDTX7_Msk  (0x1UL << PORT_RDTX_RDTX7_Pos)
#define PORT_RDTX_RDTX7      PORT_RDTX_RDTX7_Msk

#define PORT_RDTX_RDTX8_Pos  8
#define PORT_RDTX_RDTX8_Msk  (0x1UL << PORT_RDTX_RDTX8_Pos)
#define PORT_RDTX_RDTX8      PORT_RDTX_RDTX8_Msk

#define PORT_RDTX_RDTX9_Pos  9
#define PORT_RDTX_RDTX9_Msk  (0x1UL << PORT_RDTX_RDTX9_Pos)
#define PORT_RDTX_RDTX9      PORT_RDTX_RDTX9_Msk

#define PORT_RDTX_RDTX10_Pos 10
#define PORT_RDTX_RDTX10_Msk (0x1UL << PORT_RDTX_RDTX10_Pos)
#define PORT_RDTX_RDTX10     PORT_RDTX_RDTX10_Msk

#define PORT_RDTX_RDTX11_Pos 11
#define PORT_RDTX_RDTX11_Msk (0x1UL << PORT_RDTX_RDTX11_Pos)
#define PORT_RDTX_RDTX11     PORT_RDTX_RDTX11_Msk

#define PORT_RDTX_RDTX12_Pos 12
#define PORT_RDTX_RDTX12_Msk (0x1UL << PORT_RDTX_RDTX12_Pos)
#define PORT_RDTX_RDTX12     PORT_RDTX_RDTX12_Msk

#define PORT_RDTX_RDTX13_Pos 13
#define PORT_RDTX_RDTX13_Msk (0x1UL << PORT_RDTX_RDTX13_Pos)
#define PORT_RDTX_RDTX13     PORT_RDTX_RDTX13_Msk

#define PORT_RDTX_RDTX14_Pos 14
#define PORT_RDTX_RDTX14_Msk (0x1UL << PORT_RDTX_RDTX14_Pos)
#define PORT_RDTX_RDTX14     PORT_RDTX_RDTX14_Msk

#define PORT_RDTX_RDTX15_Pos 15
#define PORT_RDTX_RDTX15_Msk (0x1UL << PORT_RDTX_RDTX15_Pos)
#define PORT_RDTX_RDTX15     PORT_RDTX_RDTX15_Msk

/** @} */ /* End of group MDR32VF0xI_PORT_RDTX */

/** @} */ /* End of group MDR32VF0xI_PORT_Defines */

/** @} */ /* End of group MDR32VF0xI_PORT */

/** @defgroup MDR32VF0xI_CRC MDR_CRC
 * @{
 */

/**
 * @brief MDR32VF0xI CRC control structure.
 */
typedef struct {
    __IO uint32_t CTRL;  /*!<CRC Control Register */
    __IO uint32_t STAT;  /*!<CRC Status Register */
    __O uint32_t  DATAI; /*!<CRC Input Fifo Data Register */
    __IO uint32_t VAL;   /*!<CRC Value Register */
    __IO uint32_t POL;   /*!<CRC Polynomial Register */
} MDR_CRC_TypeDef;

/** @defgroup MDR32VF0xI_CRC_Defines CRC Defines
 * @{
 */

/** @defgroup MDR32VF0xI_CRC_CTRL CRC CTRL
 * @{
 */

#define CRC_CTRL_CRCEN_Pos    0
#define CRC_CTRL_CRCEN_Msk    (0x1UL << CRC_CTRL_CRCEN_Pos)
#define CRC_CTRL_CRCEN        CRC_CTRL_CRCEN_Msk

#define CRC_CTRL_DATAINV_Pos  1
#define CRC_CTRL_DATAINV_Msk  (0x1UL << CRC_CTRL_DATAINV_Pos)
#define CRC_CTRL_DATAINV      CRC_CTRL_DATAINV_Msk

#define CRC_CTRL_DMAEN_Pos    2
#define CRC_CTRL_DMAEN_Msk    (0x1UL << CRC_CTRL_DMAEN_Pos)
#define CRC_CTRL_DMAEN        CRC_CTRL_DMAEN_Msk

#define CRC_CTRL_DLSIZE_Pos   3
#define CRC_CTRL_DLSIZE_Msk   (0x3UL << CRC_CTRL_DLSIZE_Pos)
#define CRC_CTRL_DLSIZE_8BIT  (0x0UL << CRC_CTRL_DLSIZE_Pos)
#define CRC_CTRL_DLSIZE_16BIT (0x1UL << CRC_CTRL_DLSIZE_Pos)
#define CRC_CTRL_DLSIZE_32BIT (0x2UL << CRC_CTRL_DLSIZE_Pos)

#define CRC_CTRL_DCSIZE_Pos   5
#define CRC_CTRL_DCSIZE_Msk   (0x3UL << CRC_CTRL_DCSIZE_Pos)
#define CRC_CTRL_DCSIZE_8BIT  (0x0UL << CRC_CTRL_DCSIZE_Pos)
#define CRC_CTRL_DCSIZE_16BIT (0x1UL << CRC_CTRL_DCSIZE_Pos)
#define CRC_CTRL_DCSIZE_32BIT (0x2UL << CRC_CTRL_DCSIZE_Pos)

/** @} */ /* End of group MDR32VF0xI_CRC_CTRL */

/** @defgroup MDR32VF0xI_CRC_STAT CRC STAT
 * @{
 */

/* Bit field positions: */
#define CRC_STAT_CONVCOMP_Pos  0
#define CRC_STAT_CONVCOMP_Msk  (0x1UL << CRC_STAT_CONVCOMP_Pos)
#define CRC_STAT_CONVCOMP      CRC_STAT_CONVCOMP_Msk

#define CRC_STAT_FIFOFULL_Pos  1
#define CRC_STAT_FIFOFULL_Msk  (0x1UL << CRC_STAT_FIFOFULL_Pos)
#define CRC_STAT_FIFOFULL      CRC_STAT_FIFOFULL_Msk

#define CRC_STAT_FIFOEMPTY_Pos 2
#define CRC_STAT_FIFOEMPTY_Msk (0x1UL << CRC_STAT_FIFOEMPTY_Pos)
#define CRC_STAT_FIFOEMPTY     CRC_STAT_FIFOEMPTY_Msk

#define CRC_STAT_FIFOOVER_Pos  3
#define CRC_STAT_FIFOOVER_Msk  (0x1UL << CRC_STAT_FIFOOVER_Pos)
#define CRC_STAT_FIFOOVER      CRC_STAT_FIFOOVER_Msk

/** @} */ /* End of group MDR32VF0xI_CRC_STAT */

/** @defgroup MDR32VF0xI_CRC_DATAI CRC DATAI
 * @{
 */

#define CRC_DATAI_DATA_IN_Pos 0
#define CRC_DATAI_DATA_IN_Msk (0xFFFFFFFFUL << CRC_DATAI_DATA_IN_Pos)

/** @} */ /* End of group MDR32VF0xI_CRC_DATAI */

/** @defgroup MDR32VF0xI_CRC_VAL CRC VAL
 * @{
 */

#define CRC_VAL_CRCOUT_Pos 0
#define CRC_VAL_CRCOUT_Msk (0xFFFFUL << CRC_VAL_CRCOUT_Pos)

/** @} */ /* End of group MDR32VF0xI_CRC_VAL */

/** @defgroup MDR32VF0xI_CRC_POL CRC POL
 * @{
 */

#define CRC_POL_CRC_POL_Pos 0
#define CRC_POL_CRC_POL_Msk (0x1FFFFUL << CRC_POL_CRC_POL_Pos)

/** @} */ /* End of group MDR32VF0xI_CRC_POL */

/** @} */ /* End of group MDR32VF0xI_CRC_Defines */

/** @} */ /* End of group MDR32VF0xI_CRC */

#if defined(USE_MDR32F02)

/** @defgroup MDR32VF0xI_SENSORS MDR_SENSORS
 * @{
 */

/**
 * @brief MDR32VF0xI SENSORS control structure.
 */
typedef struct {
    __IO uint32_t STATE_REG;  /*!<SENSORS State Register */
    __I uint32_t  REAL_TIME;  /*!<SENSORS Real-Time State Monitoring Register */
    __I uint32_t  RESERVED0;  /*!<Reserved */
    __IO uint32_t INT_MASK;   /*!<SENSORS Interrupt Mask Register */
    __IO uint32_t K_RES_MASK; /*!<SENSORS Key Reset Signal Mask Register */
    __I uint32_t  RESERVED1;  /*!<Reserved */
    __IO uint32_t ENABLE;     /*!<SENSORS Enable Register */
    __IO uint32_t MESH_KEY7;  /*!<SENSORS Active Mesh Key Register 7 (bits [255 ... 224]) */
    __IO uint32_t MESH_KEY6;  /*!<SENSORS Active Mesh Key Register 6 (bits [223 ... 192]) */
    __IO uint32_t MESH_KEY5;  /*!<SENSORS Active Mesh Key Register 5 (bits [191 ... 160]) */
    __IO uint32_t MESH_KEY4;  /*!<SENSORS Active Mesh Key Register 4 (bits [159 ... 128]) */
    __IO uint32_t MESH_KEY3;  /*!<SENSORS Active Mesh Key Register 3 (bits [127 ... 96]) */
    __IO uint32_t MESH_KEY2;  /*!<SENSORS Active Mesh Key Register 2 (bits [95 ... 64]) */
    __IO uint32_t MESH_KEY1;  /*!<SENSORS Active Mesh Key Register 1 (bits [63 ... 32]) */
    __IO uint32_t MESH_KEY0;  /*!<SENSORS Active Mesh Key Register 0 (bits [31 ... 0]) */
    __IO uint32_t MESH_INIT1; /*!<SENSORS Active Mesh Init Value Register 1 (bits [63 ... 32]) */
    __IO uint32_t MESH_INIT0; /*!<SENSORS Active Mesh init value Register 0 (bits [31 ... 0])  */
    __IO uint32_t MESHCNTR;   /*!<SENSORS Active Mesh Control Register */
} MDR_SENSORS_TypeDef;

/** @defgroup MDR32VF0xI_SENSORS_Defines SENSORS Defines
 * @{
 */

/** @defgroup MDR32VF0xI_SENSORS_STATE_REG SENSORS STATE_REG
 * @{
 */

#define SENSORS_STATE_REG_LIGHT_SENSOR_ALARM_Pos 0
#define SENSORS_STATE_REG_LIGHT_SENSOR_ALARM_Msk (0x1UL << SENSORS_STATE_REG_LIGHT_SENSOR_ALARM_Pos)
#define SENSORS_STATE_REG_LIGHT_SENSOR_ALARM     SENSORS_STATE_REG_LIGHT_SENSOR_ALARM_Msk

#define SENSORS_STATE_REG_MESH_EMFI_ALARM1_Pos   1
#define SENSORS_STATE_REG_MESH_EMFI_ALARM1_Msk   (0x1UL << SENSORS_STATE_REG_MESH_EMFI_ALARM1_Pos)
#define SENSORS_STATE_REG_MESH_EMFI_ALARM1       SENSORS_STATE_REG_MESH_EMFI_ALARM1_Msk

#define SENSORS_STATE_REG_MESH_EMFI_ALARM2_Pos   2
#define SENSORS_STATE_REG_MESH_EMFI_ALARM2_Msk   (0x1UL << SENSORS_STATE_REG_MESH_EMFI_ALARM2_Pos)
#define SENSORS_STATE_REG_MESH_EMFI_ALARM2       SENSORS_STATE_REG_MESH_EMFI_ALARM2_Msk

#define SENSORS_STATE_REG_MESH_EMFI_ALARM3_Pos   3
#define SENSORS_STATE_REG_MESH_EMFI_ALARM3_Msk   (0x1UL << SENSORS_STATE_REG_MESH_EMFI_ALARM3_Pos)
#define SENSORS_STATE_REG_MESH_EMFI_ALARM3       SENSORS_STATE_REG_MESH_EMFI_ALARM3_Msk

#define SENSORS_STATE_REG_MESH_ACTIVE_ALARM_Pos  4
#define SENSORS_STATE_REG_MESH_ACTIVE_ALARM_Msk  (0x1UL << SENSORS_STATE_REG_MESH_ACTIVE_ALARM_Pos)
#define SENSORS_STATE_REG_MESH_ACTIVE_ALARM      SENSORS_STATE_REG_MESH_ACTIVE_ALARM_Msk

/** @} */ /* End of group MDR32VF0xI_SENSORS_STATE_REG */

/** @defgroup MDR32VF0xI_SENSORS_REAL_TIME SENSORS REAL_TIME
 * @{
 */

#define SENSORS_REAL_TIME_LIGHT_SENSOR_ALARM_Pos 0
#define SENSORS_REAL_TIME_LIGHT_SENSOR_ALARM_Msk (0x1UL << SENSORS_REAL_TIME_LIGHT_SENSOR_ALARM_Pos)
#define SENSORS_REAL_TIME_LIGHT_SENSOR_ALARM     SENSORS_REAL_TIME_LIGHT_SENSOR_ALARM_Msk

#define SENSORS_REAL_TIME_MESH_EMFI_ALARM1_Pos   1
#define SENSORS_REAL_TIME_MESH_EMFI_ALARM1_Msk   (0x1UL << SENSORS_REAL_TIME_MESH_EMFI_ALARM1_Pos)
#define SENSORS_REAL_TIME_MESH_EMFI_ALARM1       SENSORS_REAL_TIME_MESH_EMFI_ALARM1_Msk

#define SENSORS_REAL_TIME_MESH_EMFI_ALARM2_Pos   2
#define SENSORS_REAL_TIME_MESH_EMFI_ALARM2_Msk   (0x1UL << SENSORS_REAL_TIME_MESH_EMFI_ALARM2_Pos)
#define SENSORS_REAL_TIME_MESH_EMFI_ALARM2       SENSORS_REAL_TIME_MESH_EMFI_ALARM2_Msk

#define SENSORS_REAL_TIME_MESH_EMFI_ALARM3_Pos   3
#define SENSORS_REAL_TIME_MESH_EMFI_ALARM3_Msk   (0x1UL << SENSORS_REAL_TIME_MESH_EMFI_ALARM3_Pos)
#define SENSORS_REAL_TIME_MESH_EMFI_ALARM3       SENSORS_REAL_TIME_MESH_EMFI_ALARM3_Msk

#define SENSORS_REAL_TIME_MESH_ACTIVE_ALARM_Pos  4
#define SENSORS_REAL_TIME_MESH_ACTIVE_ALARM_Msk  (0x1UL << SENSORS_REAL_TIME_MESH_ACTIVE_ALARM_Pos)
#define SENSORS_REAL_TIME_MESH_ACTIVE_ALARM      SENSORS_REAL_TIME_MESH_ACTIVE_ALARM_Msk

/** @} */ /* End of group MDR32VF0xI_SENSORS_REAL_TIME */

/** @defgroup MDR32VF0xI_SENSORS_INT_MASK SENSORS INT_MASK
 * @{
 */

#define SENSORS_INT_MASK_LIGHT_SENSOR_ALARM_Pos 0
#define SENSORS_INT_MASK_LIGHT_SENSOR_ALARM_Msk (0x1UL << SENSORS_INT_MASK_LIGHT_SENSOR_ALARM_Pos)
#define SENSORS_INT_MASK_LIGHT_SENSOR_ALARM     SENSORS_INT_MASK_LIGHT_SENSOR_ALARM_Msk

#define SENSORS_INT_MASK_MESH_EMFI_ALARM1_Pos   1
#define SENSORS_INT_MASK_MESH_EMFI_ALARM1_Msk   (0x1UL << SENSORS_INT_MASK_MESH_EMFI_ALARM1_Pos)
#define SENSORS_INT_MASK_MESH_EMFI_ALARM1       SENSORS_INT_MASK_MESH_EMFI_ALARM1_Msk

#define SENSORS_INT_MASK_MESH_EMFI_ALARM2_Pos   2
#define SENSORS_INT_MASK_MESH_EMFI_ALARM2_Msk   (0x1UL << SENSORS_INT_MASK_MESH_EMFI_ALARM2_Pos)
#define SENSORS_INT_MASK_MESH_EMFI_ALARM2       SENSORS_INT_MASK_MESH_EMFI_ALARM2_Msk

#define SENSORS_INT_MASK_MESH_EMFI_ALARM3_Pos   3
#define SENSORS_INT_MASK_MESH_EMFI_ALARM3_Msk   (0x1UL << SENSORS_INT_MASK_MESH_EMFI_ALARM3_Pos)
#define SENSORS_INT_MASK_MESH_EMFI_ALARM3       SENSORS_INT_MASK_MESH_EMFI_ALARM3_Msk

#define SENSORS_INT_MASK_MESH_ACTIVE_ALARM_Pos  4
#define SENSORS_INT_MASK_MESH_ACTIVE_ALARM_Msk  (0x1UL << SENSORS_INT_MASK_MESH_ACTIVE_ALARM_Pos)
#define SENSORS_INT_MASK_MESH_ACTIVE_ALARM      SENSORS_INT_MASK_MESH_ACTIVE_ALARM_Msk

/** @} */ /* End of group MDR32VF0xI_SENSORS_INT_MASK */

/** @defgroup MDR32VF0xI_SENSORS_K_RES_MASK SENSORS K_RES_MASK
 * @{
 */

#define SENSORS_K_RES_MASK_LIGHT_SENSOR_ALARM_Pos 0
#define SENSORS_K_RES_MASK_LIGHT_SENSOR_ALARM_Msk (0x1UL << SENSORS_K_RES_MASK_LIGHT_SENSOR_ALARM_Pos)
#define SENSORS_K_RES_MASK_LIGHT_SENSOR_ALARM     SENSORS_K_RES_MASK_LIGHT_SENSOR_ALARM_Msk

#define SENSORS_K_RES_MASK_MESH_EMFI_ALARM1_Pos   1
#define SENSORS_K_RES_MASK_MESH_EMFI_ALARM1_Msk   (0x1UL << SENSORS_K_RES_MASK_MESH_EMFI_ALARM1_Pos)
#define SENSORS_K_RES_MASK_MESH_EMFI_ALARM1       SENSORS_K_RES_MASK_MESH_EMFI_ALARM1_Msk

#define SENSORS_K_RES_MASK_MESH_EMFI_ALARM2_Pos   2
#define SENSORS_K_RES_MASK_MESH_EMFI_ALARM2_Msk   (0x1UL << SENSORS_K_RES_MASK_MESH_EMFI_ALARM2_Pos)
#define SENSORS_K_RES_MASK_MESH_EMFI_ALARM2       SENSORS_K_RES_MASK_MESH_EMFI_ALARM2_Msk

#define SENSORS_K_RES_MASK_MESH_EMFI_ALARM3_Pos   3
#define SENSORS_K_RES_MASK_MESH_EMFI_ALARM3_Msk   (0x1UL << SENSORS_K_RES_MASK_MESH_EMFI_ALARM3_Pos)
#define SENSORS_K_RES_MASK_MESH_EMFI_ALARM3       SENSORS_K_RES_MASK_MESH_EMFI_ALARM3_Msk

#define SENSORS_K_RES_MASK_MESH_ACTIVE_ALARM_Pos  4
#define SENSORS_K_RES_MASK_MESH_ACTIVE_ALARM_Msk  (0x1UL << SENSORS_K_RES_MASK_MESH_ACTIVE_ALARM_Pos)
#define SENSORS_K_RES_MASK_MESH_ACTIVE_ALARM      SENSORS_K_RES_MASK_MESH_ACTIVE_ALARM_Msk

/** @} */ /* End of group MDR32VF0xI_SENSORS_K_RES_MASK */

/** @defgroup MDR32VF0xI_SENSORS_ENABLE SENSORS ENABLE
 * @{
 */

#define SENSORS_ENABLE_LIGHT_SENSOR_ALARM_Pos 0
#define SENSORS_ENABLE_LIGHT_SENSOR_ALARM_Msk (0x1UL << SENSORS_ENABLE_LIGHT_SENSOR_ALARM_Pos)
#define SENSORS_ENABLE_LIGHT_SENSOR_ALARM     SENSORS_ENABLE_LIGHT_SENSOR_ALARM_Msk

#define SENSORS_ENABLE_MESH_EMFI_ALARM1_Pos   1
#define SENSORS_ENABLE_MESH_EMFI_ALARM1_Msk   (0x1UL << SENSORS_ENABLE_MESH_EMFI_ALARM1_Pos)
#define SENSORS_ENABLE_MESH_EMFI_ALARM1       SENSORS_ENABLE_MESH_EMFI_ALARM1_Msk

#define SENSORS_ENABLE_MESH_EMFI_ALARM2_Pos   2
#define SENSORS_ENABLE_MESH_EMFI_ALARM2_Msk   (0x1UL << SENSORS_ENABLE_MESH_EMFI_ALARM2_Pos)
#define SENSORS_ENABLE_MESH_EMFI_ALARM2       SENSORS_ENABLE_MESH_EMFI_ALARM2_Msk

#define SENSORS_ENABLE_MESH_EMFI_ALARM3_Pos   3
#define SENSORS_ENABLE_MESH_EMFI_ALARM3_Msk   (0x1UL << SENSORS_ENABLE_MESH_EMFI_ALARM3_Pos)
#define SENSORS_ENABLE_MESH_EMFI_ALARM3       SENSORS_ENABLE_MESH_EMFI_ALARM3_Msk

#define SENSORS_ENABLE_MESH_ACTIVE_ALARM_Pos  4
#define SENSORS_ENABLE_MESH_ACTIVE_ALARM_Msk  (0x1UL << SENSORS_ENABLE_MESH_ACTIVE_ALARM_Pos)
#define SENSORS_ENABLE_MESH_ACTIVE_ALARM      SENSORS_ENABLE_MESH_ACTIVE_ALARM_Msk

/** @} */ /* End of group MDR32VF0xI_SENSORS_ENABLE */

/** @defgroup MDR32VF0xI_SENSORS_MESHCNTR SENSORS ENABLE
 * @{
 */

#define SENSORS_MESHCNTR_MESH_CLK_DIV_Pos 0
#define SENSORS_MESHCNTR_MESH_CLK_DIV_Msk (0x7UL << SENSORS_MESHCNTR_MESH_CLK_DIV_Pos)
#define SENSORS_MESHCNTR_MESH_CLK_DIV_3   (0x0UL << SENSORS_MESHCNTR_MESH_CLK_DIV_Pos)
#define SENSORS_MESHCNTR_MESH_CLK_DIV_4   (0x1UL << SENSORS_MESHCNTR_MESH_CLK_DIV_Pos)
#define SENSORS_MESHCNTR_MESH_CLK_DIV_8   (0x2UL << SENSORS_MESHCNTR_MESH_CLK_DIV_Pos)
#define SENSORS_MESHCNTR_MESH_CLK_DIV_16  (0x3UL << SENSORS_MESHCNTR_MESH_CLK_DIV_Pos)
#define SENSORS_MESHCNTR_MESH_CLK_DIV_32  (0x4UL << SENSORS_MESHCNTR_MESH_CLK_DIV_Pos)
#define SENSORS_MESHCNTR_MESH_CLK_DIV_64  (0x5UL << SENSORS_MESHCNTR_MESH_CLK_DIV_Pos)
#define SENSORS_MESHCNTR_MESH_CLK_DIV_128 (0x6UL << SENSORS_MESHCNTR_MESH_CLK_DIV_Pos)
#define SENSORS_MESHCNTR_MESH_CLK_DIV_265 (0x7UL << SENSORS_MESHCNTR_MESH_CLK_DIV_Pos)

/** @} */ /* End of group MDR32VF0xI_SENSORS_MESHCNTR */

/** @} */ /* End of group MDR32VF0xI_SENSORS_Defines */

/** @} */ /* End of group MDR32VF0xI_SENSORS */

/** @defgroup MDR32VF0xI_CLK_MEASURE MDR_CLK_MEASURE
 * @{
 */

/**
 * @brief MDR32VF0xI CLK_MEASURE control structure.
 */
typedef struct {
    __IO uint32_t CNTR_STAT;       /*!<CLK_MEASURE State and Control Register */
    __IO uint32_t ALARM_SHIFT_RST; /*!<CLK_MEASURE Alarm Threshold Register for Key Reset */
    __IO uint32_t ALARM_SHIFT_INT; /*!<CLK_MEASURE Alarm Threshold Register for Interrupt Generation */
    __IO uint32_t ALARM_PREG_0;    /*!<CLK_MEASURE Counters Reset Setup Register */
    __IO uint32_t ALARM_PREG_1;    /*!<CLK_MEASURE Counters Reset Setup Register */
    __IO uint32_t STAT;            /*!<CLK_MEASURE SHIFT_REG Max Value Register */
} MDR_CLK_MEASURE_TypeDef;

/** @defgroup MDR32VF0xI_CLK_MEASURE_Defines CLK_MEASURE Defines
 * @{
 */

/** @defgroup MDR32VF0xI_CLK_MEASURE_CNTR_STAT CLK_MEASURE CNTR_STAT
 * @{
 */

#define CLK_MEASURE_CNTR_STAT_EN_Pos        0
#define CLK_MEASURE_CNTR_STAT_EN_Msk        (0x1UL << CLK_MEASURE_CNTR_STAT_EN_Pos)
#define CLK_MEASURE_CNTR_STAT_EN            CLK_MEASURE_CNTR_STAT_EN_Msk

#define CLK_MEASURE_CNTR_STAT_INTERRUPT_Pos 4
#define CLK_MEASURE_CNTR_STAT_INTERRUPT_Msk (0x1UL << CLK_MEASURE_CNTR_STAT_INTERRUPT_Pos)
#define CLK_MEASURE_CNTR_STAT_INTERRUPT     CLK_MEASURE_CNTR_STAT_INTERRUPT_Msk

#define CLK_MEASURE_CNTR_STAT_KEY_RESET_Pos 5
#define CLK_MEASURE_CNTR_STAT_KEY_RESET_Msk (0x1UL << CLK_MEASURE_CNTR_STAT_KEY_RESET_Pos)
#define CLK_MEASURE_CNTR_STAT_KEY_RESET     CLK_MEASURE_CNTR_STAT_KEY_RESET_Msk

/** @} */ /* End of group MDR32VF0xI_CLK_MEASURE_CNTR_STAT */

/** @defgroup MDR32VF0xI_CLK_MEASURE_ALARM_SHIFT_RST CLK_MEASURE ALARM_SHIFT_RST
 * @{
 */

#define CLK_MEASURE_ALARM_SHIFT_RST_BASE_REG0_Pos 0
#define CLK_MEASURE_ALARM_SHIFT_RST_BASE_REG0_Msk (0xFFFFUL << CLK_MEASURE_ALARM_SHIFT_RST_BASE_REG0_Pos)

#define CLK_MEASURE_ALARM_SHIFT_RST_BASE_REG2_Pos 16
#define CLK_MEASURE_ALARM_SHIFT_RST_BASE_REG2_Msk (0xFFFFUL << CLK_MEASURE_ALARM_SHIFT_RST_BASE_REG2_Pos)

/** @} */ /* End of group MDR32VF0xI_CLK_MEASURE_ALARM_SHIFT_RST */

/** @defgroup MDR32VF0xI_CLK_MEASURE_ALARM_SHIFT_INT CLK_MEASURE ALARM_SHIFT_INT
 * @{
 */

#define CLK_MEASURE_ALARM_SHIFT_INT_BASE_REG1_Pos 0
#define CLK_MEASURE_ALARM_SHIFT_INT_BASE_REG1_Msk (0xFFFFUL << CLK_MEASURE_ALARM_SHIFT_INT_BASE_REG1_Pos)

#define CLK_MEASURE_ALARM_SHIFT_INT_BASE_REG3_Pos 16
#define CLK_MEASURE_ALARM_SHIFT_INT_BASE_REG3_Msk (0xFFFFUL << CLK_MEASURE_ALARM_SHIFT_INT_BASE_REG3_Pos)

/** @} */ /* End of group MDR32VF0xI_CLK_MEASURE_ALARM_SHIFT_INT */

/** @defgroup MDR32VF0xI_CLK_MEASURE_ALARM_PREG_0 CLK_MEASURE ALARM_PREG_0
 * @{
 */

#define CLK_MEASURE_ALARM_PREG_0_PREG1_Pos 0
#define CLK_MEASURE_ALARM_PREG_0_PREG1_Msk (0xFFFFUL << CLK_MEASURE_ALARM_PREG_0_PREG1_Pos)

#define CLK_MEASURE_ALARM_PREG_0_PREG0_Pos 16
#define CLK_MEASURE_ALARM_PREG_0_PREG0_Msk (0xFFFFUL << CLK_MEASURE_ALARM_PREG_0_PREG0_Pos)

/** @} */ /* End of group MDR32VF0xI_CLK_MEASURE_ALARM_PREG_0 */

/** @defgroup MDR32VF0xI_CLK_MEASURE_ALARM_PREG_1 CLK_MEASURE ALARM_PREG_1
 * @{
 */

#define CLK_MEASURE_ALARM_PREG_1_PREG3_Pos 0
#define CLK_MEASURE_ALARM_PREG_1_PREG3_Msk (0xFFFFUL << CLK_MEASURE_ALARM_PREG_1_PREG3_Pos)

#define CLK_MEASURE_ALARM_PREG_1_PREG2_Pos 16
#define CLK_MEASURE_ALARM_PREG_1_PREG2_Msk (0xFFFFUL << CLK_MEASURE_ALARM_PREG_1_PREG2_Pos)

/** @} */ /* End of group MDR32VF0xI_CLK_MEASURE_ALARM_PREG_1 */

/** @defgroup MDR32VF0xI_CLK_MEASURE_STAT CLK_MEASURE STAT
 * @{
 */

#define CLK_MEASURE_STAT_MAX_SHIFT1_Pos 0
#define CLK_MEASURE_STAT_MAX_SHIFT1_Msk (0xFFFFUL << CLK_MEASURE_STAT_MAX_SHIFT1_Pos)

#define CLK_MEASURE_STAT_MAX_SHIFT0_Pos 16
#define CLK_MEASURE_STAT_MAX_SHIFT0_Msk (0xFFFFUL << CLK_MEASURE_STAT_MAX_SHIFT0_Pos)

/** @} */ /* End of group MDR32VF0xI_CLK_MEASURE_STAT */

/** @} */ /* End of group MDR32VF0xI_CLK_MEASURE_Defines */

/** @} */ /* End of group MDR32VF0xI_CLK_MEASURE */

#endif

/** @defgroup MDR32VF0xI_RANDOM MDR_RANDOM
 * @{
 */

/**
 * @brief MDR32VF0xI RANDOM control structure.
 */
typedef struct {
    __IO uint32_t STAT_CTRL; /*!<RANDOM State and Control Register */
    __IO uint32_t INT_CTRL;  /*!<RANDOM Interrupt Control Register */
    __IO uint32_t CLK_DIV;   /*!<RANDOM Clock Prescaler Register */
    __IO uint32_t PAUSE;     /*!<RANDOM Startup Pause Register */
    __I uint32_t  OUTPUT;    /*!<RANDOM Value Register */
    __I uint32_t  PAUSE_CNT; /*!<RANDOM Pause Counter Register */
    __I uint32_t  TEMP;      /*!<RANDOM Temporary Value Register */
} MDR_RANDOM_TypeDef;

/** @defgroup MDR32VF0xI_RANDOM_Defines RANDOM Defines
 * @{
 */

/** @defgroup MDR32VF0xI_RANDOM_STAT_CTRL RANDOM STAT_CTRL
 * @{
 */

#define RANDOM_STAT_CTRL_EN_STATE_Pos            1
#define RANDOM_STAT_CTRL_EN_STATE_Msk            (0x1UL << RANDOM_STAT_CTRL_EN_STATE_Pos)
#define RANDOM_STAT_CTRL_EN_STATE                RANDOM_STAT_CTRL_EN_STATE_Msk

#define RANDOM_STAT_CTRL_BUSY_Pos                2
#define RANDOM_STAT_CTRL_BUSY_Msk                (0x1UL << RANDOM_STAT_CTRL_BUSY_Pos)
#define RANDOM_STAT_CTRL_BUSY                    RANDOM_STAT_CTRL_BUSY_Msk

#define RANDOM_STAT_CTRL_UNREAD_DATA_Pos         3
#define RANDOM_STAT_CTRL_UNREAD_DATA_Msk         (0x1UL << RANDOM_STAT_CTRL_UNREAD_DATA_Pos)
#define RANDOM_STAT_CTRL_UNREAD_DATA             RANDOM_STAT_CTRL_UNREAD_DATA_Msk

#define RANDOM_STAT_CTRL_MASK_Pos                4
#define RANDOM_STAT_CTRL_MASK_Msk                (0x3UL << RANDOM_STAT_CTRL_MASK_Pos)
#define RANDOM_STAT_CTRL_MASK_GEN0               (0x1UL << RANDOM_STAT_CTRL_MASK_Pos)
#define RANDOM_STAT_CTRL_MASK_GEN1               (0x2UL << RANDOM_STAT_CTRL_MASK_Pos)

#define RANDOM_STAT_CTRL_WORK_MODE_Pos           8
#define RANDOM_STAT_CTRL_WORK_MODE_Msk           (0x3UL << RANDOM_STAT_CTRL_WORK_MODE_Pos)
#define RANDOM_STAT_CTRL_WORK_MODE_STOP          (0x0UL << RANDOM_STAT_CTRL_WORK_MODE_Pos)
#define RANDOM_STAT_CTRL_WORK_MODE_SINGLE        (0x1UL << RANDOM_STAT_CTRL_WORK_MODE_Pos)
#define RANDOM_STAT_CTRL_WORK_MODE_START_ON_READ (0x2UL << RANDOM_STAT_CTRL_WORK_MODE_Pos)
#define RANDOM_STAT_CTRL_WORK_MODE_CONTINUOUS    (0x3UL << RANDOM_STAT_CTRL_WORK_MODE_Pos)

#define RANDOM_STAT_CTRL_BIT_CNT_Pos             12
#define RANDOM_STAT_CTRL_BIT_CNT_Msk             (0x3FUL << RANDOM_STAT_CTRL_BIT_CNT_Pos)

/** @} */ /* End of group MDR32VF0xI_RANDOM_STAT_CTRL */

/** @defgroup MDR32VF0xI_RANDOM_INT_CTRL RANDOM INT_CTRL
 * @{
 */

#define RANDOM_INT_CTRL_INT_EN_Pos      0
#define RANDOM_INT_CTRL_INT_EN_Msk      (0x1UL << RANDOM_INT_CTRL_INT_EN_Pos)
#define RANDOM_INT_CTRL_INT_EN          RANDOM_INT_CTRL_INT_EN_Msk

#define RANDOM_INT_CTRL_UNREAD_DATA_Pos 1
#define RANDOM_INT_CTRL_UNREAD_DATA_Msk (0x1UL << RANDOM_INT_CTRL_UNREAD_DATA_Pos)
#define RANDOM_INT_CTRL_UNREAD_DATA     RANDOM_INT_CTRL_UNREAD_DATA_Msk

/** @} */ /* End of group MDR32VF0xI_RANDOM_INT_CTRL */

/** @defgroup MDR32VF0xI_RANDOM_CLK_DIV RANDOM CLK_DIV
 * @{
 */

#define RANDOM_CLK_DIV_DIV_Pos 0
#define RANDOM_CLK_DIV_DIV_Msk (0xFFFFUL << RANDOM_CLK_DIV_DIV_Pos)

/** @} */ /* End of group MDR32VF0xI_RANDOM_CLK_DIV */

/** @defgroup MDR32VF0xI_RANDOM_PAUSE RANDOM PAUSE
 * @{
 */

#define RANDOM_PAUSE_PAUSE_Pos 0
#define RANDOM_PAUSE_PAUSE_Msk (0xFFFFFFFFUL << RANDOM_PAUSE_PAUSE_Pos)

/** @} */ /* End of group MDR32VF0xI_RANDOM_PAUSE */

/** @defgroup MDR32VF0xI_RANDOM_OUTPUT RANDOM OUTPUT
 * @{
 */

#define RANDOM_OUTPUT_DATA_Pos 0
#define RANDOM_OUTPUT_DATA_Msk (0xFFFFFFFFUL << RANDOM_OUTPUT_DATA_Pos)

/** @} */ /* End of group MDR32VF0xI_RANDOM_OUTPUT */

/** @defgroup MDR32VF0xI_RANDOM_PAUSE_CNT RANDOM PAUSE_CNT
 * @{
 */

#define RANDOM_PAUSE_CNT_CUR_PAUSE_Pos 0
#define RANDOM_PAUSE_CNT_CUR_PAUSE_Msk (0xFFFFFFFFUL << RANDOM_PAUSE_CNT_CUR_PAUSE_Pos)

/** @} */ /* End of group MDR32VF0xI_RANDOM_PAUSE_CNT */

/** @defgroup MDR32VF0xI_RANDOM_TEMP RANDOM TEMP
 * @{
 */

#define RANDOM_TEMP_TEMP_DATA_Pos 0
#define RANDOM_TEMP_TEMP_DATA_Msk (0xFFFFFFFFUL << RANDOM_TEMP_TEMP_DATA_Pos)

/** @} */ /* End of group MDR32VF0xI_RANDOM_TEMP */

/** @} */ /* End of group MDR32VF0xI_RANDOM_Defines */

/** @} */ /* End of group MDR32VF0xI_RANDOM */

/** @defgroup MDR32VF0xI_USART MDR_USART
 * @{
 */

/**
 * @brief MDR32VF0xI USART control structure.
 */
typedef struct {
    __IO uint32_t SR;   /*!<USART Status Register */
    __IO uint32_t DR;   /*!<USART Data Register */
    __IO uint32_t BRR;  /*!<USART Baud Rate Control Register */
    __IO uint32_t CR1;  /*!<USART Control1 Register */
    __IO uint32_t CR2;  /*!<USART Control2 Register */
    __IO uint32_t CR3;  /*!<USART Control3 Register */
    __IO uint32_t GTPR; /*!<USART Guard Time and Prescaler Register */
} MDR_USART_TypeDef;

/** @defgroup MDR32VF0xI_USART_Defines USART Defines
 * @{
 */

/** @defgroup MDR32VF0xI_USART_SR USART SR
 * @{
 */

#define USART_SR_PE_Pos   0
#define USART_SR_PE_Msk   (0x1UL << USART_SR_PE_Pos)
#define USART_SR_PE       USART_SR_PE_Msk

#define USART_SR_FE_Pos   1
#define USART_SR_FE_Msk   (0x1UL << USART_SR_FE_Pos)
#define USART_SR_FE       USART_SR_FE_Msk

#define USART_SR_NF_Pos   2
#define USART_SR_NF_Msk   (0x1UL << USART_SR_NF_Pos)
#define USART_SR_NF       USART_SR_NF_Msk

#define USART_SR_ORE_Pos  3
#define USART_SR_ORE_Msk  (0x1UL << USART_SR_ORE_Pos)
#define USART_SR_ORE      USART_SR_ORE_Msk

#define USART_SR_IDLE_Pos 4
#define USART_SR_IDLE_Msk (0x1UL << USART_SR_IDLE_Pos)
#define USART_SR_IDLE     USART_SR_IDLE_Msk

#define USART_SR_RXNE_Pos 5
#define USART_SR_RXNE_Msk (0x1UL << USART_SR_RXNE_Pos)
#define USART_SR_RXNE     USART_SR_RXNE_Msk

#define USART_SR_TC_Pos   6
#define USART_SR_TC_Msk   (0x1UL << USART_SR_TC_Pos)
#define USART_SR_TC       USART_SR_TC_Msk

#define USART_SR_TXE_Pos  7
#define USART_SR_TXE_Msk  (0x1UL << USART_SR_TXE_Pos)
#define USART_SR_TXE      USART_SR_TXE_Msk

#define USART_SR_CTS_Pos  9
#define USART_SR_CTS_Msk  (0x1UL << USART_SR_CTS_Pos)
#define USART_SR_CTS      USART_SR_CTS_Msk

/** @} */ /* End of group MDR32VF0xI_USART_SR */

/** @defgroup MDR32VF0xI_USART_DR USART DR
 * @{
 */

#define USART_DR_DR_Pos 0
#define USART_DR_DR_Msk (0x1FFUL << USART_DR_DR_Pos)

/** @} */ /* End of group MDR32VF0xI_USART_DR */

/** @defgroup MDR32VF0xI_USART_BRR USART BRR
 * @{
 */

#define USART_BRR_DIV_FRACTION_Pos 0
#define USART_BRR_DIV_FRACTION_Msk (0xFUL << USART_BRR_DIV_FRACTION_Pos)

#define USART_BRR_DIV_MANTISSA_Pos 4
#define USART_BRR_DIV_MANTISSA_Msk (0xFFFUL << USART_BRR_DIV_MANTISSA_Pos)

/** @} */ /* End of group MDR32VF0xI_USART_BRR */

/** @defgroup MDR32VF0xI_USART_CR1 USART CR1
 * @{
 */

#define USART_CR1_SBK_Pos    0
#define USART_CR1_SBK_Msk    (0x1UL << USART_CR1_SBK_Pos)
#define USART_CR1_SBK        USART_CR1_SBK_Msk

#define USART_CR1_RWU_Pos    1
#define USART_CR1_RWU_Msk    (0x1UL << USART_CR1_RWU_Pos)
#define USART_CR1_RWU        USART_CR1_RWU_Msk

#define USART_CR1_RE_Pos     2
#define USART_CR1_RE_Msk     (0x1UL << USART_CR1_RE_Pos)
#define USART_CR1_RE         USART_CR1_RE_Msk

#define USART_CR1_TE_Pos     3
#define USART_CR1_TE_Msk     (0x1UL << USART_CR1_TE_Pos)
#define USART_CR1_TE         USART_CR1_TE_Msk

#define USART_CR1_IDLEIE_Pos 4
#define USART_CR1_IDLEIE_Msk (0x1UL << USART_CR1_IDLEIE_Pos)
#define USART_CR1_IDLEIE     USART_CR1_IDLEIE_Msk

#define USART_CR1_RXNEIE_Pos 5
#define USART_CR1_RXNEIE_Msk (0x1UL << USART_CR1_RXNEIE_Pos)
#define USART_CR1_RXNEIE     USART_CR1_RXNEIE_Msk

#define USART_CR1_TCIE_Pos   6
#define USART_CR1_TCIE_Msk   (0x1UL << USART_CR1_TCIE_Pos)
#define USART_CR1_TCIE       USART_CR1_TCIE_Msk

#define USART_CR1_TXEIE_Pos  7
#define USART_CR1_TXEIE_Msk  (0x1UL << USART_CR1_TXEIE_Pos)
#define USART_CR1_TXEIE      USART_CR1_TXEIE_Msk

#define USART_CR1_PEIE_Pos   8
#define USART_CR1_PEIE_Msk   (0x1UL << USART_CR1_PEIE_Pos)
#define USART_CR1_PEIE       USART_CR1_PEIE_Msk

#define USART_CR1_PS_Pos     9
#define USART_CR1_PS_Msk     (0x1UL << USART_CR1_PS_Pos)
#define USART_CR1_PS         USART_CR1_PS_Msk

#define USART_CR1_PCE_Pos    10
#define USART_CR1_PCE_Msk    (0x1UL << USART_CR1_PCE_Pos)
#define USART_CR1_PCE        USART_CR1_PCE_Msk

#define USART_CR1_WAKE_Pos   11
#define USART_CR1_WAKE_Msk   (0x1UL << USART_CR1_WAKE_Pos)
#define USART_CR1_WAKE       USART_CR1_WAKE_Msk

#define USART_CR1_M_Pos      12
#define USART_CR1_M_Msk      (0x1UL << USART_CR1_M_Pos)
#define USART_CR1_M          USART_CR1_M_Msk

#define USART_CR1_UE_Pos     13
#define USART_CR1_UE_Msk     (0x1UL << USART_CR1_UE_Pos)
#define USART_CR1_UE         USART_CR1_UE_Msk

#define USART_CR1_OVER8_Pos  15
#define USART_CR1_OVER8_Msk  (0x1UL << USART_CR1_OVER8_Pos)
#define USART_CR1_OVER8      USART_CR1_OVER8_Msk

/** @} */ /* End of group MDR32VF0xI_USART_CR1 */

/** @defgroup MDR32VF0xI_USART_CR2 USART CR2
 * @{
 */

#define USART_CR2_ADD_Pos   0
#define USART_CR2_ADD_Msk   (0xFUL << USART_CR2_ADD_Pos)

#define USART_CR2_LBCL_Pos  8
#define USART_CR2_LBCL_Msk  (0x1UL << USART_CR2_LBCL_Pos)
#define USART_CR2_LBCL      USART_CR2_LBCL_Msk

#define USART_CR2_CPHA_Pos  9
#define USART_CR2_CPHA_Msk  (0x1UL << USART_CR2_CPHA_Pos)
#define USART_CR2_CPHA      USART_CR2_CPHA_Msk

#define USART_CR2_CPOL_Pos  10
#define USART_CR2_CPOL_Msk  (0x1UL << USART_CR2_CPOL_Pos)
#define USART_CR2_CPOL      USART_CR2_CPOL_Msk

#define USART_CR2_CLKEN_Pos 11
#define USART_CR2_CLKEN_Msk (0x1UL << USART_CR2_CLKEN_Pos)
#define USART_CR2_CLKEN     USART_CR2_CLKEN_Msk

#define USART_CR2_STOP_Pos  12
#define USART_CR2_STOP_Msk  (0x3UL << USART_CR2_STOP_Pos)

/** @} */ /* End of group MDR32VF0xI_USART_CR2 */

/** @defgroup MDR32VF0xI_USART_CR3 USART CR3
 * @{
 */

#define USART_CR3_EIE_Pos    0
#define USART_CR3_EIE_Msk    (0x1UL << USART_CR3_EIE_Pos)
#define USART_CR3_EIE        USART_CR3_EIE_Msk

#define USART_CR3_HDSEL_Pos  3
#define USART_CR3_HDSEL_Msk  (0x1UL << USART_CR3_HDSEL_Pos)
#define USART_CR3_HDSEL      USART_CR3_HDSEL_Msk

#define USART_CR3_NACK_Pos   4
#define USART_CR3_NACK_Msk   (0x1UL << USART_CR3_NACK_Pos)
#define USART_CR3_NACK       USART_CR3_NACK_Msk

#define USART_CR3_SCEN_Pos   5
#define USART_CR3_SCEN_Msk   (0x1UL << USART_CR3_SCEN_Pos)
#define USART_CR3_SCEN       USART_CR3_SCEN_Msk

#define USART_CR3_DMAR_Pos   6
#define USART_CR3_DMAR_Msk   (0x1UL << USART_CR3_DMAR_Pos)
#define USART_CR3_DMAR       USART_CR3_DMAR_Msk

#define USART_CR3_DMAT_Pos   7
#define USART_CR3_DMAT_Msk   (0x1UL << USART_CR3_DMAT_Pos)
#define USART_CR3_DMAT       USART_CR3_DMAT_Msk

#define USART_CR3_RTSE_Pos   8
#define USART_CR3_RTSE_Msk   (0x1UL << USART_CR3_RTSE_Pos)
#define USART_CR3_RTSE       USART_CR3_RTSE_Msk

#define USART_CR3_CTSE_Pos   9
#define USART_CR3_CTSE_Msk   (0x1UL << USART_CR3_CTSE_Pos)
#define USART_CR3_CTSE       USART_CR3_CTSE_Msk

#define USART_CR3_CSTIE_Pos  10
#define USART_CR3_CSTIE_Msk  (0x1UL << USART_CR3_CSTIE_Pos)
#define USART_CR3_CSTIE      USART_CR3_CSTIE_Msk

#define USART_CR3_ONEBIT_Pos 11
#define USART_CR3_ONEBIT_Msk (0x1UL << USART_CR3_ONEBIT_Pos)
#define USART_CR3_ONEBIT     USART_CR3_ONEBIT_Msk

/** @} */ /* End of group MDR32VF0xI_USART_CR3 */

/** @defgroup MDR32VF0xI_USART_GTPR USART GTPR
 * @{
 */

#define USART_GTPR_PSC_Pos 0
#define USART_GTPR_PSC_Msk (0x1FUL << USART_GTPR_PSC_Pos)

#define USART_GTPR_GT_Pos  8
#define USART_GTPR_GT_Msk  (0xFFUL << USART_GTPR_GT_Pos)

/** @} */ /* End of group MDR32VF0xI_USART_GTPR */

/** @} */ /* End of group MDR32VF0xI_USART_Defines */

/** @} */ /* End of group MDR32VF0xI_USART */

#if defined(USE_MDR32F02)

/** @defgroup MDR32VF0xI_L_BLOCK MDR_L_BLOCK
 * @{
 */

/**
 * @brief MDR32VF0xI L_BLOCK control structure.
 */
typedef struct {
    __IO uint32_t TRANSFORM_0;    /*!<L_BLOCK Transformation Register 1 */
    __IO uint32_t TRANSFORM_1;    /*!<L_BLOCK Transformation Register 2 */
    __IO uint32_t TRANSFORM_2;    /*!<L_BLOCK Transformation Register 3 */
    __IO uint32_t TRANSFORM_3;    /*!<L_BLOCK Transformation Register 4 */
    __IO uint32_t TABLE_CHANGE_0; /*!<L_BLOCK Table Change Register 1 */
    __IO uint32_t TABLE_CHANGE_1; /*!<L_BLOCK Table Change Register 2 */
    __IO uint32_t TABLE_CHANGE_2; /*!<L_BLOCK Table Change Register 3 */
    __IO uint32_t TABLE_CHANGE_3; /*!<L_BLOCK Table Change Register 4 */
    __IO uint32_t SETUP;          /*!<L_BLOCK Transformation Setup Register */
} MDR_L_BLOCK_TypeDef;

/** @defgroup MDR32VF0xI_L_BLOCK_Defines L_BLOCK Defines
 * @{
 */

/** @defgroup MDR32VF0xI_L_BLOCK_TRANSFORM_N L_BLOCK TRANSFORM_N
 * @{
 */

#define L_BLOCK_TRANSFORM_N_DATA_Pos 0
#define L_BLOCK_TRANSFORM_N_DATA_Msk (0xFFFFFFFFUL << L_BLOCK_TRANSFORM_N_DATA_Pos)

/** @} */ /* End of group MDR32VF0xI_L_BLOCK_TRANSFORM_N */

/** @defgroup MDR32VF0xI_L_BLOCK_TABLE_CHANGE_N L_BLOCK TABLE_CHANGE_N
 * @{
 */

#define L_BLOCK_TABLE_CHANGE_N_TABLE_VAL_N0_Pos 0
#define L_BLOCK_TABLE_CHANGE_N_TABLE_VAL_N0_Msk (0xFFUL << L_BLOCK_TABLE_CHANGE_N_TABLE_VAL_N0_Pos)

#define L_BLOCK_TABLE_CHANGE_N_TABLE_VAL_N1_Pos 8
#define L_BLOCK_TABLE_CHANGE_N_TABLE_VAL_N1_Msk (0xFFUL << L_BLOCK_TABLE_CHANGE_N_TABLE_VAL_N1_Pos)

#define L_BLOCK_TABLE_CHANGE_N_TABLE_VAL_N2_Pos 16
#define L_BLOCK_TABLE_CHANGE_N_TABLE_VAL_N2_Msk (0xFFUL << L_BLOCK_TABLE_CHANGE_N_TABLE_VAL_N2_Pos)

#define L_BLOCK_TABLE_CHANGE_N_TABLE_VAL_N3_Pos 24
#define L_BLOCK_TABLE_CHANGE_N_TABLE_VAL_N3_Msk (0xFFUL << L_BLOCK_TABLE_CHANGE_N_TABLE_VAL_N3_Pos)

/** @} */ /* End of group MDR32VF0xI_L_BLOCK_TABLE_CHANGE_N */

/** @defgroup MDR32VF0xI_L_BLOCK_SETUP L_BLOCK SETUP
 * @{
 */

#define L_BLOCK_SETUP_SET_R_COUNT_Pos  0
#define L_BLOCK_SETUP_SET_R_COUNT_Msk  (0x1FUL << L_BLOCK_SETUP_SET_R_COUNT_Pos)

#define L_BLOCK_SETUP_CURR_R_COUNT_Pos 8
#define L_BLOCK_SETUP_CURR_R_COUNT_Msk (0x1FUL << L_BLOCK_SETUP_CURR_R_COUNT_Pos)

#define L_BLOCK_SETUP_SET_L_DIR_Pos    16
#define L_BLOCK_SETUP_SET_L_DIR_Msk    (0x1UL << L_BLOCK_SETUP_SET_L_DIR_Pos)
#define L_BLOCK_SETUP_SET_L_DIR        L_BLOCK_SETUP_SET_L_DIR_Msk

#define L_BLOCK_SETUP_CURR_L_DIR_Pos   17
#define L_BLOCK_SETUP_CURR_L_DIR_Msk   (0x1UL << L_BLOCK_SETUP_CURR_L_DIR_Pos)
#define L_BLOCK_SETUP_CURR_L_DIR       L_BLOCK_SETUP_CURR_L_DIR_Msk

/** @} */ /* End of group MDR32VF0xI_L_BLOCK_SETUP */

/** @} */ /* End of group MDR32VF0xI_L_BLOCK_Defines */

/** @} */ /* End of group MDR32VF0xI_L_BLOCK */

/** @defgroup MDR32VF0xI_S_BLOCK MDR_S_BLOCK
 * @{
 */

/**
 * @brief MDR32VF0xI S_BLOCK control structure.
 */
typedef struct {
    __IO uint32_t TRANSFORM_0;  /*!<S_BLOCK Transformation Register 1 */
    __IO uint32_t TRANSFORM_1;  /*!<S_BLOCK Transformation Register 2 */
    __IO uint32_t TRANSFORM_2;  /*!<S_BLOCK Transformation Register 3 */
    __IO uint32_t TRANSFORM_3;  /*!<S_BLOCK Transformation Register 4 */
    __IO uint32_t TRANSFORM_4;  /*!<S_BLOCK Transformation Register 5 */
    __IO uint32_t TRANSFORM_5;  /*!<S_BLOCK Transformation Register 6 */
    __IO uint32_t TRANSFORM_6;  /*!<S_BLOCK Transformation Register 7 */
    __IO uint32_t TRANSFORM_7;  /*!<S_BLOCK Transformation Register 8 */
    __IO uint32_t TABLE_CHANGE; /*!<S_BLOCK Table Change Register */
} MDR_S_BLOCK_TypeDef;

/** @defgroup MDR32VF0xI_S_BLOCK_Defines S_BLOCK Defines
 * @{
 */

/** @defgroup MDR32VF0xI_S_BLOCK_TRANSFORM_N S_BLOCK TRANSFORM_N
 * @{
 */

#define S_BLOCK_TRANSFORM_N_DATA_Pos 0
#define S_BLOCK_TRANSFORM_N_DATA_Msk (0xFFFFFFFFUL << S_BLOCK_TRANSFORM_N_DATA_Pos)

/** @} */ /* End of group MDR32VF0xI_S_BLOCK_TRANSFORM_N */

/** @defgroup MDR32VF0xI_S_BLOCK_TABLE_CHANGE S_BLOCK TABLE_CHANGE
 * @{
 */

#define S_BLOCK_TABLE_CHANGE_TABLE_VAL_0_Pos 0
#define S_BLOCK_TABLE_CHANGE_TABLE_VAL_0_Msk (0xFFUL << S_BLOCK_TABLE_CHANGE_TABLE_VAL_0_Pos)

#define S_BLOCK_TABLE_CHANGE_TABLE_VAL_1_Pos 8
#define S_BLOCK_TABLE_CHANGE_TABLE_VAL_1_Msk (0xFFUL << S_BLOCK_TABLE_CHANGE_TABLE_VAL_1_Pos)

#define S_BLOCK_TABLE_CHANGE_TABLE_VAL_2_Pos 16
#define S_BLOCK_TABLE_CHANGE_TABLE_VAL_2_Msk (0xFFUL << S_BLOCK_TABLE_CHANGE_TABLE_VAL_2_Pos)

#define S_BLOCK_TABLE_CHANGE_TABLE_VAL_3_Pos 24
#define S_BLOCK_TABLE_CHANGE_TABLE_VAL_3_Msk (0xFFUL << S_BLOCK_TABLE_CHANGE_TABLE_VAL_3_Pos)

/** @} */ /* End of group MDR32VF0xI_S_BLOCK_TABLE_CHANGE */

/** @} */ /* End of group MDR32VF0xI_S_BLOCK_Defines */

/** @} */ /* End of group MDR32VF0xI_S_BLOCK */

/** @defgroup MDR32VF0xI_P_BYTE MDR_P_BYTE
 * @{
 */

/**
 * @brief MDR32VF0xI P_BYTE control structure
 */
typedef struct {
    __IO uint32_t TRANSFORM_0;  /*!<P_BYTE Word Permutation Register 1 */
    __IO uint32_t TRANSFORM_1;  /*!<P_BYTE Word Permutation Register 2 */
    __IO uint32_t TRANSFORM_2;  /*!<P_BYTE Word Permutation Register 3 */
    __IO uint32_t TRANSFORM_3;  /*!<P_BYTE Word Permutation Register 4 */
    __IO uint32_t TRANSFORM_4;  /*!<P_BYTE Word Permutation Register 5 */
    __IO uint32_t TRANSFORM_5;  /*!<P_BYTE Word Permutation Register 6 */
    __IO uint32_t TRANSFORM_6;  /*!<P_BYTE Word Permutation Register 7 */
    __IO uint32_t TRANSFORM_7;  /*!<P_BYTE Word Permutation Register 8 */
    __IO uint32_t TRANSFORM_8;  /*!<P_BYTE Word Permutation Register 9 */
    __IO uint32_t TRANSFORM_9;  /*!<P_BYTE Word Permutation Register 10 */
    __IO uint32_t TRANSFORM_10; /*!<P_BYTE Word Permutation Register 11 */
    __IO uint32_t TRANSFORM_11; /*!<P_BYTE Word Permutation Register 12 */
    __IO uint32_t TRANSFORM_12; /*!<P_BYTE Word Permutation Register 13 */
    __IO uint32_t TRANSFORM_13; /*!<P_BYTE Word Permutation Register 14 */
    __IO uint32_t TRANSFORM_14; /*!<P_BYTE Word Permutation Register 15 */
    __IO uint32_t TRANSFORM_15; /*!<P_BYTE Word Permutation Register 16 */
    __IO uint32_t MUX_SET;      /*!<P_BYTE Permutation Setting Register */
    __IO uint32_t MUX_VAL;      /*!<P_BYTE Permutation Value Register */
} MDR_P_BYTE_TypeDef;

/** @defgroup MDR32VF0xI_P_BYTE_Defines P_BYTE Defines
 * @{
 */

/** @defgroup MDR32VF0xI_P_BYTE_TRANSFORM_N P_BYTE TRANSFORM_N
 * @{
 */

#define P_BYTE_TRANSFORM_N_DATA_Pos 0
#define P_BYTE_TRANSFORM_N_DATA_Msk (0xFFFFFFFFUL << P_BYTE_TRANSFORM_N_DATA_Pos)

/** @} */ /* End of group MDR32VF0xI_P_BYTE_TRANSFORM_N */

/** @defgroup MDR32VF0xI_P_BYTE_MUX_SET P_BYTE MUX_SET
 * @{
 */

#define P_BYTE_MUX_SET_IN_SEL_Pos  0
#define P_BYTE_MUX_SET_IN_SEL_Msk  (0x3FUL << P_BYTE_MUX_SET_IN_SEL_Pos)

#define P_BYTE_MUX_SET_OUT_SEL_Pos 8
#define P_BYTE_MUX_SET_OUT_SEL_Msk (0x3FUL << P_BYTE_MUX_SET_OUT_SEL_Pos)

/** @} */ /* End of group MDR32VF0xI_P_BYTE_MUX_SET */

/** @defgroup MDR32VF0xI_P_BYTE_MUX_VAL P_BYTE MUX_VAL
 * @{
 */

#define P_BYTE_MUX_VAL_VAL_Pos 0
#define P_BYTE_MUX_VAL_VAL_Msk (0x3FUL << P_BYTE_MUX_VAL_VAL_Pos)

#define P_BYTE_MUX_VAL_SEL_Pos 8
#define P_BYTE_MUX_VAL_SEL_Msk (0x3FUL << P_BYTE_MUX_VAL_SEL_Pos)

/** @} */ /* End of group MDR32VF0xI_P_BYTE_MUX_VAL */

/** @} */ /* End of group MDR32VF0xI_P_BYTE_Defines */

/** @} */ /* End of group MDR32VF0xI_P_BYTE */

/** @defgroup MDR32VF0xI_P_BIT MDR_P_BIT
 * @{
 */

/**
 * @brief MDR32VF0xI P_BIT control structure
 */
typedef struct {
    __IO uint32_t TRANSFORM_0; /*!<P_BIT Word Permutation Register 1 */
    __IO uint32_t TRANSFORM_1; /*!<P_BIT Word Permutation Register 2 */
    __IO uint32_t TRANSFORM_2; /*!<P_BIT Word Permutation Register 3 */
    __IO uint32_t TRANSFORM_3; /*!<P_BIT Word Permutation Register 4 */
    __IO uint32_t MUX_SET;     /*!<P_BIT Permutation Setting Register */
    __IO uint32_t MUX_VAL;     /*!<P_BIT Permutation Value Register */
} MDR_P_BIT_TypeDef;

/** @defgroup MDR32VF0xI_P_BIT_Defines P_BIT Defines
 * @{
 */

/** @defgroup MDR32VF0xI_P_BIT_TRANSFORM_N P_BIT TRANSFORM_N
 * @{
 */

#define P_BIT_TRANSFORM_N_DATA_Pos 0
#define P_BIT_TRANSFORM_N_DATA_Msk (0xFFFFFFFFUL << P_BIT_TRANSFORM_N_DATA_Pos)

/** @} */ /* End of group MDR32VF0xI_P_BIT_TRANSFORM_N */

/** @defgroup MDR32VF0xI_P_BIT_MUX_SET P_BIT MUX_SET
 * @{
 */

#define P_BIT_MUX_SET_IN_SEL_Pos  0
#define P_BIT_MUX_SET_IN_SEL_Msk  (0x7FUL << P_BIT_MUX_SET_IN_SEL_Pos)

#define P_BIT_MUX_SET_OUT_SEL_Pos 8
#define P_BIT_MUX_SET_OUT_SEL_Msk (0x7FUL << P_BIT_MUX_SET_OUT_SEL_Pos)

/** @} */ /* End of group MDR32VF0xI_P_BIT_MUX_SET */

/** @defgroup MDR32VF0xI_P_BIT_MUX_VAL P_BIT MUX_VAL
 * @{
 */

#define P_BIT_MUX_VAL_VAL_Pos 0
#define P_BIT_MUX_VAL_VAL_Msk (0x7FUL << P_BIT_MUX_VAL_VAL_Pos)

#define P_BIT_MUX_VAL_SEL_Pos 8
#define P_BIT_MUX_VAL_SEL_Msk (0x7FUL << P_BIT_MUX_VAL_SEL_Pos)

/** @} */ /* End of group MDR32VF0xI_P_BIT_MUX_VAL */

/** @} */ /* End of group MDR32VF0xI_P_BIT_Defines */

/** @} */ /* End of group MDR32VF0xI_P_BIT */

/** @defgroup MDR32VF0xI_OTP MDR_OTP
 * @{
 */

/**
 * @brief MDR32VF0xI OTP control structure
 */
typedef struct {
    __IO uint32_t STAT_CTRL;     /*!<OTP Status and Control Register */
    __I uint32_t  STAT;          /*!<OTP Status Register */
    __IO uint32_t DELAY_0;       /*!<OTP Delay Register 0 */
    __IO uint32_t DELAY_1;       /*!<OTP Delay Register 1 */
    __IO uint32_t RW_CMD;        /*!<OTP Read-Write Command Register */
    __I uint32_t  READ_DATA;     /*!<OTP Read Data Register */
    __IO uint32_t WRITE_PROTECT; /*!<OTP Write Protection Register */
    __IO uint32_t READ_PROTECT;  /*!<OTP Read Protection Register */
    __IO uint32_t RESERVED0[2];  /*!<Reserved */
    __I uint32_t  TEST_OPT;      /*!<OTP Test Options Register */
} MDR_OTP_TypeDef;

/** @defgroup MDR32VF0xI_OTP_Defines OTP Defines
 * @{
 */

/** @defgroup MDR32VF0xI_OTP_STAT_CTRL OTP STAT_CTRL
 * @{
 */

#define OTP_STAT_CTRL_FSM_STATE_Pos           0
#define OTP_STAT_CTRL_FSM_STATE_Msk           (0x3UL << OTP_STAT_CTRL_FSM_STATE_Pos)
#define OTP_STAT_CTRL_FSM_STATE_IDLE          (0x0UL << OTP_STAT_CTRL_FSM_STATE_Pos)
#define OTP_STAT_CTRL_FSM_STATE_INIT          (0x1UL << OTP_STAT_CTRL_FSM_STATE_Pos)
#define OTP_STAT_CTRL_FSM_STATE_READY         (0x2UL << OTP_STAT_CTRL_FSM_STATE_Pos)
#define OTP_STAT_CTRL_FSM_STATE_WAIT_REG_READ (0x3UL << OTP_STAT_CTRL_FSM_STATE_Pos)
#define OTP_STAT_CTRL_FSM_STATE_WAIT_BUS_READ (0x4UL << OTP_STAT_CTRL_FSM_STATE_Pos)
#define OTP_STAT_CTRL_FSM_STATE_WAIT_WRITE    (0x5UL << OTP_STAT_CTRL_FSM_STATE_Pos)
#define OTP_STAT_CTRL_FSM_STATE_DIRECT        (0x6UL << OTP_STAT_CTRL_FSM_STATE_Pos)

#define OTP_STAT_CTRL_BUSY_Pos                3
#define OTP_STAT_CTRL_BUSY_Msk                (0x1UL << OTP_STAT_CTRL_BUSY_Pos)
#define OTP_STAT_CTRL_BUSY                    OTP_STAT_CTRL_BUSY_Msk

#define OTP_STAT_CTRL_OTP_LOCK_Pos            4
#define OTP_STAT_CTRL_OTP_LOCK_Msk            (0x1UL << OTP_STAT_CTRL_OTP_LOCK_Pos)
#define OTP_STAT_CTRL_OTP_LOCK                OTP_STAT_CTRL_OTP_LOCK_Msk

#define OTP_STAT_CTRL_OTP_PLOCK_Pos           5
#define OTP_STAT_CTRL_OTP_PLOCK_Msk           (0x1UL << OTP_STAT_CTRL_OTP_PLOCK_Pos)
#define OTP_STAT_CTRL_OTP_PLOCK               OTP_STAT_CTRL_OTP_PLOCK_Msk

#define OTP_STAT_CTRL_RW_ERROR_Pos            6
#define OTP_STAT_CTRL_RW_ERROR_Msk            (0x1UL << OTP_STAT_CTRL_RW_ERROR_Pos)
#define OTP_STAT_CTRL_RW_ERROR                OTP_STAT_CTRL_RW_ERROR_Msk

#define OTP_STAT_CTRL_TEST_CRC_Pos            7
#define OTP_STAT_CTRL_TEST_CRC_Msk            (0x1UL << OTP_STAT_CTRL_TEST_CRC_Pos)
#define OTP_STAT_CTRL_TEST_CRC                OTP_STAT_CTRL_TEST_CRC_Msk

#define OTP_STAT_CTRL_WRITE_PCRC_Pos          8
#define OTP_STAT_CTRL_WRITE_PCRC_Msk          (0x1UL << OTP_STAT_CTRL_WRITE_PCRC_Pos)
#define OTP_STAT_CTRL_WRITE_PCRC              OTP_STAT_CTRL_WRITE_PCRC_Msk

#define OTP_STAT_CTRL_READ_PCRC_Pos           9
#define OTP_STAT_CTRL_READ_PCRC_Msk           (0x1UL << OTP_STAT_CTRL_READ_PCRC_Pos)
#define OTP_STAT_CTRL_READ_PCRC               OTP_STAT_CTRL_READ_PCRC_Msk

#define OTP_STAT_CTRL_DIR_EN_Pos              10
#define OTP_STAT_CTRL_DIR_EN_Msk              (0x1UL << OTP_STAT_CTRL_DIR_EN_Pos)
#define OTP_STAT_CTRL_DIR_EN                  OTP_STAT_CTRL_DIR_EN_Msk

/** @} */ /* End of group MDR32VF0xI_OTP_STAT_CTRL */

/** @defgroup MDR32VF0xI_OTP_STAT OTP STAT
 * @{
 */

#define OTP_STAT_OTP_FSM_STATE_Pos 0
#define OTP_STAT_OTP_FSM_STATE_Msk (0x1FUL << OTP_STAT_OTP_FSM_STATE_Pos)

#define OTP_STAT_OTP_LOCK_Pos      6
#define OTP_STAT_OTP_LOCK_Msk      (0x1UL << OTP_STAT_OTP_LOCK_Pos)
#define OTP_STAT_OTP_LOCK          OTP_STAT_OTP_LOCK_Msk

#define OTP_STAT_OTP_BUSY_Pos      7
#define OTP_STAT_OTP_BUSY_Msk      (0x1UL << OTP_STAT_OTP_BUSY_Pos)
#define OTP_STAT_OTP_BUSY          OTP_STAT_OTP_BUSY_Msk

/** @} */ /* End of group MDR32VF0xI_OTP_STAT */

/** @defgroup MDR32VF0xI_OTP_DELAY_0 OTP DELAY_0
 * @{
 */

#define OTP_DELAY_0_DELAY_20NS_Pos 0
#define OTP_DELAY_0_DELAY_20NS_Msk (0xFUL << OTP_DELAY_0_DELAY_20NS_Pos)

#define OTP_DELAY_0_DELAY_70NS_Pos 4
#define OTP_DELAY_0_DELAY_70NS_Msk (0xFUL << OTP_DELAY_0_DELAY_70NS_Pos)

#define OTP_DELAY_0_DELAY_50NS_Pos 8
#define OTP_DELAY_0_DELAY_50NS_Msk (0x1FUL << OTP_DELAY_0_DELAY_50NS_Pos)

#define OTP_DELAY_0_DELAY_1US_Pos  16
#define OTP_DELAY_0_DELAY_1US_Msk  (0x1FFUL << OTP_DELAY_0_DELAY_1US_Pos)

/** @} */ /* End of group MDR32VF0xI_OTP_DELAY_0 */

/** @defgroup MDR32VF0xI_OTP_DELAY_1 OTP DELAY_1
 * @{
 */

#define OTP_DELAY_1_DELAY_16US_Pos 0
#define OTP_DELAY_1_DELAY_16US_Msk (0x1FFFFUL << OTP_DELAY_1_DELAY_16US_Pos)

/** @} */ /* End of group MDR32VF0xI_OTP_DELAY_1 */

/** @defgroup MDR32VF0xI_OTP_RW_CMD OTP RW_CMD
 * @{
 */

#define OTP_RW_CMD_ADDR_Pos     0
#define OTP_RW_CMD_ADDR_Msk     (0x1FFFFUL << OTP_RW_CMD_ADDR_Pos)

#define OTP_RW_CMD_DATA_Pos     24
#define OTP_RW_CMD_DATA_Msk     (0x1UL << OTP_RW_CMD_DATA_Pos)
#define OTP_RW_CMD_DATA         OTP_RW_CMD_DATA_Msk

#define OTP_RW_CMD_READ_Pos     28
#define OTP_RW_CMD_READ_Msk     (0x1UL << OTP_RW_CMD_READ_Pos)
#define OTP_RW_CMD_READ         OTP_RW_CMD_READ_Msk

#define OTP_RW_CMD_WRITE_Pos    29
#define OTP_RW_CMD_WRITE_Msk    (0x1UL << OTP_RW_CMD_WRITE_Pos)
#define OTP_RW_CMD_WRITE        OTP_RW_CMD_WRITE_Msk

#define OTP_RW_CMD_RW_ERROR_Pos 31
#define OTP_RW_CMD_RW_ERROR_Msk (0x1UL << OTP_RW_CMD_RW_ERROR_Pos)
#define OTP_RW_CMD_RW_ERROR     OTP_RW_CMD_RW_ERROR_Msk

/** @} */ /* End of group MDR32VF0xI_OTP_RW_CMD */

/** @defgroup MDR32VF0xI_OTP_READ_DATA OTP READ_DATA
 * @{
 */

#define OTP_READ_DATA_DATA_Pos 0
#define OTP_READ_DATA_DATA_Msk (0xFFUL << OTP_READ_DATA_DATA_Pos)

/** @} */ /* End of group MDR32VF0xI_OTP_READ_DATA */

/** @defgroup MDR32VF0xI_OTP_WRITE_PROTECT OTP WRITE_PROTECT
 * @{
 */

#define OTP_WRITE_PROTECT_Pos 0
#define OTP_WRITE_PROTECT_Msk (0xFFUL << OTP_WRITE_PROTECT_Pos)
#define OTP_WRITE_PROTECT_0   (0x01UL << OTP_WRITE_PROTECT_Pos)
#define OTP_WRITE_PROTECT_1   (0x02UL << OTP_WRITE_PROTECT_Pos)
#define OTP_WRITE_PROTECT_2   (0x04UL << OTP_WRITE_PROTECT_Pos)
#define OTP_WRITE_PROTECT_3   (0x08UL << OTP_WRITE_PROTECT_Pos)
#define OTP_WRITE_PROTECT_4   (0x10UL << OTP_WRITE_PROTECT_Pos)
#define OTP_WRITE_PROTECT_5   (0x20UL << OTP_WRITE_PROTECT_Pos)
#define OTP_WRITE_PROTECT_6   (0x40UL << OTP_WRITE_PROTECT_Pos)
#define OTP_WRITE_PROTECT_7   (0x80UL << OTP_WRITE_PROTECT_Pos)

/** @} */ /* End of group MDR32VF0xI_OTP_WRITE_PROTECT */

/** @defgroup MDR32VF0xI_OTP_READ_PROTECT OTP READ_PROTECT
 * @{
 */

#define OTP_READ_PROTECT_Pos 0
#define OTP_READ_PROTECT_Msk (0xFFUL << OTP_READ_PROTECT_Pos)
#define OTP_READ_PROTECT_0   (0x01UL << OTP_READ_PROTECT_Pos)
#define OTP_READ_PROTECT_1   (0x02UL << OTP_READ_PROTECT_Pos)
#define OTP_READ_PROTECT_2   (0x04UL << OTP_READ_PROTECT_Pos)
#define OTP_READ_PROTECT_3   (0x08UL << OTP_READ_PROTECT_Pos)
#define OTP_READ_PROTECT_4   (0x10UL << OTP_READ_PROTECT_Pos)
#define OTP_READ_PROTECT_5   (0x20UL << OTP_READ_PROTECT_Pos)
#define OTP_READ_PROTECT_6   (0x40UL << OTP_READ_PROTECT_Pos)
#define OTP_READ_PROTECT_7   (0x80UL << OTP_READ_PROTECT_Pos)

/** @} */ /* End of group MDR32VF0xI_OTP_READ_PROTECT */

/** @defgroup MDR32VF0xI_OTP_TEST_OPT OTP TEST_OPT
 * @{
 */

#define OTP_TEST_OPT_DEBUG_Pos 0
#define OTP_TEST_OPT_DEBUG_Msk (0xFFUL << OTP_TEST_OPT_DEBUG_Pos)

/** @} */ /* End of group MDR32VF0xI_OTP_TEST_OPT */

/** @} */ /* End of group MDR32VF0xI_OTP_Defines */

/** @} */ /* End of group MDR32VF0xI_OTP */

/** @defgroup MDR32F02_OTP_SPECIAL_FIELDS OTP_SPECIAL_FIELDS
 * @{
 */

/**
 * @brief MDR32F02 OTP special fields structure.
 */
typedef struct {
    __I uint32_t  UNIQ_ID[5]; /*!<OTP_SPECIAL_FIELDS Unique Chip Identifier Fields */
    __I uint32_t  BG_TRIM;    /*!<OTP_SPECIAL_FIELDS BandGap Reference Voltage Trimming Field (unused) */
    __I uint32_t  HSI_TRIM;   /*!<OTP_SPECIAL_FIELDS HSI and LSI Trimming Field */
    __IO uint32_t PROTECTION; /*!<OTP_SPECIAL_FIELDS OTP Protection Word Field */
} OTP_SpecialFields_TypeDef;

/** @defgroup MDR32F02_OTP_SPECIAL_FIELDS_Defines MDR32F02_OTP_SPECIAL_FIELDS Defines
 * @{
 */

/** @defgroup MDR32F02_OTP_SPECIAL_FIELDS_UNIQUE_ID MDR32F02_OTP_SPECIAL_FIELDS UNIQUE_ID
 * @{
 */

#define OTP_SPECIAL_FIELDS_UNIQUE_IDx_UNIQUE_ID_Pos 0
#define OTP_SPECIAL_FIELDS_UNIQUE_IDx_UNIQUE_ID_Msk (0xFFFFFFFFUL << OTP_SPECIAL_FIELDS_UNIQUE_IDx_UNIQUE_ID_Pos)

/** @} */ /* End of group MDR32F02_OTP_SPECIAL_FIELDS_UNIQUE_ID */

/** @defgroup MDR32F02_OTP_SPECIAL_FIELDS_TRIM_HSI MDR32F02_OTP_SPECIAL_FIELDS TRIM_HSI
 * @{
 */

#define OTP_SPECIAL_FIELDS_TRIM_HSI_TRIM_HSI_Pos 0
#define OTP_SPECIAL_FIELDS_TRIM_HSI_TRIM_HSI_Msk (0x3FUL << OTP_SPECIAL_FIELDS_TRIM_HSI_TRIM_HSI_Pos)

/** @} */ /* End of group MDR32F02_OTP_SPECIAL_FIELDS_TRIM_HSI_TRIM_HSI_Pos */

/** @defgroup MDR32F02_OTP_SPECIAL_FIELDS_PROTECTION MDR32F02_OTP_SPECIAL_FIELDS PROTECTION
 * @{
 */

#define OTP_SPECIAL_FIELDS_PROTECTION_REGIONS_Pos 28
#define OTP_SPECIAL_FIELDS_PROTECTION_REGIONS_Msk (0xFUL << OTP_SPECIAL_FIELDS_PROTECTION_REGIONS_Pos)

/** @} */ /* End of group MDR32F02_OTP_SPECIAL_FIELDS_PROTECTION */

/** @} */ /* End of group MDR32F02_OTP_SPECIAL_FIELDS_Defines */

/** @} */ /* End of group MDR32F02_OTP_SPECIAL_FIELDS */

#endif

/** @defgroup MDR32VF0xI_Peripheral_Declaration MDR32VF0xI Peripheral Declaration
 * @{
 */

#define MDR_SSP1    ((MDR_SSP_TypeDef*)MDR_SSP1_BASE)
#define MDR_SSP2    ((MDR_SSP_TypeDef*)MDR_SSP2_BASE)
#define MDR_SSP3    ((MDR_SSP_TypeDef*)MDR_SSP3_BASE)
#define MDR_UART1   ((MDR_UART_TypeDef*)MDR_UART1_BASE)
#define MDR_UART2   ((MDR_UART_TypeDef*)MDR_UART2_BASE)
#define MDR_UART3   ((MDR_UART_TypeDef*)MDR_UART3_BASE)
#define MDR_UART4   ((MDR_UART_TypeDef*)MDR_UART4_BASE)
#define MDR_FLASH   ((MDR_FLASH_TypeDef*)MDR_FLASH_BASE)
#define MDR_RST_CLK ((MDR_RST_CLK_TypeDef*)MDR_RST_CLK_BASE)
#define MDR_DMA     ((MDR_DMA_TypeDef*)MDR_DMA_BASE)
#define MDR_I2C     ((MDR_I2C_TypeDef*)MDR_I2C_BASE)
#define MDR_ADC     ((MDR_ADC_TypeDef*)MDR_ADC_BASE)
#define MDR_WWDG    ((MDR_WWDG_TypeDef*)MDR_WWDG_BASE)
#define MDR_IWDG    ((MDR_IWDG_TypeDef*)MDR_IWDG_BASE)
#define MDR_POWER   ((MDR_POWER_TypeDef*)MDR_POWER_BASE)
#define MDR_BKP     ((MDR_BKP_TypeDef*)MDR_BKP_BASE)
#define MDR_ADCUI   ((MDR_ADCUI_TypeDef*)MDR_ADCUI_BASE)
#define MDR_TIMER1  ((MDR_TIMER_TypeDef*)MDR_TIMER1_BASE)
#define MDR_TIMER2  ((MDR_TIMER_TypeDef*)MDR_TIMER2_BASE)
#define MDR_TIMER3  ((MDR_TIMER_TypeDef*)MDR_TIMER3_BASE)
#define MDR_TIMER4  ((MDR_TIMER_TypeDef*)MDR_TIMER4_BASE)
#define MDR_PORTA   ((MDR_PORT_TypeDef*)MDR_PORTA_BASE)
#define MDR_PORTB   ((MDR_PORT_TypeDef*)MDR_PORTB_BASE)
#define MDR_PORTC   ((MDR_PORT_TypeDef*)MDR_PORTC_BASE)
#define MDR_PORTD   ((MDR_PORT_TypeDef*)MDR_PORTD_BASE)
#define MDR_CRC     ((MDR_CRC_TypeDef*)MDR_CRC_BASE)
#define MDR_RANDOM  ((MDR_RANDOM_TypeDef*)MDR_RANDOM_BASE)
#define MDR_USART   ((MDR_USART_TypeDef*)MDR_USART_BASE)
#if defined(USE_MDR32F02)
#define MDR_L_BLOCK     ((MDR_L_BLOCK_TypeDef*)MDR_L_BLOCK_BASE)
#define MDR_S_BLOCK0    ((MDR_S_BLOCK_TypeDef*)MDR_S_BLOCK0_BASE)
#define MDR_S_BLOCK1    ((MDR_S_BLOCK_TypeDef*)MDR_S_BLOCK1_BASE)
#define MDR_S_BLOCK2    ((MDR_S_BLOCK_TypeDef*)MDR_S_BLOCK2_BASE)
#define MDR_S_BLOCK3    ((MDR_S_BLOCK_TypeDef*)MDR_S_BLOCK3_BASE)
#define MDR_S_BLOCK4    ((MDR_S_BLOCK_TypeDef*)MDR_S_BLOCK4_BASE)
#define MDR_S_BLOCK5    ((MDR_S_BLOCK_TypeDef*)MDR_S_BLOCK5_BASE)
#define MDR_S_BLOCK6    ((MDR_S_BLOCK_TypeDef*)MDR_S_BLOCK6_BASE)
#define MDR_S_BLOCK7    ((MDR_S_BLOCK_TypeDef*)MDR_S_BLOCK7_BASE)
#define MDR_P_BYTE      ((MDR_P_BYTE_TypeDef*)MDR_P_BYTE_BASE)
#define MDR_P_BIT0      ((MDR_P_BIT_TypeDef*)MDR_P_BIT0_BASE)
#define MDR_P_BIT1      ((MDR_P_BIT_TypeDef*)MDR_P_BIT1_BASE)
#define MDR_P_BIT2      ((MDR_P_BIT_TypeDef*)MDR_P_BIT2_BASE)
#define MDR_P_BIT3      ((MDR_P_BIT_TypeDef*)MDR_P_BIT3_BASE)
#define MDR_SENSORS     ((MDR_SENSORS_TypeDef*)MDR_SENSORS_BASE)
#define MDR_CLK_MEASURE ((MDR_CLK_MEASURE_TypeDef*)MDR_CLK_MEASURE_BASE)
#define MDR_OTP         ((MDR_OTP_TypeDef*)MDR_OTP_REG_BASE)
#endif

#if defined(USE_MDR32F02)
#define OTP_SPECIAL_FIELDS ((OTP_SpecialFields_TypeDef*)(OTP_BASE + 0x3FE0))
#endif

#if defined(USE_MDR1206FI)
#define MDR_FLASH_SPECIAL_FIELDS ((MDR_FLASH_SpecialFields_TypeDef*)MDR_FLASH_SPECIAL_FIELDS_BASE)
#elif defined(USE_MDR1206AFI)
#define MDR_FLASH_SPECIAL_FIELDS ((MDR_FLASH_SpecialFields_TypeDef*)MDR_FLASH_SPECIAL_FIELDS_BASE)
#elif defined(USE_MDR1206)
#define MDR_FLASH_SPECIAL_FIELDS_MDR1206FI  ((MDR_FLASH_SpecialFields_TypeDef*)MDR_FLASH_SPECIAL_FIELDS_MDR1206FI_BASE)
#define MDR_FLASH_SPECIAL_FIELDS_MDR1206AFI ((MDR_FLASH_SpecialFields_TypeDef*)MDR_FLASH_SPECIAL_FIELDS_MDR1206AFI_BASE)
#endif

/** @} */ /* End of group MDR32VF0xI_Peripheral_Declaration */



/** @} */ /* End of group MDR32VF0xI_DEVICE */

/** @} */ /* End of group DEVICE_SUPPORT */

#endif /* MDR32VF0xI_H */

/*********************** (C) COPYRIGHT 2025 Milandr ****************************
 *
 * END OF FILE MDR32VF0xI.h */
