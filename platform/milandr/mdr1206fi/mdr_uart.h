/**
 *******************************************************************************
 * @file    MDR32VF0xI_uart.h
 * @author  Milandr Application Team
 * @version V0.1.0
 * @date    18/04/2024
 * @brief   This file contains all the functions prototypes for the UART
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
#ifndef MDR32VF0xI_UART_H
#define MDR32VF0xI_UART_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "rst_clk.h"
#include "mdr1206fi.h"


/** @addtogroup MDR32VF0xI_StdPeriph_Driver MDR32VF0xI Standard Peripheral Driver
 * @{
 */

/** @addtogroup UART UART
 * @{
 */

/** @defgroup UART_Exported_Types UART Exported Types
 * @{
 */

/**
 * @brief UART transfer mode.
 */
typedef enum {
    UART_MODE_TX    = UART_CR_TXE,                /*!< Enable only the transmitter. */
    UART_MODE_RX    = UART_CR_RXE,                /*!< Enable only the receiver. */
    UART_MODE_TX_RX = (UART_CR_TXE | UART_CR_RXE) /*!< Enable the transmitter and receiver. */
} UART_Mode_TypeDef;

#define IS_UART_MODE(MODE) (((MODE) == UART_MODE_TX) || \
                            ((MODE) == UART_MODE_RX) || \
                            ((MODE) == UART_MODE_TX_RX))

/**
 * @brief UART word length.
 */
typedef enum {
    UART_WORD_LENGTH_5BIT = UART_LCR_H_WLEN_5BIT, /*!< Select a 5-bit word length. */
    UART_WORD_LENGTH_6BIT = UART_LCR_H_WLEN_6BIT, /*!< Select a 6-bit word length. */
    UART_WORD_LENGTH_7BIT = UART_LCR_H_WLEN_7BIT, /*!< Select a 7-bit word length. */
    UART_WORD_LENGTH_8BIT = UART_LCR_H_WLEN_8BIT, /*!< Select a 8-bit word length. */
    UART_WORD_LENGTH_9BIT = UART_LCR_H_WLEN_9BIT  /*!< Select a 9-bit word length. */
} UART_WordLength_TypeDef;

#define IS_UART_WORD_LENGTH(LENGTH) (((LENGTH) == UART_WORD_LENGTH_5BIT) || \
                                     ((LENGTH) == UART_WORD_LENGTH_6BIT) || \
                                     ((LENGTH) == UART_WORD_LENGTH_7BIT) || \
                                     ((LENGTH) == UART_WORD_LENGTH_8BIT) || \
                                     ((LENGTH) == UART_WORD_LENGTH_9BIT))

/**
 * @brief UART stop bits.
 */
typedef enum {
    UART_STOP_BITS_1BIT = (((uint32_t)0x0) << UART_LCR_H_STP2_Pos), /*!< Select 1 stop bit at the end of a frame when transmitting. */
    UART_STOP_BITS_2BIT = (((uint32_t)0x1) << UART_LCR_H_STP2_Pos)  /*!< Select 2 stop bits at the end of a frame when transmitting . */
} UART_StopBits_TypeDef;

#define IS_UART_STOP_BITS(STOP_BITS) (((STOP_BITS) == UART_STOP_BITS_1BIT) || \
                                      ((STOP_BITS) == UART_STOP_BITS_2BIT))

/**
 * @brief UART parity control.
 */
typedef enum {
    UART_PARITY_NONE = ((uint32_t)0x0),                                   /*!< Parity control is disabled. */
    UART_PARITY_ODD  = (UART_LCR_H_PEN),                                  /*!< Parity control is enabled, odd parity is selected. */
    UART_PARITY_EVEN = (UART_LCR_H_PEN | UART_LCR_H_EPS),                 /*!< Parity control is enabled, even parity is selected. */
    UART_PARITY_ONE  = (UART_LCR_H_PEN | UART_LCR_H_SPS),                 /*!< Select the mode in which the parity bit is always 1. */
    UART_PARITY_ZERO = (UART_LCR_H_PEN | UART_LCR_H_EPS | UART_LCR_H_SPS) /*!< Select the mode in which the parity bit is always 0. */
} UART_Parity_TypeDef;

