/**
 *******************************************************************************
 * @file    MDR32VF0xI_port.h
 * @author  Milandr Application Team
 * @version V0.2.0
 * @date    31/03/2025
 * @brief   This file contains all the functions prototypes for the PORT
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
#ifndef MDR32VF0xI_PORT_H
#define MDR32VF0xI_PORT_H


/* Includes ------------------------------------------------------------------*/

#include "mdr1206fi.h"

/** @addtogroup MDR32VF0xI_StdPeriph_Driver MDR32VF0xI Standard Peripheral Driver
 * @{
 */

/** @addtogroup PORT
 * @{
 */

/** @defgroup PORT_Exported_Types PORT Exported Types
 * @{
 */

/**
 * @brief PORT pins enumeration.
 */
typedef enum {
    PORT_PIN_0   = PORT_RXTX_RXTX0,   /*!< Pin 0 is selected. */
    PORT_PIN_1   = PORT_RXTX_RXTX1,   /*!< Pin 1 is selected. */
    PORT_PIN_2   = PORT_RXTX_RXTX2,   /*!< Pin 2 is selected. */
    PORT_PIN_3   = PORT_RXTX_RXTX3,   /*!< Pin 3 is selected. */
    PORT_PIN_4   = PORT_RXTX_RXTX4,   /*!< Pin 4 is selected. */
    PORT_PIN_5   = PORT_RXTX_RXTX5,   /*!< Pin 5 is selected. */
    PORT_PIN_6   = PORT_RXTX_RXTX6,   /*!< Pin 6 is selected. */
    PORT_PIN_7   = PORT_RXTX_RXTX7,   /*!< Pin 7 is selected. */
    PORT_PIN_8   = PORT_RXTX_RXTX8,   /*!< Pin 8 is selected. */
    PORT_PIN_9   = PORT_RXTX_RXTX9,   /*!< Pin 9 is selected. */
    PORT_PIN_10  = PORT_RXTX_RXTX10,  /*!< Pin 10 is selected. */
    PORT_PIN_11  = PORT_RXTX_RXTX11,  /*!< Pin 11 is selected. */
    PORT_PIN_12  = PORT_RXTX_RXTX12,  /*!< Pin 12 is selected. */
    PORT_PIN_13  = PORT_RXTX_RXTX13,  /*!< Pin 13 is selected. */
    PORT_PIN_14  = PORT_RXTX_RXTX14,  /*!< Pin 14 is selected. */
    PORT_PIN_15  = PORT_RXTX_RXTX15,  /*!< Pin 15 is selected. */
    PORT_PIN_ALL = PORT_RXTX_RXTX_Msk /*!< All pins are selected. */
} PORT_Pin_TypeDef;

#define IS_PORT_PIN(PIN) ((((PIN) & ~PORT_RXTX_RXTX_Msk) == 0x00) && \
                          ((PIN) != 0x00))

#define IS_GET_PORT_PIN(PIN) (((PIN) == PORT_PIN_0) ||  \
                              ((PIN) == PORT_PIN_1) ||  \
                              ((PIN) == PORT_PIN_2) ||  \
                              ((PIN) == PORT_PIN_3) ||  \
                              ((PIN) == PORT_PIN_4) ||  \
                              ((PIN) == PORT_PIN_5) ||  \
                              ((PIN) == PORT_PIN_6) ||  \
                              ((PIN) == PORT_PIN_7) ||  \
                              ((PIN) == PORT_PIN_8) ||  \
                              ((PIN) == PORT_PIN_9) ||  \
                              ((PIN) == PORT_PIN_10) || \
                              ((PIN) == PORT_PIN_11) || \
                              ((PIN) == PORT_PIN_12) || \
                              ((PIN) == PORT_PIN_13) || \
                              ((PIN) == PORT_PIN_14) || \
                              ((PIN) == PORT_PIN_15))

/**
 * @brief PORT pin analog-digital mode enumeration.
 */
typedef enum {
    PORT_MODE_ANALOG  = ((uint32_t)0x0),
    PORT_MODE_DIGITAL = ((uint32_t)0x1)
} PORT_Mode_TypeDef;

#define IS_PORT_MODE(MODE) (((MODE) == PORT_MODE_ANALOG) || \
                            ((MODE) == PORT_MODE_DIGITAL))

/**
 * @brief PORT pin direction (OE) enumeration.
 */
typedef enum {
    PORT_DIRECTION_INPUT        = ((uint32_t)0x0), /*!< Port TX disable, RX enable. */
    PORT_DIRECTION_OUTPUT       = ((uint32_t)0x1), /*!< Port TX enable, RX disable. */
    PORT_DIRECTION_INPUT_OUTPUT = ((uint32_t)0x2)  /*!< Port TX enable, RX enable. */
} PORT_Direction_TypeDef;

#define IS_PORT_DIR(DIR) (((DIR) == PORT_DIRECTION_INPUT) ||  \
                          ((DIR) == PORT_DIRECTION_OUTPUT) || \
                          ((DIR) == PORT_DIRECTION_INPUT_OUTPUT))

