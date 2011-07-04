/**
 * @file
 * @brief USART Controller interface
 *
 * @date 26.09.10
 * @author Anton Kozlov
 */

#ifndef AT91SAM7_US_H_
#define AT91SAM7_US_H_

#include <types.h>

typedef struct _AT91S_USART {
	at91_reg_t	 US_CR; 	// Control Register
	at91_reg_t	 US_MR; 	// Mode Register
	at91_reg_t	 US_IER; 	// Interrupt Enable Register
	at91_reg_t	 US_IDR; 	// Interrupt Disable Register
	at91_reg_t	 US_IMR; 	// Interrupt Mask Register
	at91_reg_t	 US_CSR; 	// Channel Status Register
	at91_reg_t	 US_RHR; 	// Receiver Holding Register
	at91_reg_t	 US_THR; 	// Transmitter Holding Register
	at91_reg_t	 US_BRGR; 	// Baud Rate Generator Register
	at91_reg_t	 US_RTOR; 	// Receiver Time-out Register
	at91_reg_t	 US_TTGR; 	// Transmitter Time-guard Register
	at91_reg_t	 Reserved0[5]; 	//
	at91_reg_t	 US_FIDI; 	// FI_DI_Ratio Register
	at91_reg_t	 US_NER; 	// Nb Errors Register
	at91_reg_t	 Reserved1[1]; 	//
	at91_reg_t	 US_IF; 	// IRDA_FILTER Register
	at91_reg_t	 Reserved2[44]; 	//
	at91_reg_t	 US_RPR; 	// Receive Pointer Register
	at91_reg_t	 US_RCR; 	// Receive Counter Register
	at91_reg_t	 US_TPR; 	// Transmit Pointer Register
	at91_reg_t	 US_TCR; 	// Transmit Counter Register
	at91_reg_t	 US_RNPR; 	// Receive Next Pointer Register
	at91_reg_t	 US_RNCR; 	// Receive Next Counter Register
	at91_reg_t	 US_TNPR; 	// Transmit Next Pointer Register
	at91_reg_t	 US_TNCR; 	// Transmit Next Counter Register
	at91_reg_t	 US_PTCR; 	// PDC Transfer Control Register
	at91_reg_t	 US_PTSR; 	// PDC Transfer Status Register

} AT91S_USART, *AT91PS_USART;

