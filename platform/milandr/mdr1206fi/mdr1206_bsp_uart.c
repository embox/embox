/**
 *******************************************************************************
 * @file    MDR32VF0xI_uart.c
 * @author  Milandr Application Team
 * @version V0.2.0
 * @date    28/04/2025
 * @brief   This file contains all the UART firmware functions.
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
#include "mdr_uart.h"
#include "rst_clk.h"

#include "mdr1206fi.h"
/** @addtogroup MDR32VF0xI_StdPeriph_Driver MDR32VF0xI Standard Peripheral Driver
 * @{
 */

/** @addtogroup UART UART
 * @{
 */

/** @defgroup UART_Private_Defines UART Private Defines
 * @{
 */

#define IS_UART_ALL_PERIPH(PERIPH) (((PERIPH) == MDR_UART1) || \
                                    ((PERIPH) == MDR_UART2) || \
                                    ((PERIPH) == MDR_UART3) || \
                                    ((PERIPH) == MDR_UART4))

#define UART_FLAG_MASK (UART_FR_BUSY | UART_FR_RXFE | UART_FR_TXFF | UART_FR_RXFF | UART_FR_TXFE)

#define UART_IT_MASK   (UART_IMSC_RIMIM | UART_IMSC_CTSMIM | UART_IMSC_DCDMIM | UART_IMSC_DSRMIM |         \
                      UART_IMSC_RXIM | UART_IMSC_TXIM | UART_IMSC_RTIM | UART_IMSC_FEIM | UART_IMSC_PEIM | \
                      UART_IMSC_BEIM | UART_IMSC_OEIM | UART_IMSC_RNEIM | UART_IMSC_TFEIM | UART_IMSC_TNBSYIM)

#define UART_IT_CLR_MASK (UART_ICR_RIMIC | UART_ICR_CTSMIC | UART_ICR_DCDMIC | UART_ICR_DSRMIC |          \
                          UART_ICR_RXIC | UART_ICR_TXIC | UART_ICR_RTIC | UART_ICR_FEIC | UART_ICR_PEIC | \
                          UART_ICR_BEIC | UART_ICR_OEIC)

#define UART_MODEM_INPUT_MASK  (UART_FR_CTS | UART_FR_DSR | UART_FR_DCD | UART_FR_RI)

#define UART_MODEM_OUTPUT_MASK (UART_CR_DTR | UART_CR_RTS | UART_CR_OUT1 | UART_CR_OUT2)

#define UART_F_IRLPBAUD16      ((uint32_t)1843200) /*!< F_IrLPBaud16 nominal frequency Hz */
#define UART_F_IRLPBAUD16_MIN  ((uint32_t)1480000) /*!< F_IrLPBaud16 minimum frequency Hz */
#define UART_F_IRLPBAUD16_MAX  ((uint32_t)2120000) /*!< F_IrLPBaud16 maximum frequency Hz */

/** @} */ /* End of the group UART_Private_Defines */

/** @addtogroup UART_Exported_Functions UART Exported Functions
 * @{
 */

/**
 * @brief  Deinitialize the UARTx peripheral registers to their default reset values.
 * @param  UARTx: Select a UART peripheral.
 *         This parameter can be one of the MDR_UARTx values, where x is a number in the range [1; 4].
 * @return None.
 */
void UART_DeInit(MDR_UART_TypeDef* UARTx)
{
    UARTx->CR      = 0;
    UARTx->TCR     = 0;
    UARTx->RSR_ECR = 0;
    UARTx->ILPR    = 0;
    UARTx->IBRD    = 0;
    UARTx->FBRD    = 0;
    UARTx->LCR_H   = 0;
    UARTx->IFLS    = UART_IFLS_TXIFLSEL_8WORD | UART_IFLS_RXIFLSEL_8WORD;
    UARTx->IMSC    = 0;
    UARTx->ICR     = UART_IT_CLR_MASK;
    UARTx->DMACR   = 0;

    /* Set CR.RXE and CR.TXE bits. */
    UARTx->CR = UART_CR_TXE | UART_CR_RXE;
}