/**
 * @brief PORT pin function enumeration.
 */
typedef enum {
    PORT_FUNCTION_PORT        = PORT_FUNC_MODE0_PORT,
    PORT_FUNCTION_MAIN        = PORT_FUNC_MODE0_MAIN,
    PORT_FUNCTION_ALTERNATIVE = PORT_FUNC_MODE0_ALTERNATIVE,
    PORT_FUNCTION_OVERRIDDEN  = PORT_FUNC_MODE0_OVERRIDDEN
} PORT_Function_TypeDef;

#define IS_PORT_FUNC(FUNC) (((FUNC) == PORT_FUNCTION_PORT) ||        \
                            ((FUNC) == PORT_FUNCTION_MAIN) ||        \
                            ((FUNC) == PORT_FUNCTION_ALTERNATIVE) || \
                            ((FUNC) == PORT_FUNCTION_OVERRIDDEN))

/**
 * @brief PORT pin power (PWR) enumeration.
 */
typedef enum {
#if defined(USE_MDR1206AFI)
    PORT_POWER_INCREASED_UPTO_4mA = ((uint32_t)0x0),
    PORT_POWER_NOMINAL_UPTO_2mA   = ((uint32_t)0x1)
#else
    PORT_POWER_NOMINAL_UPTO_2mA   = ((uint32_t)0x0),
    PORT_POWER_INCREASED_UPTO_4mA = ((uint32_t)0x1)
#endif
} PORT_Power_TypeDef;

#define IS_PORT_POWER(PWR) (((PWR) == PORT_POWER_NOMINAL_UPTO_2mA) || \
                            ((PWR) == PORT_POWER_INCREASED_UPTO_4mA))

#if defined(USE_MDR32F02_REV_2) || defined(USE_MDR1206FI) || defined(USE_MDR1206AFI) || defined(USE_MDR1206)
/**
 * @brief PORT pin pull-up resister usage enumeration.
 */
typedef enum {
    PORT_PULL_UP_OFF = ((uint32_t)0x0),
    PORT_PULL_UP_ON  = ((uint32_t)0x1)
} PORT_PullUp_TypeDef;

#define IS_PORT_PULL_UP(PULL_UP) (((PULL_UP) == PORT_PULL_UP_OFF) || \
                                  ((PULL_UP) == PORT_PULL_UP_ON))
#endif

/**
 * @brief PORT pin pull-down resister usage enumeration.
 */
typedef enum {
    PORT_PULL_DOWN_OFF = ((uint32_t)0x0),
    PORT_PULL_DOWN_ON  = ((uint32_t)0x1)
} PORT_PullDown_TypeDef;

#define IS_PORT_PULL_DOWN(PULL_DOWN) (((PULL_DOWN) == PORT_PULL_DOWN_ON) || \
                                      ((PULL_DOWN) == PORT_PULL_DOWN_OFF))

/**
 * @brief PORT init structure definition.
 */
typedef struct {
    uint16_t PORT_Pin;                     /*!< Specify PORT pins to be configured.
                                                This parameter can be any combination of the @ref PORT_Pin_TypeDef values. */
    PORT_Mode_TypeDef PORT_Mode;           /*!< Specify the operation mode for the selected pins.
                                                This parameter can be a value of the @ref PORT_Mode_TypeDef. */
    PORT_Direction_TypeDef PORT_Direction; /*!< Specify in/out mode for the selected pins.
                                                This parameter can be a value of the @ref PORT_Direction_TypeDef. */
    PORT_Function_TypeDef PORT_Function;   /*!< Specify operation function for the selected pins.
                                                This parameter can be a value of the @ref PORT_Function_TypeDef. */
    PORT_Power_TypeDef PORT_Power;         /*!< Specify power for the selected pins.
                                                This parameter can be a value of the @ref PORT_Power_TypeDef. */
#if defined(USE_MDR32F02_REV_2) || defined(USE_MDR1206FI) || defined(USE_MDR1206AFI) || defined(USE_MDR1206)
    PORT_PullUp_TypeDef PORT_PullUp; /*!< Specify pull-up state for the selected pins.
                                          This parameter can be a value of the @ref PORT_PullUp_TypeDef. */
#endif
    PORT_PullDown_TypeDef PORT_PullDown; /*!< Specify pull-down state for the selected pins.
                                              This parameter can be a value of the @ref PORT_PullDown_TypeDef. */
} PORT_InitTypeDef;




#define JTAG_PINS(PORT)  0U
#define JTAG_PINS1(PORT) 0U
#define JTAG_PINS2(PORT) 0U
#define JTAG_PINS3(PORT) 0U


#define IS_NOT_JTAG_PIN(PORT, PIN) (((PIN) & JTAG_PINS(PORT)) == 0x00)


void PORT_Init(MDR_PORT_TypeDef* PORTx, const PORT_InitTypeDef* PORT_InitStruct);

#endif /* MDR32VF0xI_PORT_H */