#define IS_UART_PARITY(PARITY) (((PARITY) == UART_PARITY_NONE) || \
                                ((PARITY) == UART_PARITY_ODD) ||  \
                                ((PARITY) == UART_PARITY_EVEN) || \
                                ((PARITY) == UART_PARITY_ONE) ||  \
                                ((PARITY) == UART_PARITY_ZERO))

/**
 * @brief UART hardware flow control.
 */
typedef enum {
    UART_HARDWARE_FLOW_CONTROL_NONE    = ((uint32_t)0x0),                /*!< Disable RTS and CTS flow control. */
    UART_HARDWARE_FLOW_CONTROL_RTS     = UART_CR_RTSEN,                  /*!< Enable only RTS flow control. */
    UART_HARDWARE_FLOW_CONTROL_CTS     = UART_CR_CTSEN,                  /*!< Enable only CTS flow control. */
    UART_HARDWARE_FLOW_CONTROL_RTS_CTS = (UART_CR_RTSEN | UART_CR_CTSEN) /*!< Enable RTS and CTS flow control. */
} UART_HardwareFlowControl_TypeDef;

#define IS_UART_HARDWARE_FLOW_CONTROL(CONTROL) (((CONTROL) == UART_HARDWARE_FLOW_CONTROL_NONE) || \
                                                ((CONTROL) == UART_HARDWARE_FLOW_CONTROL_RTS) ||  \
                                                ((CONTROL) == UART_HARDWARE_FLOW_CONTROL_CTS) ||  \
                                                ((CONTROL) == UART_HARDWARE_FLOW_CONTROL_RTS_CTS))

/**
 * @brief UART loopback mode.
 */
typedef enum {
    UART_LOOPBACK_DISABLE = (((uint32_t)0x0) << UART_CR_LBE_Pos), /*!< Disable loopback mode. */
    UART_LOOPBACK_ENABLE  = (((uint32_t)0x1) << UART_CR_LBE_Pos)  /*!< Enable loopback mode. */
} UART_LoopbackMode_TypeDef;

#define IS_UART_LOOPBACK_MODE(LOOPBACK) (((LOOPBACK) == UART_LOOPBACK_DISABLE) || \
                                         ((LOOPBACK) == UART_LOOPBACK_ENABLE))

/**
 * @brief UART FIFO mode.
 */
typedef enum {
    UART_FIFO_MODE_DISABLE = (((uint32_t)0x0) << UART_LCR_H_FEN_Pos), /*!< Disable FIFO mode. */
    UART_FIFO_MODE_ENABLE  = (((uint32_t)0x1) << UART_LCR_H_FEN_Pos)  /*!< Enable FIFO mode. */
} UART_FIFO_Mode_TypeDef;

#define IS_UART_FIFO_MODE(MODE) (((MODE) == UART_FIFO_MODE_DISABLE) || \
                                 ((MODE) == UART_FIFO_MODE_ENABLE))

/**
 * @brief UART TX FIFO threshold level.
 */
typedef enum {
    UART_FIFO_TX_THRESHOLD_2WORD  = UART_IFLS_TXIFLSEL_2WORD,  /*!< Select the threshold level equal to 1/8 for the TX FIFO buffer. */
    UART_FIFO_TX_THRESHOLD_4WORD  = UART_IFLS_TXIFLSEL_4WORD,  /*!< Select the threshold level equal to 1/4 for the TX FIFO buffer. */
    UART_FIFO_TX_THRESHOLD_8WORD  = UART_IFLS_TXIFLSEL_8WORD,  /*!< Select the threshold level equal to 1/2 for the TX FIFO buffer. */
    UART_FIFO_TX_THRESHOLD_12WORD = UART_IFLS_TXIFLSEL_12WORD, /*!< Select the threshold level equal to 3/4 for the TX FIFO buffer. */
    UART_FIFO_TX_THRESHOLD_14WORD = UART_IFLS_TXIFLSEL_14WORD  /*!< Select the threshold level equal to 7/8 for the TX FIFO buffer. */
} UART_FIFO_TxThreshold_TypeDef;