// -------- US_CR : (USART Offset: 0x0) Debug Unit Control Register --------
#define AT91C_US_STTBRK       ((unsigned int) 0x1 <<  9) // (USART) Start Break
#define AT91C_US_STPBRK       ((unsigned int) 0x1 << 10) // (USART) Stop Break
#define AT91C_US_STTTO        ((unsigned int) 0x1 << 11) // (USART) Start Time-out
#define AT91C_US_SENDA        ((unsigned int) 0x1 << 12) // (USART) Send Address
#define AT91C_US_RSTIT        ((unsigned int) 0x1 << 13) // (USART) Reset Iterations
#define AT91C_US_RSTNACK      ((unsigned int) 0x1 << 14) // (USART) Reset Non Acknowledge
#define AT91C_US_RETTO        ((unsigned int) 0x1 << 15) // (USART) Rearm Time-out
#define AT91C_US_DTREN        ((unsigned int) 0x1 << 16) // (USART) Data Terminal ready Enable
#define AT91C_US_DTRDIS       ((unsigned int) 0x1 << 17) // (USART) Data Terminal ready Disable
#define AT91C_US_RTSEN        ((unsigned int) 0x1 << 18) // (USART) Request to Send enable
#define AT91C_US_RTSDIS       ((unsigned int) 0x1 << 19) // (USART) Request to Send Disable
// -------- US_MR : (USART Offset: 0x4) Debug Unit Mode Register --------
#define AT91C_US_USMODE       ((unsigned int) 0xF <<  0) // (USART) Usart mode
#define 	AT91C_US_USMODE_NORMAL               ((unsigned int) 0x0) // (USART) Normal
#define 	AT91C_US_USMODE_RS485                ((unsigned int) 0x1) // (USART) RS485
#define 	AT91C_US_USMODE_HWHSH                ((unsigned int) 0x2) // (USART) Hardware Handshaking
#define 	AT91C_US_USMODE_MODEM                ((unsigned int) 0x3) // (USART) Modem
#define 	AT91C_US_USMODE_ISO7816_0            ((unsigned int) 0x4) // (USART) ISO7816 protocol: T = 0
#define 	AT91C_US_USMODE_ISO7816_1            ((unsigned int) 0x6) // (USART) ISO7816 protocol: T = 1
#define 	AT91C_US_USMODE_IRDA                 ((unsigned int) 0x8) // (USART) IrDA
#define 	AT91C_US_USMODE_SWHSH                ((unsigned int) 0xC) // (USART) Software Handshaking
#define AT91C_US_CLKS         ((unsigned int) 0x3 <<  4) // (USART) Clock Selection (Baud Rate generator Input Clock
#define 	AT91C_US_CLKS_CLOCK                ((unsigned int) 0x0 <<  4) // (USART) Clock
#define 	AT91C_US_CLKS_FDIV1                ((unsigned int) 0x1 <<  4) // (USART) fdiv1
#define 	AT91C_US_CLKS_SLOW                 ((unsigned int) 0x2 <<  4) // (USART) slow_clock (ARM)
#define 	AT91C_US_CLKS_EXT                  ((unsigned int) 0x3 <<  4) // (USART) External (SCK)
#define AT91C_US_CHRL         ((unsigned int) 0x3 <<  6) // (USART) Clock Selection (Baud Rate generator Input Clock
#define 	AT91C_US_CHRL_5_BITS               ((unsigned int) 0x0 <<  6) // (USART) Character Length: 5 bits
#define 	AT91C_US_CHRL_6_BITS               ((unsigned int) 0x1 <<  6) // (USART) Character Length: 6 bits
#define 	AT91C_US_CHRL_7_BITS               ((unsigned int) 0x2 <<  6) // (USART) Character Length: 7 bits
#define 	AT91C_US_CHRL_8_BITS               ((unsigned int) 0x3 <<  6) // (USART) Character Length: 8 bits
#define AT91C_US_SYNC         ((unsigned int) 0x1 <<  8) // (USART) Synchronous Mode Select
#define AT91C_US_NBSTOP       ((unsigned int) 0x3 << 12) // (USART) Number of Stop bits
#define 	AT91C_US_NBSTOP_1_BIT                ((unsigned int) 0x0 << 12) // (USART) 1 stop bit
#define 	AT91C_US_NBSTOP_15_BIT               ((unsigned int) 0x1 << 12) // (USART) Asynchronous (SYNC=0) 2 stop bits Synchronous (SYNC=1) 2 stop bits
#define 	AT91C_US_NBSTOP_2_BIT                ((unsigned int) 0x2 << 12) // (USART) 2 stop bits
#define AT91C_US_MSBF         ((unsigned int) 0x1 << 16) // (USART) Bit Order
#define AT91C_US_MODE9        ((unsigned int) 0x1 << 17) // (USART) 9-bit Character length
#define AT91C_US_CKLO         ((unsigned int) 0x1 << 18) // (USART) Clock Output Select
#define AT91C_US_OVER         ((unsigned int) 0x1 << 19) // (USART) Over Sampling Mode
#define AT91C_US_INACK        ((unsigned int) 0x1 << 20) // (USART) Inhibit Non Acknowledge
#define AT91C_US_DSNACK       ((unsigned int) 0x1 << 21) // (USART) Disable Successive NACK
#define AT91C_US_MAX_ITER     ((unsigned int) 0x1 << 24) // (USART) Number of Repetitions
#define AT91C_US_FILTER       ((unsigned int) 0x1 << 28) // (USART) Receive Line Filter

