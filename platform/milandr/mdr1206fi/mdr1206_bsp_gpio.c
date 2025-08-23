/**
 *******************************************************************************
 * @file    MDR32VF0xI_port.c
 * @author  Milandr Application Team
 * @version V0.2.0
 * @date    03/04/2025
 * @brief   This file contains all the PORT firmware functions.
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

/* Includes ------------------------------------------------------------------*/
#include "mdr_gpio.h"

#include "mdr_uart.h"
#include "rst_clk.h"

/** @addtogroup MDR32VF0xI_StdPeriph_Driver MDR32VF0xI Standard Peripheral Driver
 * @{
 */

/** @defgroup PORT PORT
 * @{
 */

/** @defgroup PORT_Private_Defines PORT Private Defines
 * @{
 */

#define IS_PORT_ALL_PERIPH(PERIPH) (((PERIPH) == MDR_PORTA) || \
                                    ((PERIPH) == MDR_PORTB) || \
                                    ((PERIPH) == MDR_PORTC) || \
                                    ((PERIPH) == MDR_PORTD))

/** @} */ /* End of the group PORT_Private_Defines */

/** @defgroup PORT_Exported_Functions PORT Exported Functions
 * @{
 */

/**
 * @brief  Reset the MDR_PORTx peripheral registers to their default reset values.
 * @param  PORTx: Select MDR_PORTx peripheral. x can be (A..D).
 * @return None.
 */
void PORT_DeInit(MDR_PORT_TypeDef* PORTx)
{


    PORTx->OE     = 0;
    PORTx->FUNC   = 0;
    PORTx->ANALOG = 0;
    PORTx->PWR    = 0;
    PORTx->RXTX   = 0;
    PORTx->PULL   = 0;
}

/**
 * @brief  Initialize the PORTx peripheral according to the specified
 *         parameters in the PORT_InitStruct.
 * @param  PORTx: Select MDR_PORTx peripheral. x can be (A..D).
 * @param  PORT_InitStruct: The pointer to the @ref PORT_InitTypeDef structure that
 *         contains the configuration information for the specified PORT peripheral.
 * @return None.
 */
void PORT_Init(MDR_PORT_TypeDef* PORTx, const PORT_InitTypeDef* PORT_InitStruct)
{
    uint32_t TempOE;
    uint32_t TempFUNC;
    uint32_t TempANALOG;
    uint32_t TempPULL;
    uint32_t TempPWR;
    uint32_t PortPin, Pos, MaskS, MaskL, MaskD;
#if defined(USE_MDR1206)
    PORT_Power_TypeDef PinPower;
#endif



    /* Get current PORT register values. */
    TempOE     = PORTx->OE;
    TempFUNC   = PORTx->FUNC;
    TempANALOG = PORTx->ANALOG;
    TempPULL   = PORTx->PULL;
    TempPWR    = PORTx->PWR;

    /* Form new values. */
    Pos   = 0;
    MaskS = 0x0001;
    MaskL = 0x00000003;
#if defined(USE_MDR32F02_REV_2) || defined(USE_MDR1206FI) || defined(USE_MDR1206AFI) || defined(USE_MDR1206)
    MaskD = 0x00010001;
#else
    MaskD = 0x00000001;
#endif

#if defined(USE_MDR1206)
    // if (GetChipID() == CHIP_ID_MDR1206AFI) {
    //     PinPower = (PORT_Power_TypeDef)(~(PORT_InitStruct->PORT_Power) & 0x1);
    // } else {
        PinPower = (PORT_Power_TypeDef)(PORT_InitStruct->PORT_Power);
   // }
#endif

    for (PortPin = PORT_InitStruct->PORT_Pin; PortPin != 0; PortPin >>= 1) {
        if ((PortPin & 0x1) != 0) {
            /* Main settings PORT registers. */
            if (PORT_InitStruct->PORT_Mode == PORT_MODE_ANALOG) {
                TempOE &= ~(MaskS);
                TempFUNC &= ~(MaskL);
                TempANALOG &= ~(MaskS);
                TempPULL &= ~(MaskD);
            } else {
                switch (PORT_InitStruct->PORT_Direction) {
                    case PORT_DIRECTION_INPUT:
                        TempOE &= ~MaskS;
                        TempANALOG |= MaskS;
                        break;
                    case PORT_DIRECTION_OUTPUT:
                        TempOE |= MaskS;
                        TempANALOG &= ~MaskS;
                        break;
                    case PORT_DIRECTION_INPUT_OUTPUT:
                        TempOE |= MaskS;
                        TempANALOG |= MaskS;
                        break;
                    default:
                        break;
                }
                TempFUNC = (TempFUNC & ~MaskL) | ((uint32_t)PORT_InitStruct->PORT_Function << (Pos * 2));
                TempPULL = (TempPULL & ~MaskD) |
#if defined(USE_MDR32F02_REV_2) || defined(USE_MDR1206FI) || defined(USE_MDR1206AFI) || defined(USE_MDR1206)
                           ((uint32_t)PORT_InitStruct->PORT_PullUp << (Pos + PORT_PULL_PULL_UP_Pos)) |
#endif
                           ((uint32_t)PORT_InitStruct->PORT_PullDown << Pos);
#if defined(USE_MDR1206)
                TempPWR = (TempPWR & ~MaskL) | ((uint32_t)PinPower << (Pos * 2));
#else
                TempPWR = (TempPWR & ~MaskL) | ((uint32_t)PORT_InitStruct->PORT_Power << (Pos * 2));
#endif
            }
        }
        MaskS <<= 1;
        MaskL <<= 2;
        MaskD <<= 1;
        Pos++;
    }

    /* Partial deinitialization for all pins which will be configured. */
    PORTx->OE &= ~(MaskS);
    PORTx->FUNC &= ~(MaskL);
    PORTx->ANALOG &= ~(MaskS);

    /* Configure PORT registers with new values. */
    PORTx->PULL   = TempPULL & (~JTAG_PINS3(PORTx));
    PORTx->PWR    = TempPWR & (~JTAG_PINS2(PORTx));
    PORTx->ANALOG = TempANALOG & (~JTAG_PINS(PORTx));
    PORTx->FUNC   = TempFUNC & (~JTAG_PINS1(PORTx));
    PORTx->OE     = TempOE & (~JTAG_PINS(PORTx));
}