#define IS_UART_FIFO_TX_THRESHOLD(THRESHOLD) (((THRESHOLD) == UART_FIFO_TX_THRESHOLD_2WORD) ||  \
                                              ((THRESHOLD) == UART_FIFO_TX_THRESHOLD_4WORD) ||  \
                                              ((THRESHOLD) == UART_FIFO_TX_THRESHOLD_8WORD) ||  \
                                              ((THRESHOLD) == UART_FIFO_TX_THRESHOLD_12WORD) || \
                                              ((THRESHOLD) == UART_FIFO_TX_THRESHOLD_14WORD))

/**
 * @brief UART RX FIFO threshold level.
 */
typedef enum {
    UART_FIFO_RX_THRESHOLD_2WORD  = UART_IFLS_RXIFLSEL_2WORD,  /*!< Select the threshold level equal to 1/8 for the RX FIFO buffer. */
    UART_FIFO_RX_THRESHOLD_4WORD  = UART_IFLS_RXIFLSEL_4WORD,  /*!< Select the threshold level equal to 1/4 for the RX FIFO buffer. */
    UART_FIFO_RX_THRESHOLD_8WORD  = UART_IFLS_RXIFLSEL_8WORD,  /*!< Select the threshold level equal to 1/2 for the RX FIFO buffer. */
    UART_FIFO_RX_THRESHOLD_12WORD = UART_IFLS_RXIFLSEL_12WORD, /*!< Select the threshold level equal to 3/4 for the RX FIFO buffer. */
    UART_FIFO_RX_THRESHOLD_14WORD = UART_IFLS_RXIFLSEL_14WORD  /*!< Select the threshold level equal to 7/8 for the RX FIFO buffer. */
} UART_FIFO_RxThreshold_TypeDef;

#define IS_UART_FIFO_RX_THRESHOLD(THRESHOLD) (((THRESHOLD) == UART_FIFO_RX_THRESHOLD_2WORD) ||  \
                                              ((THRESHOLD) == UART_FIFO_RX_THRESHOLD_4WORD) ||  \
                                              ((THRESHOLD) == UART_FIFO_RX_THRESHOLD_8WORD) ||  \
                                              ((THRESHOLD) == UART_FIFO_RX_THRESHOLD_12WORD) || \
                                              ((THRESHOLD) == UART_FIFO_RX_THRESHOLD_14WORD))

/**
 * @brief UART IrDA mode.
 */
typedef enum {
    UART_IRDA_MODE_DISABLE = (((uint32_t)0x0) << UART_CR_SIREN_Pos), /*!< Disable IrDA mode. */
    UART_IRDA_MODE_ENABLE  = (((uint32_t)0x1) << UART_CR_SIREN_Pos)  /*!< Enable IrDA mode. */
} UART_IRDA_Mode_TypeDef;

#define IS_UART_IRDA_MODE(MODE) (((MODE) == UART_IRDA_MODE_DISABLE) || \
                                 ((MODE) == UART_IRDA_MODE_ENABLE))

/**
 * @brief UART IrDA power mode.
 */
typedef enum {
    UART_IRDA_POWER_NORMAL = (((uint32_t)0x0) << UART_CR_SIRLP_Pos), /*!< Select the IrDA normal power mode (pulse transmission uses Baud16 clock, speed up to 460800 baud). */
    UART_IRDA_POWER_LOW    = (((uint32_t)0x1) << UART_CR_SIRLP_Pos)  /*!< Select the IrDA low power mode (pulse transmission uses IrLPBaud16 clock, speed up to 115200 baud). */
} UART_IRDA_PowerMode_TypeDef;