// -------- US_IER : (USART Offset: 0x8) Debug Unit Interrupt Enable Register --------
#define AT91C_US_RXBRK        ((unsigned int) 0x1 <<  2) // (USART) Break Received/End of Break
#define AT91C_US_TIMEOUT      ((unsigned int) 0x1 <<  8) // (USART) Receiver Time-out
#define AT91C_US_ITERATION    ((unsigned int) 0x1 << 10) // (USART) Max number of Repetitions Reached
#define AT91C_US_NACK         ((unsigned int) 0x1 << 13) // (USART) Non Acknowledge
#define AT91C_US_RIIC         ((unsigned int) 0x1 << 16) // (USART) Ring INdicator Input Change Flag
#define AT91C_US_DSRIC        ((unsigned int) 0x1 << 17) // (USART) Data Set Ready Input Change Flag
#define AT91C_US_DCDIC        ((unsigned int) 0x1 << 18) // (USART) Data Carrier Flag
#define AT91C_US_CTSIC        ((unsigned int) 0x1 << 19) // (USART) Clear To Send Input Change Flag
// -------- US_IDR : (USART Offset: 0xc) Debug Unit Interrupt Disable Register --------
// -------- US_IMR : (USART Offset: 0x10) Debug Unit Interrupt Mask Register --------
// -------- US_CSR : (USART Offset: 0x14) Debug Unit Channel Status Register --------
#define AT91C_US_RI           ((unsigned int) 0x1 << 20) // (USART) Image of RI Input
#define AT91C_US_DSR          ((unsigned int) 0x1 << 21) // (USART) Image of DSR Input
#define AT91C_US_DCD          ((unsigned int) 0x1 << 22) // (USART) Image of DCD Input
#define AT91C_US_CTS          ((unsigned int) 0x1 << 23) // (USART) Image of CTS Input