/**
 * @brief  Fill each PORT_InitStruct member with its default value.
 * @param  PORT_InitStruct: The pointer to the @ref PORT_InitTypeDef structure
 *         which is to be initialized.
 * @return None.
 */
void PORT_StructInit(PORT_InitTypeDef* PORT_InitStruct)
{
    /* Reset PORT initialization structure parameters values. */
    PORT_InitStruct->PORT_Pin       = PORT_PIN_ALL;
    PORT_InitStruct->PORT_Mode      = PORT_MODE_ANALOG;
    PORT_InitStruct->PORT_Direction = PORT_DIRECTION_INPUT;
    PORT_InitStruct->PORT_Function  = PORT_FUNCTION_PORT;
    PORT_InitStruct->PORT_Power     = PORT_POWER_NOMINAL_UPTO_2mA;
#if defined(USE_MDR32F02_REV_2) || defined(USE_MDR1206FI) || defined(USE_MDR1206AFI) || defined(USE_MDR1206)
    PORT_InitStruct->PORT_PullUp = PORT_PULL_UP_OFF;
#endif
    PORT_InitStruct->PORT_PullDown = PORT_PULL_DOWN_OFF;
}

/**
 * @brief  Read a specified input pin of a port.
 * @param  PORTx: Select MDR_PORTx peripheral. x can be (A..D).
 * @param  PORT_Pin: @ref PORT_Pin_TypeDef - specify a port bit to read.
 *         This parameter can be PORT_Pin_x where x can be:
 *           - (0..15) for MDR_PORTA,
 *           - (0..14) for MDR_PORTB,
 *           - (0...7) for MDR_PORTC,
 *           - (0..15) for MDR_PORTD.
 * @return @ref BitStatus - the value of the port input.
 */
BitStatus PORT_ReadInputDataBit(MDR_PORT_TypeDef* PORTx, PORT_Pin_TypeDef PORT_Pin)
{
    BitStatus Status;


    if ((PORTx->RXTX & (uint32_t)PORT_Pin) != RESET) {
        Status = SET;
    } else {
        Status = RESET;
    }

    return Status;
}

/**
 * @brief  Read the specified PORT input data port.
 * @param  PORTx: Select MDR_PORTx peripheral. x can be (A..D).
 * @return PORT input data port value. Bits (16..31) are always 0.
 */
uint32_t PORT_ReadInputData(MDR_PORT_TypeDef* PORTx)
{

    return (PORTx->RXTX);
}

/**
 * @brief  Read a specified output pin of a port.
 * @param  PORTx: Select MDR_PORTx peripheral. x can be (A..D).
 * @param  PORT_Pin: @ref PORT_Pin_TypeDef - specify a port bit to read.
 *         This parameter can be PORT_Pin_x where x can be:
 *           - (0..15) for MDR_PORTA,
 *           - (0..14) for MDR_PORTB,
 *           - (0...7) for MDR_PORTC,
 *           - (0..15) for MDR_PORTD.
 * @return @ref BitStatus - the output port pin value.
 */
BitStatus PORT_ReadOutputDataBit(MDR_PORT_TypeDef* PORTx, PORT_Pin_TypeDef PORT_Pin)
{
    BitStatus Status;

    if ((PORTx->RDTX & (uint32_t)PORT_Pin) != RESET) {
        Status = SET;
    } else {
        Status = RESET;
    }

    return Status;
}