#define IS_UART_IRDA_POWER_MODE(POWER) (((POWER) == UART_IRDA_POWER_NORMAL) || \
                                        ((POWER) == UART_IRDA_POWER_LOW))

/**
 * @brief UART modem control input lines.
 */
typedef enum {
    UART_MODEM_INPUT_CTS = UART_FR_CTS, /*!< Inverted state on the nUARTCTS line. */
    UART_MODEM_INPUT_DSR = UART_FR_DSR, /*!< Inverted state on the nUARTDSR line. */
    UART_MODEM_INPUT_DCD = UART_FR_DCD, /*!< Inverted state on the nUARTDCD line. */
    UART_MODEM_INPUT_RI  = UART_FR_RI   /*!< Inverted state on the nUARTRI line. */
} UART_ModemInput_TypeDef;

#define IS_UART_MODEM_INPUT(INPUT) (((INPUT) == UART_MODEM_INPUT_CTS) || \
                                    ((INPUT) == UART_MODEM_INPUT_DSR) || \
                                    ((INPUT) == UART_MODEM_INPUT_DCD) || \
                                    ((INPUT) == UART_MODEM_INPUT_RI))

/**
 * @brief UART modem control output lines.
 */
typedef enum {
    UART_MODEM_OUTPUT_DTR  = UART_CR_DTR,  /*!< Set the inverted value on the nUARTDTR line. */
    UART_MODEM_OUTPUT_RTS  = UART_CR_RTS,  /*!< Set the inverted value on the nUARTRTS line (used only if CR.RTSEn = 0). */
    UART_MODEM_OUTPUT_OUT1 = UART_CR_OUT1, /*!< Set the inverted value on the nUARTOut1 line (used only in loopback mode: nUARTDCD = nUARTOut1). */
    UART_MODEM_OUTPUT_OUT2 = UART_CR_OUT2  /*!< Set the inverted value on the nUARTOut2 line (used only in loopback mode: nUARTRI = nUARTOut2). */
} UART_ModemOutput_TypeDef;

#define IS_UART_MODEM_OUTPUT(OUTPUT) (((OUTPUT) == UART_MODEM_OUTPUT_DTR) ||  \
                                      ((OUTPUT) == UART_MODEM_OUTPUT_RTS) ||  \
                                      ((OUTPUT) == UART_MODEM_OUTPUT_OUT1) || \
                                      ((OUTPUT) == UART_MODEM_OUTPUT_OUT2))

#define IS_UART_MODEM_OUTPUTS(OUTPUTS) (((OUTPUTS) & ~(uint32_t)(UART_MODEM_OUTPUT_DTR |  \
                                                                 UART_MODEM_OUTPUT_RTS |  \
                                                                 UART_MODEM_OUTPUT_OUT1 | \
                                                                 UART_MODEM_OUTPUT_OUT2)) == 0)

/**
 * @brief UART status flags.
 */
typedef enum {
    UART_FLAG_BUSY = UART_FR_BUSY, /*!< Transmitter is busy. */
    UART_FLAG_RXFE = UART_FR_RXFE, /*!< Receive FIFO is empty. */
    UART_FLAG_TXFF = UART_FR_TXFF, /*!< Transmit FIFO is full. */
    UART_FLAG_RXFF = UART_FR_RXFF, /*!< Receive FIFO is full. */
    UART_FLAG_TXFE = UART_FR_TXFE  /*!< Transmit FIFO is empty. */
} UART_Flags_TypeDef;

#define IS_UART_FLAG(FLAG) (((FLAG) == UART_FLAG_BUSY) || \
                            ((FLAG) == UART_FLAG_RXFE) || \
                            ((FLAG) == UART_FLAG_TXFF) || \
                            ((FLAG) == UART_FLAG_RXFF) || \
                            ((FLAG) == UART_FLAG_TXFE))

/**
 * @brief UART interrupt definition.
 */