// ========== Register definition for PDC_US1 peripheral ==========
#define AT91C_US1_RNCR  ((at91_reg_t *) 	0xFFFC4114) // (PDC_US1) Receive Next Counter Register
#define AT91C_US1_PTCR  ((at91_reg_t *) 	0xFFFC4120) // (PDC_US1) PDC Transfer Control Register
#define AT91C_US1_TCR   ((at91_reg_t *) 	0xFFFC410C) // (PDC_US1) Transmit Counter Register
#define AT91C_US1_PTSR  ((at91_reg_t *) 	0xFFFC4124) // (PDC_US1) PDC Transfer Status Register
#define AT91C_US1_TNPR  ((at91_reg_t *) 	0xFFFC4118) // (PDC_US1) Transmit Next Pointer Register
#define AT91C_US1_RCR   ((at91_reg_t *) 	0xFFFC4104) // (PDC_US1) Receive Counter Register
#define AT91C_US1_RNPR  ((at91_reg_t *) 	0xFFFC4110) // (PDC_US1) Receive Next Pointer Register
#define AT91C_US1_RPR   ((at91_reg_t *) 	0xFFFC4100) // (PDC_US1) Receive Pointer Register
#define AT91C_US1_TNCR  ((at91_reg_t *) 	0xFFFC411C) // (PDC_US1) Transmit Next Counter Register
#define AT91C_US1_TPR   ((at91_reg_t *) 	0xFFFC4108) // (PDC_US1) Transmit Pointer Register
// ========== Register definition for US1 peripheral ==========
#define AT91C_US1_IF    ((at91_reg_t *) 	0xFFFC404C) // (US1) IRDA_FILTER Register
#define AT91C_US1_NER   ((at91_reg_t *) 	0xFFFC4044) // (US1) Nb Errors Register
#define AT91C_US1_RTOR  ((at91_reg_t *) 	0xFFFC4024) // (US1) Receiver Time-out Register
#define AT91C_US1_CSR   ((at91_reg_t *) 	0xFFFC4014) // (US1) Channel Status Register
#define AT91C_US1_IDR   ((at91_reg_t *) 	0xFFFC400C) // (US1) Interrupt Disable Register
#define AT91C_US1_IER   ((at91_reg_t *) 	0xFFFC4008) // (US1) Interrupt Enable Register
#define AT91C_US1_THR   ((at91_reg_t *) 	0xFFFC401C) // (US1) Transmitter Holding Register
#define AT91C_US1_TTGR  ((at91_reg_t *) 	0xFFFC4028) // (US1) Transmitter Time-guard Register
#define AT91C_US1_RHR   ((at91_reg_t *) 	0xFFFC4018) // (US1) Receiver Holding Register
#define AT91C_US1_BRGR  ((at91_reg_t *) 	0xFFFC4020) // (US1) Baud Rate Generator Register
#define AT91C_US1_IMR   ((at91_reg_t *) 	0xFFFC4010) // (US1) Interrupt Mask Register
#define AT91C_US1_FIDI  ((at91_reg_t *) 	0xFFFC4040) // (US1) FI_DI_Ratio Register
#define AT91C_US1_CR    ((at91_reg_t *) 	0xFFFC4000) // (US1) Control Register
#define AT91C_US1_MR    ((at91_reg_t *) 	0xFFFC4004) // (US1) Mode Register
// ========== Register definition for PDC_US0 peripheral ==========
#define AT91C_US0_TNPR  ((at91_reg_t *) 	0xFFFC0118) // (PDC_US0) Transmit Next Pointer Register
#define AT91C_US0_RNPR  ((at91_reg_t *) 	0xFFFC0110) // (PDC_US0) Receive Next Pointer Register
#define AT91C_US0_TCR   ((at91_reg_t *) 	0xFFFC010C) // (PDC_US0) Transmit Counter Register
#define AT91C_US0_PTCR  ((at91_reg_t *) 	0xFFFC0120) // (PDC_US0) PDC Transfer Control Register
#define AT91C_US0_PTSR  ((at91_reg_t *) 	0xFFFC0124) // (PDC_US0) PDC Transfer Status Register
#define AT91C_US0_TNCR  ((at91_reg_t *) 	0xFFFC011C) // (PDC_US0) Transmit Next Counter Register
#define AT91C_US0_TPR   ((at91_reg_t *) 	0xFFFC0108) // (PDC_US0) Transmit Pointer Register
#define AT91C_US0_RCR   ((at91_reg_t *) 	0xFFFC0104) // (PDC_US0) Receive Counter Register
#define AT91C_US0_RPR   ((at91_reg_t *) 	0xFFFC0100) // (PDC_US0) Receive Pointer Register
#define AT91C_US0_RNCR  ((at91_reg_t *) 	0xFFFC0114) // (PDC_US0) Receive Next Counter Register
// ========== Register definition for US0 peripheral ==========
#define AT91C_US0_BRGR  ((at91_reg_t *) 	0xFFFC0020) // (US0) Baud Rate Generator Register
#define AT91C_US0_NER   ((at91_reg_t *) 	0xFFFC0044) // (US0) Nb Errors Register
#define AT91C_US0_CR    ((at91_reg_t *) 	0xFFFC0000) // (US0) Control Register
#define AT91C_US0_IMR   ((at91_reg_t *) 	0xFFFC0010) // (US0) Interrupt Mask Register
#define AT91C_US0_FIDI  ((at91_reg_t *) 	0xFFFC0040) // (US0) FI_DI_Ratio Register
#define AT91C_US0_TTGR  ((at91_reg_t *) 	0xFFFC0028) // (US0) Transmitter Time-guard Register
#define AT91C_US0_MR    ((at91_reg_t *) 	0xFFFC0004) // (US0) Mode Register
#define AT91C_US0_RTOR  ((at91_reg_t *) 	0xFFFC0024) // (US0) Receiver Time-out Register
#define AT91C_US0_CSR   ((at91_reg_t *) 	0xFFFC0014) // (US0) Channel Status Register
#define AT91C_US0_RHR   ((at91_reg_t *) 	0xFFFC0018) // (US0) Receiver Holding Register
#define AT91C_US0_IDR   ((at91_reg_t *) 	0xFFFC000C) // (US0) Interrupt Disable Register
#define AT91C_US0_THR   ((at91_reg_t *) 	0xFFFC001C) // (US0) Transmitter Holding Register
#define AT91C_US0_IF    ((at91_reg_t *) 	0xFFFC004C) // (US0) IRDA_FILTER Register
#define AT91C_US0_IER   ((at91_reg_t *) 	0xFFFC0008) // (US0) Interrupt Enable Register

