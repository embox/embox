/**
 * @brief IMX UART memory mapped registers
 * @details i.MX Processor Reference Manual
 *
 * @date 14.12.23
 * @author Aleksey Zhmulin
 */
#ifndef DRIVERS_SERIAL_IMX_UART_IMX_UART_REGS_H_
#define DRIVERS_SERIAL_IMX_UART_IMX_UART_REGS_H_

/* clang-format off */

#define IMX_URXD(base)        (base + 0x00) /* UART Receiver Register */
#define IMX_UTXD(base)        (base + 0x40) /* UART Transmitter Register */
#define IMX_UCR1(base)        (base + 0x80) /* UART Control Register 1 */
#define IMX_UCR2(base)        (base + 0x84) /* UART Control Register 2 */
#define IMX_UCR3(base)        (base + 0x88) /* UART Control Register 3 */
#define IMX_UCR4(base)        (base + 0x8c) /* UART Control Register 4 */
#define IMX_UFCR(base)        (base + 0x90) /* UART FIFO Control Register */
#define IMX_USR1(base)        (base + 0x94) /* UART Status Register 1 */
#define IMX_USR2(base)        (base + 0x98) /* UART Status Register 2 */
#define IMX_UESC(base)        (base + 0x9c) /* UART Escape Character Register */
#define IMX_UTIM(base)        (base + 0xa0) /* UART Escape Timer Register */
#define IMX_UBIR(base)        (base + 0xa4) /* UART BRM Incremental Register */
#define IMX_UBMR(base)        (base + 0xa8) /* UART BRM Modulator Register */
#define IMX_UBRC(base)        (base + 0xac) /* UART Baud Rate Count Register */
#define IMX_ONEMS(base)       (base + 0xb0) /* UART One Millisecond Register */
#define IMX_UTS(base)         (base + 0xb4) /* UART Test Register */
#define IMX_UMCR(base)        (base + 0xb8) /* UART Mode Control Register */

#define IMX_URXD_DATA         /* Received Data */
#define IMX_URXD_DATA_MASK    0xffU
#define IMX_URXD_DATA_SHIFT   0

#define IMX_UTXD_DATA         /* Transmit Data */
#define IMX_UTXD_DATA_MASK    0xffU
#define IMX_UTXD_DATA_SHIFT   0
#define IMX_URXD_CHARRDY      (1U << 15) /* Character Ready */
#define IMX_URXD_ERR          (1U << 14) /* Error Detect */
#define IMX_URXD_OVRRUN       (1U << 13) /* Receiver Overrun */
#define IMX_URXD_FRMERR       (1U << 12) /* Frame Error */
#define IMX_URXD_BRK          (1U << 11) /* BREAK Detect */
#define IMX_URXD_PRERR        (1U << 10) /* Parity Error */

#define IMX_UCR1_ICD          /* Idle Condition Detect */
#define IMX_UCR1_ICD_MASK     0b11U
#define IMX_UCR1_ICD_SHIFT    10
#define IMX_UCR1_ADEN         (1U << 15) /* Automatic Baud Rate Detection Interrupt Enable */
#define IMX_UCR1_ADBR         (1U << 14) /* Automatic Detection of Baud Rate */
#define IMX_UCR1_TRDYEN       (1U << 13) /* Transmitter Ready Interrupt Enable */
#define IMX_UCR1_IDEN         (1U << 12) /* Idle Condition Detected Interrupt Enable */
#define IMX_UCR1_RRDYEN       (1U << 9)  /* Receiver Ready Interrupt Enable */
#define IMX_UCR1_RXDMAEN      (1U << 8)  /* Receive Ready DMA Enable */
#define IMX_UCR1_IREN         (1U << 7)  /* Infrared Interface Enable */
#define IMX_UCR1_TXMPTYEN     (1U << 6)  /* Transmitter Empty Interrupt Enable */
#define IMX_UCR1_RTSDEN       (1U << 5)  /* RTS Delta Interrupt Enable */
#define IMX_UCR1_SNDBRK       (1U << 4)  /* Send BREAK */
#define IMX_UCR1_TXDMAEN      (1U << 3)  /* Transmitter Ready DMA Enable */
#define IMX_UCR1_ATDMAEN      (1U << 2)  /* Aging DMA Timer Enable */
#define IMX_UCR1_DOZE         (1U << 1)  /* The UART is disabled when in DOZE state */
#define IMX_UCR1_UARTEN       (1U << 0)  /* UART Enable */