typedef enum {
    UART_IT_RI    = UART_IMSC_RIMIM,   /*!< Line nUARTRI change interrupt (UARTRIINTR). */
    UART_IT_CTS   = UART_IMSC_CTSMIM,  /*!< Line nUARTCTS change interrupt (UARTCTSINTR). */
    UART_IT_DCD   = UART_IMSC_DCDMIM,  /*!< Line nUARTDCD change interrupt (UARTDCDINTR). */
    UART_IT_DSR   = UART_IMSC_DSRMIM,  /*!< Line nUARTDSR change interrupt (UARTDSRINTR). */
    UART_IT_RX    = UART_IMSC_RXIM,    /*!< Receiver interrupt (UARTRXINTR). */
    UART_IT_TX    = UART_IMSC_TXIM,    /*!< Transmitter interrupt (UARTTXINTR). */
    UART_IT_RT    = UART_IMSC_RTIM,    /*!< Data input timeout interrupt (UARTRTINTR). */
    UART_IT_FE    = UART_IMSC_FEIM,    /*!< Frame structure error interrupt (UARTFEINTR). */
    UART_IT_PE    = UART_IMSC_PEIM,    /*!< Parity error interrupt (UARTPEINTR). */
    UART_IT_BE    = UART_IMSC_BEIM,    /*!< Line break interrupt (UARTBEINTR). */
    UART_IT_OE    = UART_IMSC_OEIM,    /*!< Receive FIFO overrun interrupt (UARTOEINTR). */
    UART_IT_RNE   = UART_IMSC_RNEIM,   /*!< Receive FIFO not empty interrupt (UARTRNEINTR). */
    UART_IT_TFE   = UART_IMSC_TFEIM,   /*!< Transmit FIFO empty interrupt (UARTTFEINTR). */
    UART_IT_TNBSY = UART_IMSC_TNBSYIM, /*!< Transmit shift register empty interrupt (UARTTNBSYINTR). */
} UART_IT_TypeDef;

#define IS_UART_CONFIG_IT(IT) (((IT) == UART_IT_RI) || ((IT) == UART_IT_CTS) ||  \
                               ((IT) == UART_IT_DCD) || ((IT) == UART_IT_DSR) || \
                               ((IT) == UART_IT_RX) || ((IT) == UART_IT_TX) ||   \
                               ((IT) == UART_IT_RT) || ((IT) == UART_IT_FE) ||   \
                               ((IT) == UART_IT_PE) || ((IT) == UART_IT_BE) ||   \
                               ((IT) == UART_IT_OE) || ((IT) == UART_IT_RNE) ||  \
                               ((IT) == UART_IT_TFE) || ((IT) == UART_IT_TNBSY))

#define IS_UART_CONFIG_ITS(ITS) (((ITS) & ~(uint32_t)(UART_IT_RI | UART_IT_CTS |  \
                                                      UART_IT_DCD | UART_IT_DSR | \
                                                      UART_IT_RX | UART_IT_TX |   \
                                                      UART_IT_RT | UART_IT_FE |   \
                                                      UART_IT_PE | UART_IT_BE |   \
                                                      UART_IT_OE | UART_IT_RNE |  \
                                                      UART_IT_TFE | UART_IT_TNBSY)) == 0)

/**
 * @brief UART DMA control.
 */
typedef enum {
    UART_DMA_REQ_RX     = UART_DMACR_RXDMAE,  /*!< Enable request to the DMA from receiver. */
    UART_DMA_REQ_TX     = UART_DMACR_TXDMAE,  /*!< Enable request to the DMA from transmitter. */
    UART_DMA_STOP_RXERR = UART_DMACR_DMAONERR /*!< Enable blocking of a request to the DMA when a receive error is detected. */
} UART_DMA_Control_TypeDef;

#define IS_UART_DMA_CONTROL(CONTROL) (((CONTROL) == UART_DMA_REQ_RX) || \
                                      ((CONTROL) == UART_DMA_REQ_TX) || \
                                      ((CONTROL) == UART_DMA_STOP_RXERR))