typedef struct _AT91S_DBGU {
	at91_reg_t	 DBGU_CR; 	// Control Register
	at91_reg_t	 DBGU_MR; 	// Mode Register
	at91_reg_t	 DBGU_IER; 	// Interrupt Enable Register
	at91_reg_t	 DBGU_IDR; 	// Interrupt Disable Register
	at91_reg_t	 DBGU_IMR; 	// Interrupt Mask Register
	at91_reg_t	 DBGU_CSR; 	// Channel Status Register
	at91_reg_t	 DBGU_RHR; 	// Receiver Holding Register
	at91_reg_t	 DBGU_THR; 	// Transmitter Holding Register
	at91_reg_t	 DBGU_BRGR; 	// Baud Rate Generator Register
	at91_reg_t	 Reserved0[7]; 	//
	at91_reg_t	 DBGU_CIDR; 	// Chip ID Register
	at91_reg_t	 DBGU_EXID; 	// Chip ID Extension Register
	at91_reg_t	 DBGU_FNTR; 	// Force NTRST Register
	at91_reg_t	 Reserved1[45]; 	//
	at91_reg_t	 DBGU_RPR; 	// Receive Pointer Register
	at91_reg_t	 DBGU_RCR; 	// Receive Counter Register
	at91_reg_t	 DBGU_TPR; 	// Transmit Pointer Register
	at91_reg_t	 DBGU_TCR; 	// Transmit Counter Register
	at91_reg_t	 DBGU_RNPR; 	// Receive Next Pointer Register
	at91_reg_t	 DBGU_RNCR; 	// Receive Next Counter Register
	at91_reg_t	 DBGU_TNPR; 	// Transmit Next Pointer Register
	at91_reg_t	 DBGU_TNCR; 	// Transmit Next Counter Register
	at91_reg_t	 DBGU_PTCR; 	// PDC Transfer Control Register
	at91_reg_t	 DBGU_PTSR; 	// PDC Transfer Status Register
} AT91S_DBGU, *AT91PS_DBGU;

