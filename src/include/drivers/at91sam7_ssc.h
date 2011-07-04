
#ifndef AT91SAM7_SSC_H_
#define AT91SAM7_SSC_H_

typedef struct _AT91S_SSC {
	at91_reg_t	 SSC_CR; 	// Control Register
	at91_reg_t	 SSC_CMR; 	// Clock Mode Register
	at91_reg_t	 Reserved0[2]; 	//
	at91_reg_t	 SSC_RCMR; 	// Receive Clock ModeRegister
	at91_reg_t	 SSC_RFMR; 	// Receive Frame Mode Register
	at91_reg_t	 SSC_TCMR; 	// Transmit Clock Mode Register
	at91_reg_t	 SSC_TFMR; 	// Transmit Frame Mode Register
	at91_reg_t	 SSC_RHR; 	// Receive Holding Register
	at91_reg_t	 SSC_THR; 	// Transmit Holding Register
	at91_reg_t	 Reserved1[2]; 	//
	at91_reg_t	 SSC_RSHR; 	// Receive Sync Holding Register
	at91_reg_t	 SSC_TSHR; 	// Transmit Sync Holding Register
	at91_reg_t	 Reserved2[2]; 	//
	at91_reg_t	 SSC_SR; 	// Status Register
	at91_reg_t	 SSC_IER; 	// Interrupt Enable Register
	at91_reg_t	 SSC_IDR; 	// Interrupt Disable Register
	at91_reg_t	 SSC_IMR; 	// Interrupt Mask Register
	at91_reg_t	 Reserved3[44]; 	//
	at91_reg_t	 SSC_RPR; 	// Receive Pointer Register
	at91_reg_t	 SSC_RCR; 	// Receive Counter Register
	at91_reg_t	 SSC_TPR; 	// Transmit Pointer Register
	at91_reg_t	 SSC_TCR; 	// Transmit Counter Register
	at91_reg_t	 SSC_RNPR; 	// Receive Next Pointer Register
	at91_reg_t	 SSC_RNCR; 	// Receive Next Counter Register
	at91_reg_t	 SSC_TNPR; 	// Transmit Next Pointer Register
	at91_reg_t	 SSC_TNCR; 	// Transmit Next Counter Register
	at91_reg_t	 SSC_PTCR; 	// PDC Transfer Control Register
	at91_reg_t	 SSC_PTSR; 	// PDC Transfer Status Register
} AT91S_SSC, *AT91PS_SSC;