#define IS_UART_DMA_CONTROLS(CONTROL) (((CONTROL) & ~(uint32_t)(UART_DMA_REQ_RX | \
                                                                UART_DMA_REQ_TX | \
                                                                UART_DMA_STOP_RXERR)) == 0)

/**
 * @brief  UART init structure definition.
 */
typedef struct {
    uint32_t UART_BaudRate;                                    /*!< This member configures the UART baud rate.
                                                                    The baud rate is computed using the following formula:
                                                                    - IntegerDivider = (uint32_t)(UARTCLK / (16 * UART_BaudRate)).
                                                                    - FractionalDivider = (uint32_t)((((float)IntegerDivider - (uint32_t)IntegerDivider) * 64) + 0.5). */
    UART_Mode_TypeDef UART_Mode;                               /*!< Specify the permission for the transmitter and receiver.
                                                                    This parameter can be a value of the @ref UART_Mode_TypeDef. */
    UART_WordLength_TypeDef UART_WordLength;                   /*!< Specify the number of data bits to be transmitted or received in a frame.
                                                                    This parameter can be a value of the @ref UART_WordLength_TypeDef. */
    UART_StopBits_TypeDef UART_StopBits;                       /*!< Specify the number of stop bits to transmit.
                                                                    This parameter can be a value of the @ref UART_StopBits_TypeDef. */
    UART_Parity_TypeDef UART_Parity;                           /*!< Specify the parity mode.
                                                                    This parameter can be a value of the @ref UART_Parity_TypeDef. */
    UART_HardwareFlowControl_TypeDef UART_HardwareFlowControl; /*!< Specify the hardware flow control mode.
                                                                    This parameter can be a value of the @ref UART_HardwareFlowControl_TypeDef. */
    UART_LoopbackMode_TypeDef UART_LoopbackMode;               /*!< Enable or disable the loopback mode.
                                                                    This parameter can be a value of the @ref UART_LoopbackMode_TypeDef. */

    UART_FIFO_Mode_TypeDef UART_FIFO_Mode;               /*!< Specify whether the FIFO mode is used for TX and RX buffers.
                                                              This parameter can be a value of the @ref UART_FIFO_Mode_TypeDef.
                                                              If the FIFO mode is used, then TX and RX buffers are 16 location deep,
                                                              else TX and RX buffers become 1-bit deep holding registers. */
    UART_FIFO_TxThreshold_TypeDef UART_FIFO_TxThreshold; /*!< Specify the threshold level of the TX FIFO for the generation of the events. Used only in FIFO mode.
                                                              This parameter can be a value of the @ref UART_FIFO_TxThreshold_TypeDef. */
    UART_FIFO_RxThreshold_TypeDef UART_FIFO_RxThreshold; /*!< Specify the threshold level of the RX FIFO for the generation of the events. Used only in FIFO mode.
                                                              This parameter can be a value of the @ref UART_FIFO_RxThreshold_TypeDef. */

    UART_IRDA_Mode_TypeDef UART_IRDA_Mode;           /*!< Specify whether the IrDA mode is used.
                                                          This parameter can be a value of the @ref UART_IRDA_Mode_TypeDef. */
    UART_IRDA_PowerMode_TypeDef UART_IRDA_PowerMode; /*!< Specify the IrDA power mode. Used only in IrDA mode.
                                                          This parameter can be a value of the @ref UART_IRDA_PowerMode_TypeDef. */
} UART_InitTypeDef;

/** @} */ /* End of the group UART_Exported_Types */

/** @defgroup UART_Exported_Defines UART Exported Defines
 * @{
 */

/** @defgroup UART_Baudrate UART Baudrate
 * @{
 */

#define IS_UART_BAUDRATE(BAUDRATE) (((BAUDRATE) > 0) && ((BAUDRATE) <= 921600))

