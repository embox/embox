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


        PinPower = (PORT_Power_TypeDef)(PORT_InitStruct->PORT_Power);


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