// -------- SSC_CR : (SSC Offset: 0x0) SSC Control Register --------
#define AT91C_SSC_RXEN        ((unsigned int) 0x1 <<  0) // (SSC) Receive Enable
#define AT91C_SSC_RXDIS       ((unsigned int) 0x1 <<  1) // (SSC) Receive Disable
#define AT91C_SSC_TXEN        ((unsigned int) 0x1 <<  8) // (SSC) Transmit Enable
#define AT91C_SSC_TXDIS       ((unsigned int) 0x1 <<  9) // (SSC) Transmit Disable
#define AT91C_SSC_SWRST       ((unsigned int) 0x1 << 15) // (SSC) Software Reset
// -------- SSC_RCMR : (SSC Offset: 0x10) SSC Receive Clock Mode Register --------
#define AT91C_SSC_CKS         ((unsigned int) 0x3 <<  0) // (SSC) Receive/Transmit Clock Selection
#define 	AT91C_SSC_CKS_DIV                  ((unsigned int) 0x0) // (SSC) Divided Clock
#define 	AT91C_SSC_CKS_TK                   ((unsigned int) 0x1) // (SSC) TK Clock signal
#define 	AT91C_SSC_CKS_RK                   ((unsigned int) 0x2) // (SSC) RK pin
#define AT91C_SSC_CKO         ((unsigned int) 0x7 <<  2) // (SSC) Receive/Transmit Clock Output Mode Selection
#define 	AT91C_SSC_CKO_NONE                 ((unsigned int) 0x0 <<  2) // (SSC) Receive/Transmit Clock Output Mode: None RK pin: Input-only
#define 	AT91C_SSC_CKO_CONTINOUS            ((unsigned int) 0x1 <<  2) // (SSC) Continuous Receive/Transmit Clock RK pin: Output
#define 	AT91C_SSC_CKO_DATA_TX              ((unsigned int) 0x2 <<  2) // (SSC) Receive/Transmit Clock only during data transfers RK pin: Output
#define AT91C_SSC_CKI         ((unsigned int) 0x1 <<  5) // (SSC) Receive/Transmit Clock Inversion
#define AT91C_SSC_START       ((unsigned int) 0xF <<  8) // (SSC) Receive/Transmit Start Selection
#define 	AT91C_SSC_START_CONTINOUS            ((unsigned int) 0x0 <<  8) // (SSC) Continuous, as soon as the receiver is enabled, and immediately after the end of transfer of the previous data.
#define 	AT91C_SSC_START_TX                   ((unsigned int) 0x1 <<  8) // (SSC) Transmit/Receive start
#define 	AT91C_SSC_START_LOW_RF               ((unsigned int) 0x2 <<  8) // (SSC) Detection of a low level on RF input
#define 	AT91C_SSC_START_HIGH_RF              ((unsigned int) 0x3 <<  8) // (SSC) Detection of a high level on RF input
#define 	AT91C_SSC_START_FALL_RF              ((unsigned int) 0x4 <<  8) // (SSC) Detection of a falling edge on RF input
#define 	AT91C_SSC_START_RISE_RF              ((unsigned int) 0x5 <<  8) // (SSC) Detection of a rising edge on RF input
#define 	AT91C_SSC_START_LEVEL_RF             ((unsigned int) 0x6 <<  8) // (SSC) Detection of any level change on RF input
#define 	AT91C_SSC_START_EDGE_RF              ((unsigned int) 0x7 <<  8) // (SSC) Detection of any edge on RF input
#define 	AT91C_SSC_START_0                    ((unsigned int) 0x8 <<  8) // (SSC) Compare 0
#define AT91C_SSC_STTDLY      ((unsigned int) 0xFF << 16) // (SSC) Receive/Transmit Start Delay
#define AT91C_SSC_PERIOD      ((unsigned int) 0xFF << 24) // (SSC) Receive/Transmit Period Divider Selection
// -------- SSC_RFMR : (SSC Offset: 0x14) SSC Receive Frame Mode Register --------
#define AT91C_SSC_DATLEN      ((unsigned int) 0x1F <<  0) // (SSC) Data Length
#define AT91C_SSC_LOOP        ((unsigned int) 0x1 <<  5) // (SSC) Loop Mode
#define AT91C_SSC_MSBF        ((unsigned int) 0x1 <<  7) // (SSC) Most Significant Bit First
#define AT91C_SSC_DATNB       ((unsigned int) 0xF <<  8) // (SSC) Data Number per Frame
#define AT91C_SSC_FSLEN       ((unsigned int) 0xF << 16) // (SSC) Receive/Transmit Frame Sync length
#define AT91C_SSC_FSOS        ((unsigned int) 0x7 << 20) // (SSC) Receive/Transmit Frame Sync Output Selection
#define 	AT91C_SSC_FSOS_NONE                 ((unsigned int) 0x0 << 20) // (SSC) Selected Receive/Transmit Frame Sync Signal: None RK pin Input-only
#define 	AT91C_SSC_FSOS_NEGATIVE             ((unsigned int) 0x1 << 20) // (SSC) Selected Receive/Transmit Frame Sync Signal: Negative Pulse
#define 	AT91C_SSC_FSOS_POSITIVE             ((unsigned int) 0x2 << 20) // (SSC) Selected Receive/Transmit Frame Sync Signal: Positive Pulse
#define 	AT91C_SSC_FSOS_LOW                  ((unsigned int) 0x3 << 20) // (SSC) Selected Receive/Transmit Frame Sync Signal: Driver Low during data transfer
#define 	AT91C_SSC_FSOS_HIGH                 ((unsigned int) 0x4 << 20) // (SSC) Selected Receive/Transmit Frame Sync Signal: Driver High during data transfer
#define 	AT91C_SSC_FSOS_TOGGLE               ((unsigned int) 0x5 << 20) // (SSC) Selected Receive/Transmit Frame Sync Signal: Toggling at each start of data transfer
#define AT91C_SSC_FSEDGE      ((unsigned int) 0x1 << 24) // (SSC) Frame Sync Edge Detection
// -------- SSC_TCMR : (SSC Offset: 0x18) SSC Transmit Clock Mode Register --------
// -------- SSC_TFMR : (SSC Offset: 0x1c) SSC Transmit Frame Mode Register --------
#define AT91C_SSC_DATDEF      ((unsigned int) 0x1 <<  5) // (SSC) Data Default Value
#define AT91C_SSC_FSDEN       ((unsigned int) 0x1 << 23) // (SSC) Frame Sync Data Enable
// -------- SSC_SR : (SSC Offset: 0x40) SSC Status Register --------
#define AT91C_SSC_TXRDY       ((unsigned int) 0x1 <<  0) // (SSC) Transmit Ready
#define AT91C_SSC_TXEMPTY     ((unsigned int) 0x1 <<  1) // (SSC) Transmit Empty
#define AT91C_SSC_ENDTX       ((unsigned int) 0x1 <<  2) // (SSC) End Of Transmission
#define AT91C_SSC_TXBUFE      ((unsigned int) 0x1 <<  3) // (SSC) Transmit Buffer Empty
#define AT91C_SSC_RXRDY       ((unsigned int) 0x1 <<  4) // (SSC) Receive Ready
#define AT91C_SSC_OVRUN       ((unsigned int) 0x1 <<  5) // (SSC) Receive Overrun
#define AT91C_SSC_ENDRX       ((unsigned int) 0x1 <<  6) // (SSC) End of Reception
#define AT91C_SSC_RXBUFF      ((unsigned int) 0x1 <<  7) // (SSC) Receive Buffer Full
#define AT91C_SSC_TXSYN       ((unsigned int) 0x1 << 10) // (SSC) Transmit Sync
#define AT91C_SSC_RXSYN       ((unsigned int) 0x1 << 11) // (SSC) Receive Sync
#define AT91C_SSC_TXENA       ((unsigned int) 0x1 << 16) // (SSC) Transmit Enable
#define AT91C_SSC_RXENA       ((unsigned int) 0x1 << 17) // (SSC) Receive Enable
// -------- SSC_IER : (SSC Offset: 0x44) SSC Interrupt Enable Register --------
// -------- SSC_IDR : (SSC Offset: 0x48) SSC Interrupt Disable Register --------
// -------- SSC_IMR : (SSC Offset: 0x4c) SSC Interrupt Mask Register --------