/**
 * @brief  Initialize a UARTx peripheral according to the specified
 *         parameters in the UART_InitStruct.
 * @note:  Specified peripheral clock UARTx_CLK should be configured before calling UART_Init() to set proper baud rate.
 * @param  UARTx: Select a UART peripheral.
 *         This parameter can be one of the MDR_UARTx values, where x is a number in the range [1; 4].
 * @param  UART_InitStruct: The pointer to the @ref UART_InitTypeDef structure
 *         that contains the configuration information for a specified UARTx peripheral.
 * @return @ref ErrorStatus - a baud rate status.
 */
ErrorStatus UART_Init(MDR_UART_TypeDef* UARTx, const UART_InitTypeDef* UART_InitStruct)
{
    uint32_t             TmpReg, UART_CLK_Freq;
    uint32_t             RealSpeed;
    uint32_t             Divider;
    uint32_t             IntegerDivider;
    uint32_t             FractionalDivider;
    uint32_t             SpeedError;
    RST_CLK_Freq_TypeDef RST_CLK_Clocks;
    ErrorStatus          Status = SUCCESS;



    /* UART CR configuration. */
    /* Set TXE and RXE bits according to the UART_Mode value. */
    /* Set RTSEn and CTSEn bits according to the UART_HardwareFlowControl value. */
    /* Set LBE bit according to the UART_LoopbackMode value. */
    /* Set SIREN bit according to the UART_IRDA_Mode value. */
    /* Set SIRLP bit according to the UART_IRDA_PowerMode value. */
    TmpReg = (uint32_t)UART_InitStruct->UART_Mode |
             (uint32_t)UART_InitStruct->UART_HardwareFlowControl |
             (uint32_t)UART_InitStruct->UART_LoopbackMode |
             (uint32_t)UART_InitStruct->UART_IRDA_Mode |
             (uint32_t)UART_InitStruct->UART_IRDA_PowerMode;
    UARTx->CR = TmpReg;

    /* Configure UART Baud Rate. */
    RST_CLK_GetClocksFreq(&RST_CLK_Clocks, (RST_CLK_CLOCK_FREQ_UART1 | RST_CLK_CLOCK_FREQ_UART2 | RST_CLK_CLOCK_FREQ_UART3 | RST_CLK_CLOCK_FREQ_UART4));

    if (UARTx == MDR_UART1) {
        UART_CLK_Freq = RST_CLK_Clocks.UART_Frequency[0];
    } else if (UARTx == MDR_UART2) {
        UART_CLK_Freq = RST_CLK_Clocks.UART_Frequency[1];
    } else if (UARTx == MDR_UART3) {
        UART_CLK_Freq = RST_CLK_Clocks.UART_Frequency[2];
    } else {
        UART_CLK_Freq = RST_CLK_Clocks.UART_Frequency[3];
    }

    /* Determine the integer part. */
    Divider        = (UART_CLK_Freq << 3) / UART_InitStruct->UART_BaudRate;
    IntegerDivider = Divider >> 7;
    /* Determine the fractional part. */
    if (IntegerDivider == 0) {
        IntegerDivider    = 1;
        FractionalDivider = 0;
    } else if (IntegerDivider >= 65535) {
        IntegerDivider    = 65535;
        FractionalDivider = 0;
    } else {
        FractionalDivider = ((Divider & 0x7F) + 1) >> 1;
    }
    /* Determine a speed error. */
    RealSpeed  = (UART_CLK_Freq << 2) / ((IntegerDivider << 6) + FractionalDivider);
    SpeedError = (RealSpeed * 100) / UART_InitStruct->UART_BaudRate;
    if ((SpeedError < 98) || (SpeedError > 102)) {
        Status = ERROR;
    } else {
        /* Write UART baud rate. */
        UARTx->IBRD = IntegerDivider;
        UARTx->FBRD = FractionalDivider;

        /* UART LCR_H configuration. */
        /* Set WLEN bits according to the UART_WordLength value. */
        /* Set STP2 bit according to the UART_StopBits value. */
        /* Set PEN, EPS and SPS bits according to the UART_Parity value. */
        /* Set FEN bit according to the UART_FIFO_Mode value. */
        TmpReg = (uint32_t)UART_InitStruct->UART_WordLength |
                 (uint32_t)UART_InitStruct->UART_StopBits |
                 (uint32_t)UART_InitStruct->UART_Parity |
                 (uint32_t)UART_InitStruct->UART_FIFO_Mode;
        UARTx->LCR_H = TmpReg;

        /* UART IFLS configuration. */
        UARTx->IFLS = (uint32_t)UART_InitStruct->UART_FIFO_TxThreshold |
                      (uint32_t)UART_InitStruct->UART_FIFO_RxThreshold;

        /* Configure IrDA mode. */
        if (UART_InitStruct->UART_IRDA_Mode == UART_IRDA_MODE_ENABLE) {
            /* Configure loopback mode. */
            if (UART_InitStruct->UART_LoopbackMode == UART_LOOPBACK_ENABLE) {
                UARTx->TCR |= UART_TCR_SIRTEST;
            } else {
                UARTx->TCR &= ~UART_TCR_SIRTEST;
            }

            /* UART ILPR configuration when IrDA low power mode is selected. */
            if (UART_InitStruct->UART_IRDA_PowerMode == UART_IRDA_POWER_LOW) {
                /* Determine the divider. */
                Divider = UART_CLK_Freq / UART_F_IRLPBAUD16;
                if (Divider == 0) {
                    Status = ERROR;
                } else {
                    /* Determine a speed error. */
                    RealSpeed = UART_CLK_Freq / Divider;
                    if ((RealSpeed < UART_F_IRLPBAUD16_MIN) || (RealSpeed > UART_F_IRLPBAUD16_MAX)) {
                        Status = ERROR;
                    } else {
                        UARTx->ILPR = Divider;
                    }
                }
            }
        }
    }

    return Status;
}