#define IMX_UCR2_RTEC         /* Request to Send Edge Control */
#define IMX_UCR2_RTEC_MASK    0b11U
#define IMX_UCR2_RTEC_SHIFT   9
#define IMX_UCR2_ESCI         (1U << 15) /* Escape Sequence Interrupt Enable */
#define IMX_UCR2_IRTS         (1U << 14) /* Ignore RTS Pin */
#define IMX_UCR2_CTSC         (1U << 13) /* CTS Pin Control */
#define IMX_UCR2_CTS          (1U << 12) /* Clear to Send */
#define IMX_UCR2_ESCEN        (1U << 11) /* Escape Enable */
#define IMX_UCR2_PREN         (1U << 8)  /* Parity Enable */
#define IMX_UCR2_PROE         (1U << 7)  /* Parity Odd/Even */
#define IMX_UCR2_STPB         (1U << 6)  /* Controls the number of stop bits */
#define IMX_UCR2_WS           (1U << 5)  /* Word Size (7/8-bit) */
#define IMX_UCR2_RTSEN        (1U << 4)  /* Request to Send Interrupt Enable */
#define IMX_UCR2_ATEN         (1U << 3)  /* Aging Timer Enable */
#define IMX_UCR2_TXEN         (1U << 2)  /* Transmitter Enable */
#define IMX_UCR2_RXEN         (1U << 1)  /* Receiver Enable */
#define IMX_UCR2_SRST         (1U << 0)  /* Disable Software Reset */

#define IMX_UCR3_DPEC         /* DTR/DSR Interrupt Edge Control */
#define IMX_UCR3_DPEC_MASK    0b11U
#define IMX_UCR3_DPEC_SHIFT   14
#define IMX_UCR3_ESCI         (1U << 13) /* Data Terminal Ready Interrupt Enable */
#define IMX_UCR3_PARERREN     (1U << 12) /* Parity Error Interrupt Enable */
#define IMX_UCR3_FRAERREN     (1U << 11) /* Frame Error Interrupt Enable */
#define IMX_UCR3_DSR          (1U << 10) /* Data Set Ready */
#define IMX_UCR3_DCD          (1U << 9)  /* Data Carrier Detect */
#define IMX_UCR3_RI           (1U << 8)  /* Ring Indicator */
#define IMX_UCR3_ADNIMP       (1U << 7)  /* Autobaud Detection Not Improved */
#define IMX_UCR3_RXDSEN       (1U << 6)  /* Receive Status Interrupt Enable */
#define IMX_UCR3_AIRINTEN     (1U << 5)  /* Asynchronous IR WAKE Interrupt Enable */
#define IMX_UCR3_AWAKEN       (1U << 4)  /* Asynchronous WAKE Interrupt Enable */
#define IMX_UCR3_DTRDEN       (1U << 3)  /* Data Terminal Ready Delta Enable */
#define IMX_UCR3_RXDMUXSEL    (1U << 2)  /* RXD Muxed Input Selected */
#define IMX_UCR3_INVT         (1U << 1)  /* Invert TXD output */
#define IMX_UCR3_ACIEN        (1U << 0)  /* Autobaud Counter Interrupt Enable */