// ========== Register definition for PDC_SSC peripheral ==========
#define AT91C_SSC_TNCR  ((AT91_REG *) 	0xFFFD411C) // (PDC_SSC) Transmit Next Counter Register
#define AT91C_SSC_RPR   ((AT91_REG *) 	0xFFFD4100) // (PDC_SSC) Receive Pointer Register
#define AT91C_SSC_RNCR  ((AT91_REG *) 	0xFFFD4114) // (PDC_SSC) Receive Next Counter Register
#define AT91C_SSC_TPR   ((AT91_REG *) 	0xFFFD4108) // (PDC_SSC) Transmit Pointer Register
#define AT91C_SSC_PTCR  ((AT91_REG *) 	0xFFFD4120) // (PDC_SSC) PDC Transfer Control Register
#define AT91C_SSC_TCR   ((AT91_REG *) 	0xFFFD410C) // (PDC_SSC) Transmit Counter Register
#define AT91C_SSC_RCR   ((AT91_REG *) 	0xFFFD4104) // (PDC_SSC) Receive Counter Register
#define AT91C_SSC_RNPR  ((AT91_REG *) 	0xFFFD4110) // (PDC_SSC) Receive Next Pointer Register
#define AT91C_SSC_TNPR  ((AT91_REG *) 	0xFFFD4118) // (PDC_SSC) Transmit Next Pointer Register
#define AT91C_SSC_PTSR  ((AT91_REG *) 	0xFFFD4124) // (PDC_SSC) PDC Transfer Status Register
// ========== Register definition for SSC peripheral ==========
#define AT91C_SSC_RHR   ((AT91_REG *) 	0xFFFD4020) // (SSC) Receive Holding Register
#define AT91C_SSC_RSHR  ((AT91_REG *) 	0xFFFD4030) // (SSC) Receive Sync Holding Register
#define AT91C_SSC_TFMR  ((AT91_REG *) 	0xFFFD401C) // (SSC) Transmit Frame Mode Register
#define AT91C_SSC_IDR   ((AT91_REG *) 	0xFFFD4048) // (SSC) Interrupt Disable Register
#define AT91C_SSC_THR   ((AT91_REG *) 	0xFFFD4024) // (SSC) Transmit Holding Register
#define AT91C_SSC_RCMR  ((AT91_REG *) 	0xFFFD4010) // (SSC) Receive Clock ModeRegister
#define AT91C_SSC_IER   ((AT91_REG *) 	0xFFFD4044) // (SSC) Interrupt Enable Register
#define AT91C_SSC_TSHR  ((AT91_REG *) 	0xFFFD4034) // (SSC) Transmit Sync Holding Register
#define AT91C_SSC_SR    ((AT91_REG *) 	0xFFFD4040) // (SSC) Status Register
#define AT91C_SSC_CMR   ((AT91_REG *) 	0xFFFD4004) // (SSC) Clock Mode Register
#define AT91C_SSC_TCMR  ((AT91_REG *) 	0xFFFD4018) // (SSC) Transmit Clock Mode Register
#define AT91C_SSC_CR    ((AT91_REG *) 	0xFFFD4000) // (SSC) Control Register
#define AT91C_SSC_IMR   ((AT91_REG *) 	0xFFFD404C) // (SSC) Interrupt Mask Register
#define AT91C_SSC_RFMR  ((AT91_REG *) 	0xFFFD4014) // (SSC) Receive Frame Mode Register

#endif /* AT91SAM7_SSC_H_ */