/**
 * @brief  Read the output data of a specified port.
 * @param  PORTx: Select MDR_PORTx peripheral. x can be (A..D).
 * @return The output data of a specified port. Bits (16..31) are always 0.
 */
uint32_t PORT_ReadOutputData(MDR_PORT_TypeDef* PORTx)
{

    return (PORTx->RDTX);
}

/**
 * @brief  Set the selected port bits.
 * @param  PORTx: Select MDR_PORTx peripheral. x can be (A..D).
 * @param  PORT_Pin: @ref PORT_Pin_TypeDef - specify a port bit to write.
 *         This parameter can be any combination of PORT_Pin_x where x can be:
 *           - (0..15) for MDR_PORTA,
 *           - (0..14) for MDR_PORTB,
 *           - (0...7) for MDR_PORTC,
 *           - (0..15) for MDR_PORTD.
 * @return None.
 */
void PORT_SetBits(MDR_PORT_TypeDef* PORTx, uint32_t PORT_Pin)
{

    PORTx->SETTX = (PORT_Pin & ~JTAG_PINS(PORTx));
}

/**
 * @brief  Clear the selected port bits.
 * @param  PORTx: Select MDR_PORTx peripheral. x can be (A..D).
 * @param  PORT_Pin: @ref PORT_Pin_TypeDef - specify a port bit to write.
 *         This parameter can be any combination of PORT_Pin_x where x can be:
 *           - (0..15) for MDR_PORTA,
 *           - (0..14) for MDR_PORTB,
 *           - (0...7) for MDR_PORTC,
 *           - (0..15) for MDR_PORTD.
 * @return None.
 */
void PORT_ResetBits(MDR_PORT_TypeDef* PORTx, uint32_t PORT_Pin)
{

    PORTx->CLRTX = (PORT_Pin & ~JTAG_PINS(PORTx));
}

/**
 * @brief  Set or clear a selected port bit.
 * @param  PORTx: Select MDR_PORTx peripheral. x can be (A..D).
 * @param  PORT_Pin: @ref PORT_Pin_TypeDef - specify a port bit to write.
 *         This parameter can be any combination of PORT_Pin_x where x can be:
 *           - (0..15) for MDR_PORTA,
 *           - (0..14) for MDR_PORTB,
 *           - (0...7) for MDR_PORTC,
 *           - (0..15) for MDR_PORTD.
 * @param  BitVal: @ref BitStatus - specify the value to be written to the selected bit.
 *         This parameter can be one of the following values:
 *             @arg RESET: Clears the port pin,
 *             @arg SET: Sets the port pin.
 * @return None.
 */
void PORT_WriteBit(MDR_PORT_TypeDef* PORTx, uint32_t PORT_Pin, BitStatus BitVal)
{

    if (BitVal != RESET) {
        PORTx->SETTX = (PORT_Pin & ~JTAG_PINS(PORTx));
    } else {
        PORTx->CLRTX = (PORT_Pin & ~JTAG_PINS(PORTx));
    }
}

/**
 * @brief  Write data to the specified port.
 * @param  PORTx: Select MDR_PORTx peripheral. x can be (A..D).
 * @param  PortVal: Specify the value to be written to the port output data register.
 *           - Bits (16..31) are ignored for MDR_PORTA,
 *           - bits (15..31) are ignored for MDR_PORTB,
 *           - bits (8...31) are ignored for MDR_PORTC,
 *           - bits (16..31) are ignored for MDR_PORTD.
 * @return None.
 */
void PORT_Write(MDR_PORT_TypeDef* PORTx, uint32_t PortVal)
{

    PORTx->RXTX = PortVal & (~JTAG_PINS(PORTx));
}

/**
 * @brief  Toggle a selected port bit.
 * @param  PORTx: Select MDR_PORTx peripheral. x can be (A..D).
 * @param  PORT_Pin: @ref PORT_Pin_TypeDef - specify the port bit to toggle.
 *         This parameter can be any combination of PORT_Pin_x where x can be:
 *           - (0..15) for MDR_PORTA,
 *           - (0..14) for MDR_PORTB,
 *           - (0...7) for MDR_PORTC,
 *           - (0..15) for MDR_PORTD.
 * @return None.
 */
void PORT_ToggleBit(MDR_PORT_TypeDef* PORTx, uint32_t PORT_Pin)
{
    uint32_t Temp;


    Temp         = PORTx->RDTX;
    PORTx->CLRTX = Temp & PORT_Pin & ~JTAG_PINS(PORTx);
    PORTx->SETTX = ~Temp & PORT_Pin & ~JTAG_PINS(PORTx);
}

/** @} */ /* End of the group PORT_Exported_Functions */

/** @} */ /* End of the group PORT */

/** @} */ /* End of the group MDR32VF0xI_StdPeriph_Driver */

/*********************** (C) COPYRIGHT 2025 Milandr ****************************
 *
 * END OF FILE MDR32VF0xI_port.c */