#define IMX_UCR4_CTSTL        /* CTS Trigger Level */
#define IMX_UCR4_CTSTL_MASK   0b111111U
#define IMX_UCR4_CTSTL_SHIFT  10
#define IMX_UCR4_INVR         (1U << 9) /* Invert RXD input */
#define IMX_UCR4_ENIRI        (1U << 8) /* Serial Infrared Interrupt Enable */
#define IMX_UCR4_WKEN         (1U << 7) /* WAKE Interrupt Enable */
#define IMX_UCR4_IDDMAEN      (1U << 6) /* DMA IDLE Condition Detected Interrupt Enable */
#define IMX_UCR4_IRSC         (1U << 5) /* IR Special Case */
#define IMX_UCR4_LPBYP        (1U << 4) /* Low Power Bypass */
#define IMX_UCR4_TCEN         (1U << 3) /* TransmitComplete Interrupt Enable */
#define IMX_UCR4_BKEN         (1U << 2) /* BREAK Condition Detected Interrupt Enable */
#define IMX_UCR4_OREN         (1U << 1) /* Receiver Overrun Interrupt Enable */
#define IMX_UCR4_DREN         (1U << 0) /* Receive Data Ready Interrupt Enable */

#define IMX_UFCR_TXTL         /* Transmitter Trigger Level */
#define IMX_UFCR_TXTL_MASK    0b111111U
#define IMX_UFCR_TXTL_SHIFT   10
#define IMX_UFCR_RFDIV        /* Reference Frequency Divider */
#define IMX_UFCR_RFDIV_MASK   0b111U
#define IMX_UFCR_RFDIV_SHIFT  7
#define IMX_UFCR_RFDIV_1      0b101
#define IMX_UFCR_RFDIV_2      0b100
#define IMX_UFCR_RFDIV_3      0b011
#define IMX_UFCR_RFDIV_4      0b010
#define IMX_UFCR_RFDIV_5      0b001
#define IMX_UFCR_RFDIV_6      0b000
#define IMX_UFCR_RFDIV_7      0b110
#define IMX_UFCR_RXTL         /* Receiver Trigger Level */
#define IMX_UFCR_RXTL_MASK    0b111111U
#define IMX_UFCR_RXTL_SHIFT   0
#define IMX_UFCR_DCEDTE       (1U << 6) /* DCE/DTE mode select */

#define IMX_USR1_PARITYERR    (1U << 15) /* Parity Error Interrupt Flag */
#define IMX_USR1_RTSS         (1U << 14) /* RTS_B Pin Status */
#define IMX_USR1_TRDY         (1U << 13) /* Transmitter Ready Interrupt/DMA Flag */
#define IMX_USR1_RTSD         (1U << 12) /* RTS Delta */
#define IMX_USR1_ESCF         (1U << 11) /* Escape Sequence Interrupt Flag */
#define IMX_USR1_FRAMERR      (1U << 10) /* Frame Error Interrupt Flag */
#define IMX_USR1_RRDY         (1U << 9)  /* Receiver Ready Interrupt/DMA Flag */
#define IMX_USR1_AGTIM        (1U << 8)  /* Ageing Timer Interrupt Flag */
#define IMX_USR1_DTRD         (1U << 7)  /* DTR Delta */
#define IMX_USR1_RXDS         (1U << 6)  /* Receiver IDLE Interrupt Flag */
#define IMX_USR1_AIRINT       (1U << 5)  /* Asynchronous IR WAKE Interrupt Flag */
#define IMX_USR1_AWAKE        (1U << 4)  /* Asynchronous WAKE Interrupt Flag */
#define IMX_USR1_SAD          (1U << 3)  /* Slave Address Detected Interrupt Flag */