/**
 * @brief  Fill each UART_InitStruct member with its default value.
 * @param  UART_InitStruct: The pointer to the @ref UART_InitTypeDef structure
 *         which is to be initialized.
 * @return None.
 */
void UART_StructInit(UART_InitTypeDef* UART_InitStruct)
{
    UART_InitStruct->UART_BaudRate            = 9600;
    UART_InitStruct->UART_Mode                = UART_MODE_TX_RX;
    UART_InitStruct->UART_WordLength          = UART_WORD_LENGTH_8BIT;
    UART_InitStruct->UART_StopBits            = UART_STOP_BITS_1BIT;
    UART_InitStruct->UART_Parity              = UART_PARITY_NONE;
    UART_InitStruct->UART_HardwareFlowControl = UART_HARDWARE_FLOW_CONTROL_NONE;
    UART_InitStruct->UART_LoopbackMode        = UART_LOOPBACK_DISABLE;

    UART_InitStruct->UART_FIFO_Mode        = UART_FIFO_MODE_DISABLE;
    UART_InitStruct->UART_FIFO_TxThreshold = UART_FIFO_TX_THRESHOLD_8WORD;
    UART_InitStruct->UART_FIFO_RxThreshold = UART_FIFO_RX_THRESHOLD_8WORD;

    UART_InitStruct->UART_IRDA_Mode      = UART_IRDA_MODE_DISABLE;
    UART_InitStruct->UART_IRDA_PowerMode = UART_IRDA_POWER_NORMAL;
}

/**
 * @brief  Enable or disable a specified UARTx peripheral.
 * @param  UARTx: Select a UART peripheral.
 *         This parameter can be one of the MDR_UARTx values, where x is a number in the range [1; 4].
 * @param  NewState: @ref FunctionalState - a new state of the UARTx peripheral.
 * @return None.
 */
void UART_Cmd(MDR_UART_TypeDef* UARTx, FunctionalState NewState)
{


    if (NewState != DISABLE) {
        /* Enable the selected UART by setting the UARTEN bit in the CR register. */
        UARTx->CR |= UART_CR_UARTEN;
    } else {
        /* Disable the selected UART by clearing the UARTEN bit in the CR register. */
        UARTx->CR &= ~UART_CR_UARTEN;
    }
}