// -------- DBGU_CR : (DBGU Offset: 0x0) Debug Unit Control Register --------
#define AT91C_US_RSTRX        ((unsigned int) 0x1 <<  2) // (DBGU) Reset Receiver
#define AT91C_US_RSTTX        ((unsigned int) 0x1 <<  3) // (DBGU) Reset Transmitter
#define AT91C_US_RXEN         ((unsigned int) 0x1 <<  4) // (DBGU) Receiver Enable
#define AT91C_US_RXDIS        ((unsigned int) 0x1 <<  5) // (DBGU) Receiver Disable
#define AT91C_US_TXEN         ((unsigned int) 0x1 <<  6) // (DBGU) Transmitter Enable
#define AT91C_US_TXDIS        ((unsigned int) 0x1 <<  7) // (DBGU) Transmitter Disable
#define AT91C_US_RSTSTA       ((unsigned int) 0x1 <<  8) // (DBGU) Reset Status Bits
// -------- DBGU_MR : (DBGU Offset: 0x4) Debug Unit Mode Register --------
#define AT91C_US_PAR          ((unsigned int) 0x7 <<  9) // (DBGU) Parity type
#define 	AT91C_US_PAR_EVEN                 ((unsigned int) 0x0 <<  9) // (DBGU) Even Parity
#define 	AT91C_US_PAR_ODD                  ((unsigned int) 0x1 <<  9) // (DBGU) Odd Parity
#define 	AT91C_US_PAR_SPACE                ((unsigned int) 0x2 <<  9) // (DBGU) Parity forced to 0 (Space)
#define 	AT91C_US_PAR_MARK                 ((unsigned int) 0x3 <<  9) // (DBGU) Parity forced to 1 (Mark)
#define 	AT91C_US_PAR_NONE                 ((unsigned int) 0x4 <<  9) // (DBGU) No Parity
#define 	AT91C_US_PAR_MULTI_DROP           ((unsigned int) 0x6 <<  9) // (DBGU) Multi-drop mode
#define AT91C_US_CHMODE       ((unsigned int) 0x3 << 14) // (DBGU) Channel Mode
#define 	AT91C_US_CHMODE_NORMAL               ((unsigned int) 0x0 << 14) // (DBGU) Normal Mode: The USART channel operates as an RX/TX USART.
#define 	AT91C_US_CHMODE_AUTO                 ((unsigned int) 0x1 << 14) // (DBGU) Automatic Echo: Receiver Data Input is connected to the TXD pin.
#define 	AT91C_US_CHMODE_LOCAL                ((unsigned int) 0x2 << 14) // (DBGU) Local Loopback: Transmitter Output Signal is connected to Receiver Input Signal.
#define 	AT91C_US_CHMODE_REMOTE               ((unsigned int) 0x3 << 14) // (DBGU) Remote Loopback: RXD pin is internally connected to TXD pin.
// -------- DBGU_IER : (DBGU Offset: 0x8) Debug Unit Interrupt Enable Register --------
#define AT91C_US_RXRDY        ((unsigned int) 0x1 <<  0) // (DBGU) RXRDY Interrupt
#define AT91C_US_TXRDY        ((unsigned int) 0x1 <<  1) // (DBGU) TXRDY Interrupt
#define AT91C_US_ENDRX        ((unsigned int) 0x1 <<  3) // (DBGU) End of Receive Transfer Interrupt
#define AT91C_US_ENDTX        ((unsigned int) 0x1 <<  4) // (DBGU) End of Transmit Interrupt
#define AT91C_US_OVRE         ((unsigned int) 0x1 <<  5) // (DBGU) Overrun Interrupt
#define AT91C_US_FRAME        ((unsigned int) 0x1 <<  6) // (DBGU) Framing Error Interrupt
#define AT91C_US_PARE         ((unsigned int) 0x1 <<  7) // (DBGU) Parity Error Interrupt
#define AT91C_US_TXEMPTY      ((unsigned int) 0x1 <<  9) // (DBGU) TXEMPTY Interrupt
#define AT91C_US_TXBUFE       ((unsigned int) 0x1 << 11) // (DBGU) TXBUFE Interrupt
#define AT91C_US_RXBUFF       ((unsigned int) 0x1 << 12) // (DBGU) RXBUFF Interrupt
#define AT91C_US_COMM_TX      ((unsigned int) 0x1 << 30) // (DBGU) COMM_TX Interrupt
#define AT91C_US_COMM_RX      ((unsigned int) 0x1 << 31) // (DBGU) COMM_RX Interrupt
// -------- DBGU_IDR : (DBGU Offset: 0xc) Debug Unit Interrupt Disable Register --------
// -------- DBGU_IMR : (DBGU Offset: 0x10) Debug Unit Interrupt Mask Register --------
// -------- DBGU_CSR : (DBGU Offset: 0x14) Debug Unit Channel Status Register --------
// -------- DBGU_FNTR : (DBGU Offset: 0x48) Debug Unit FORCE_NTRST Register --------
#define AT91C_US_FORCE_NTRST  ((unsigned int) 0x1 <<  0) // (DBGU) Force NTRST in JTAG

#endif /* AT91SAM7_US_H_ */