#define IMX_USR2_ADET         (1U << 15) /* Automatic Baud Rate Detect Complete */
#define IMX_USR2_TXFE         (1U << 14) /* Transmit Buffer FIFO Empty */
#define IMX_USR2_DTRF         (1U << 13) /* DTR edge triggered interrupt flag */
#define IMX_USR2_IDLE         (1U << 12) /* Idle Condition */
#define IMX_USR2_ACST         (1U << 11) /* Autobaud Counter Stopped */
#define IMX_USR2_RIDELT       (1U << 10) /* Ring Indicator Delta */
#define IMX_USR2_RIIN         (1U << 9)  /* Ring Indicator Input */
#define IMX_USR2_IRINT        (1U << 8)  /* Serial Infrared Interrupt Flag */
#define IMX_USR2_WAKE         (1U << 7)  /* Indicates the start bit is detected */
#define IMX_USR2_DCDDELT      (1U << 6)  /* Data Carrier Detect Delta */
#define IMX_USR2_DCDIN        (1U << 5)  /* Data Carrier Detect Input */
#define IMX_USR2_RTSF         (1U << 4)  /* RTS Edge Triggered Interrupt Flag */
#define IMX_USR2_TXDC         (1U << 3)  /* Transmitter Complete */
#define IMX_USR2_BRCD         (1U << 2)  /* BREAK Condition Detected */
#define IMX_USR2_ORE          (1U << 1)  /* Overrun Error */
#define IMX_USR2_RDR          (1U << 0)  /* Receive Data Ready */

#define IMX_UESC_ESCC         /* UART Escape Character */
#define IMX_UESC_ESCC_MASK    0xffU
#define IMX_UESC_ESCC_SHIFT   0

#define IMX_UTIM_TIM          /* UART Escape Timer */
#define IMX_UTIM_TIM_MASK     0xfffU
#define IMX_UTIM_TIM_SHIFT    0

#define IMX_UBIR_INC          /* Incremental Numerator */
#define IMX_UBIR_INC_MASK     0xffffU
#define IMX_UBIR_INC_SHIFT    0

#define IMX_UBMR_MOD          /* Modulator Denominator */
#define IMX_UBMR_MOD_MASK     0xffffU
#define IMX_UBMR_MOD_SHIFT    0

#define IMX_UBRC_BCNT         /* Baud Rate Count Register */
#define IMX_UBRC_BCNT_MASK    0xffffU
#define IMX_UBRC_BCNT_SHIFT   0

#define IMX_ONEMS_ONEMS       /* One Millisecond Register */
#define IMX_ONEMS_ONEMS_MASK  0xffffffU
#define IMX_ONEMS_ONEMS_SHIFT 0

#define IMX_UTS_FRCPERR       (1U << 13) /* Force Parity Error */
#define IMX_UTS_LOOP          (1U << 12) /* Loop TX and RX for Test */
#define IMX_UTS_DBGEN         (1U << 11) /* Respond to the debug_req Input Signal */
#define IMX_UTS_LOOPIR        (1U << 10) /* Loop TX and RX for IR Test */
#define IMX_UTS_RXDBG         (1U << 9)  /* rx_fifo Read Pointer Increments */
#define IMX_UTS_TXEMPTY       (1U << 6)  /* TxFIFO Empty */
#define IMX_UTS_RXEMPTY       (1U << 5)  /* RxFIFO Empty */
#define IMX_UTS_TXFULL        (1U << 4)  /* TxFIFO FULL */
#define IMX_UTS_RXFULL        (1U << 3)  /* RxFIFO FULL */
#define IMX_UTS_SOFTRST       (1U << 0)  /* Software Reset Active */

#define IMX_UMCR_SLADDR       /* Slave Address Character */
#define IMX_UMCR_SLADDR_MASK  0xffU
#define IMX_UMCR_SLADDR_SHIFT 8
#define IMX_UMCR_SADEN        (1U << 3) /* Slave Address Detected Interrupt Enable */
#define IMX_UMCR_TXB8         (1U << 2) /* Transmit bit 8 */
#define IMX_UMCR_SLAM         (1U << 1) /* Slave Address Detect Mode Selection */
#define IMX_UMCR_MDEN         (1U << 0) /* 9-bit data or Multidrop Mode Enable */

/* clang-format on */

#endif /* DRIVERS_SERIAL_IMX_UART_IMX_UART_REGS_H_ */