/** @} */ /* End of the group UART_Baudrate */

/** @defgroup UART_Data_Status UART Data Status
 * @{
 */

#define UART_DATA_FE                      UART_DR_FE
#define UART_DATA_PE                      UART_DR_PE
#define UART_DATA_BE                      UART_DR_BE
#define UART_DATA_OE                      UART_DR_OE

#define UART_DATA(DATA)                   ((uint16_t)((DATA) & UART_DR_DATA_Msk))
#define UART_FLAGS(DATA)                  ((uint16_t)((DATA) >> 9))
#define UART_CHECK_DATA_STATUS(DATA, BIT) ((((uint32_t)(DATA)) << (31 - (BIT))) >> 31)

/** @} */ /* End of the group UART_Data_Status */

/** @} */ /* End of the group UART_Exported_Defines */

/** @addtogroup UART_Exported_Functions UART Exported Functions
 * @{
 */

void        UART_DeInit(MDR_UART_TypeDef* UARTx);
ErrorStatus UART_Init(MDR_UART_TypeDef* UARTx, const UART_InitTypeDef* UART_InitStruct);
void        UART_StructInit(UART_InitTypeDef* UART_InitStruct);

void            UART_Cmd(MDR_UART_TypeDef* UARTx, FunctionalState NewState);
FunctionalState UART_GetCmdState(MDR_UART_TypeDef* UARTx);

void     UART_SendData(MDR_UART_TypeDef* UARTx, uint16_t Data);
uint16_t UART_ReceiveData(MDR_UART_TypeDef* UARTx);

void UART_BreakLine(MDR_UART_TypeDef* UARTx, FunctionalState NewState);

FlagStatus UART_GetFlagStatus(MDR_UART_TypeDef* UARTx, UART_Flags_TypeDef Flag);
uint32_t   UART_GetStatus(MDR_UART_TypeDef* UARTx);

void     UART_ITConfig(MDR_UART_TypeDef* UARTx, uint32_t ITs, FunctionalState NewState);
ITStatus UART_GetITFlagStatus(MDR_UART_TypeDef* UARTx, UART_IT_TypeDef IT);
uint32_t UART_GetITStatus(MDR_UART_TypeDef* UARTx);
ITStatus UART_GetITNonMaskedFlagStatus(MDR_UART_TypeDef* UARTx, UART_IT_TypeDef IT);
uint32_t UART_GetITNonMaskedStatus(MDR_UART_TypeDef* UARTx);
void     UART_ClearITFlags(MDR_UART_TypeDef* UARTx, uint32_t ITs);

void            UART_DMACmd(MDR_UART_TypeDef* UARTx, uint32_t DMAControl, FunctionalState NewState);
FunctionalState UART_GetDMACmdState(MDR_UART_TypeDef* UARTx, UART_DMA_Control_TypeDef DMAControl);

FlagStatus UART_GetModemInput(MDR_UART_TypeDef* UARTx, UART_ModemInput_TypeDef Input);
uint32_t   UART_GetModemInputs(MDR_UART_TypeDef* UARTx);

void       UART_SetModemOutputs(MDR_UART_TypeDef* UARTx, uint32_t Outputs);
void       UART_ClearModemOutputs(MDR_UART_TypeDef* UARTx, uint32_t Outputs);
FlagStatus UART_GetModemOutput(MDR_UART_TypeDef* UARTx, UART_ModemOutput_TypeDef Output);
uint32_t   UART_GetModemOutputs(MDR_UART_TypeDef* UARTx);

/** @} */ /* End of the group UART_Exported_Functions */

/** @} */ /* End of the group UART */

/** @} */ /* End of the group MDR32VF0xI_StdPeriph_Driver */

#ifdef __cplusplus
} // extern "C" block end
#endif

#endif /* MDR32VF0xI_UART_H */

/*********************** (C) COPYRIGHT 2025 Milandr ****************************
 *
 * END OF FILE MDR32VF0xI_uart.h */
