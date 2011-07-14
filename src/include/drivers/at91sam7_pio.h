/**
 * @file
 * @brief Parallel Input/Output Controller interface
 *
 * @date 26.09.10
 * @author Anton Kozlov
 */

#ifndef AT91SAM7_PIO_H_
#define AT91SAM7_PIO_H_

#include <types.h>
#include <hal/reg.h>

typedef struct _AT91S_PIO {
	volatile at91_reg_t	 PIO_PER; 	// PIO Enable Register
	volatile at91_reg_t	 PIO_PDR; 	// PIO Disable Register
	volatile at91_reg_t	 PIO_PSR; 	// PIO Status Register
	volatile at91_reg_t	 Reserved0[1]; 	//
	volatile at91_reg_t	 PIO_OER; 	// Output Enable Register
	volatile at91_reg_t	 PIO_ODR; 	// Output Disable Registerr
	volatile at91_reg_t	 PIO_OSR; 	// Output Status Register
	volatile at91_reg_t	 Reserved1[1]; 	//
	volatile at91_reg_t	 PIO_IFER; 	// Input Filter Enable Register
	volatile at91_reg_t	 PIO_IFDR; 	// Input Filter Disable Register
	volatile at91_reg_t	 PIO_IFSR; 	// Input Filter Status Register
	volatile at91_reg_t	 Reserved2[1]; 	//
	volatile at91_reg_t	 PIO_SODR; 	// Set Output Data Register
	volatile at91_reg_t	 PIO_CODR; 	// Clear Output Data Register
	volatile at91_reg_t	 PIO_ODSR; 	// Output Data Status Register
	volatile at91_reg_t	 PIO_PDSR; 	// Pin Data Status Register
	volatile at91_reg_t	 PIO_IER; 	// Interrupt Enable Register
	volatile at91_reg_t	 PIO_IDR; 	// Interrupt Disable Register
	volatile at91_reg_t	 PIO_IMR; 	// Interrupt Mask Register
	volatile at91_reg_t	 PIO_ISR; 	// Interrupt Status Register
	volatile at91_reg_t	 PIO_MDER; 	// Multi-driver Enable Register
	volatile at91_reg_t	 PIO_MDDR; 	// Multi-driver Disable Register
	volatile at91_reg_t	 PIO_MDSR; 	// Multi-driver Status Register
	volatile at91_reg_t	 Reserved3[1]; 	//
	volatile at91_reg_t	 PIO_PPUDR; 	// Pull-up Disable Register
	volatile at91_reg_t	 PIO_PPUER; 	// Pull-up Enable Register
	volatile at91_reg_t	 PIO_PPUSR; 	// Pull-up Status Register
	volatile at91_reg_t	 Reserved4[1]; 	//
	volatile at91_reg_t	 PIO_ASR; 	// Select A Register
	volatile at91_reg_t	 PIO_BSR; 	// Select B Register
	volatile at91_reg_t	 PIO_ABSR; 	// AB Select Status Register
	volatile at91_reg_t	 Reserved5[9]; 	//
	volatile at91_reg_t	 PIO_OWER; 	// Output Write Enable Register
	volatile at91_reg_t	 PIO_OWDR; 	// Output Write Disable Register
	volatile at91_reg_t	 PIO_OWSR; 	// Output Write Status Register
} AT91S_PIO, *AT91PS_PIO;

#define AT91C_PIO_PA0        ((unsigned int) 1 <<  0) // Pin Controlled by PA0
#define AT91C_PA0_PWM0     ((unsigned int) AT91C_PIO_PA0) //  PWM Channel 0
#define AT91C_PA0_TIOA0    ((unsigned int) AT91C_PIO_PA0) //  Timer Counter 0 Multipurpose Timer I/O Pin A
#define AT91C_PIO_PA1        ((unsigned int) 1 <<  1) // Pin Controlled by PA1
#define AT91C_PA1_PWM1     ((unsigned int) AT91C_PIO_PA1) //  PWM Channel 1
#define AT91C_PA1_TIOB0    ((unsigned int) AT91C_PIO_PA1) //  Timer Counter 0 Multipurpose Timer I/O Pin B
#define AT91C_PIO_PA10       ((unsigned int) 1 << 10) // Pin Controlled by PA10
#define AT91C_PA10_DTXD     ((unsigned int) AT91C_PIO_PA10) //  DBGU Debug Transmit Data
#define AT91C_PA10_NPCS2    ((unsigned int) AT91C_PIO_PA10) //  SPI Peripheral Chip Select 2
#define AT91C_PIO_PA11       ((unsigned int) 1 << 11) // Pin Controlled by PA11
#define AT91C_PA11_NPCS0    ((unsigned int) AT91C_PIO_PA11) //  SPI Peripheral Chip Select 0
#define AT91C_PA11_PWM0     ((unsigned int) AT91C_PIO_PA11) //  PWM Channel 0
#define AT91C_PIO_PA12       ((unsigned int) 1 << 12) // Pin Controlled by PA12
#define AT91C_PA12_MISO     ((unsigned int) AT91C_PIO_PA12) //  SPI Master In Slave
#define AT91C_PA12_PWM1     ((unsigned int) AT91C_PIO_PA12) //  PWM Channel 1
#define AT91C_PIO_PA13       ((unsigned int) 1 << 13) // Pin Controlled by PA13
#define AT91C_PA13_MOSI     ((unsigned int) AT91C_PIO_PA13) //  SPI Master Out Slave
#define AT91C_PA13_PWM2     ((unsigned int) AT91C_PIO_PA13) //  PWM Channel 2
#define AT91C_PIO_PA14       ((unsigned int) 1 << 14) // Pin Controlled by PA14
#define AT91C_PA14_SPCK     ((unsigned int) AT91C_PIO_PA14) //  SPI Serial Clock
#define AT91C_PA14_PWM3     ((unsigned int) AT91C_PIO_PA14) //  PWM Channel 3
#define AT91C_PIO_PA15       ((unsigned int) 1 << 15) // Pin Controlled by PA15
#define AT91C_PA15_TF       ((unsigned int) AT91C_PIO_PA15) //  SSC Transmit Frame Sync
#define AT91C_PA15_TIOA1    ((unsigned int) AT91C_PIO_PA15) //  Timer Counter 1 Multipurpose Timer I/O Pin A
#define AT91C_PIO_PA16       ((unsigned int) 1 << 16) // Pin Controlled by PA16
#define AT91C_PA16_TK       ((unsigned int) AT91C_PIO_PA16) //  SSC Transmit Clock
#define AT91C_PA16_TIOB1    ((unsigned int) AT91C_PIO_PA16) //  Timer Counter 1 Multipurpose Timer I/O Pin B
#define AT91C_PIO_PA17       ((unsigned int) 1 << 17) // Pin Controlled by PA17
#define AT91C_PA17_TD       ((unsigned int) AT91C_PIO_PA17) //  SSC Transmit data
#define AT91C_PA17_PCK1     ((unsigned int) AT91C_PIO_PA17) //  PMC Programmable Clock Output 1
#define AT91C_PIO_PA18       ((unsigned int) 1 << 18) // Pin Controlled by PA18
#define AT91C_PA18_RD       ((unsigned int) AT91C_PIO_PA18) //  SSC Receive Data
#define AT91C_PA18_PCK2     ((unsigned int) AT91C_PIO_PA18) //  PMC Programmable Clock Output 2
#define AT91C_PIO_PA19       ((unsigned int) 1 << 19) // Pin Controlled by PA19
#define AT91C_PA19_RK       ((unsigned int) AT91C_PIO_PA19) //  SSC Receive Clock
#define AT91C_PA19_FIQ      ((unsigned int) AT91C_PIO_PA19) //  AIC Fast Interrupt Input
#define AT91C_PIO_PA2        ((unsigned int) 1 <<  2) // Pin Controlled by PA2
#define AT91C_PA2_PWM2     ((unsigned int) AT91C_PIO_PA2) //  PWM Channel 2
#define AT91C_PA2_SCK0     ((unsigned int) AT91C_PIO_PA2) //  USART 0 Serial Clock
#define AT91C_PIO_PA20       ((unsigned int) 1 << 20) // Pin Controlled by PA20
#define AT91C_PA20_RF       ((unsigned int) AT91C_PIO_PA20) //  SSC Receive Frame Sync
#define AT91C_PA20_IRQ0     ((unsigned int) AT91C_PIO_PA20) //  External Interrupt 0
#define AT91C_PIO_PA21       ((unsigned int) 1 << 21) // Pin Controlled by PA21
#define AT91C_PA21_RXD1     ((unsigned int) AT91C_PIO_PA21) //  USART 1 Receive Data
#define AT91C_PA21_PCK1     ((unsigned int) AT91C_PIO_PA21) //  PMC Programmable Clock Output 1
#define AT91C_PIO_PA22       ((unsigned int) 1 << 22) // Pin Controlled by PA22
#define AT91C_PA22_TXD1     ((unsigned int) AT91C_PIO_PA22) //  USART 1 Transmit Data
#define AT91C_PA22_NPCS3    ((unsigned int) AT91C_PIO_PA22) //  SPI Peripheral Chip Select 3
#define AT91C_PIO_PA23       ((unsigned int) 1 << 23) // Pin Controlled by PA23
#define AT91C_PA23_SCK1     ((unsigned int) AT91C_PIO_PA23) //  USART 1 Serial Clock
#define AT91C_PA23_PWM0     ((unsigned int) AT91C_PIO_PA23) //  PWM Channel 0
#define AT91C_PIO_PA24       ((unsigned int) 1 << 24) // Pin Controlled by PA24
#define AT91C_PA24_RTS1     ((unsigned int) AT91C_PIO_PA24) //  USART 1 Ready To Send
#define AT91C_PA24_PWM1     ((unsigned int) AT91C_PIO_PA24) //  PWM Channel 1
#define AT91C_PIO_PA25       ((unsigned int) 1 << 25) // Pin Controlled by PA25
#define AT91C_PA25_CTS1     ((unsigned int) AT91C_PIO_PA25) //  USART 1 Clear To Send
#define AT91C_PA25_PWM2     ((unsigned int) AT91C_PIO_PA25) //  PWM Channel 2
#define AT91C_PIO_PA26       ((unsigned int) 1 << 26) // Pin Controlled by PA26
#define AT91C_PA26_DCD1     ((unsigned int) AT91C_PIO_PA26) //  USART 1 Data Carrier Detect
#define AT91C_PA26_TIOA2    ((unsigned int) AT91C_PIO_PA26) //  Timer Counter 2 Multipurpose Timer I/O Pin A
#define AT91C_PIO_PA27       ((unsigned int) 1 << 27) // Pin Controlled by PA27
#define AT91C_PA27_DTR1     ((unsigned int) AT91C_PIO_PA27) //  USART 1 Data Terminal ready
#define AT91C_PA27_TIOB2    ((unsigned int) AT91C_PIO_PA27) //  Timer Counter 2 Multipurpose Timer I/O Pin B
#define AT91C_PIO_PA28       ((unsigned int) 1 << 28) // Pin Controlled by PA28
#define AT91C_PA28_DSR1     ((unsigned int) AT91C_PIO_PA28) //  USART 1 Data Set ready
#define AT91C_PA28_TCLK1    ((unsigned int) AT91C_PIO_PA28) //  Timer Counter 1 external clock input
#define AT91C_PIO_PA29       ((unsigned int) 1 << 29) // Pin Controlled by PA29
#define AT91C_PA29_RI1      ((unsigned int) AT91C_PIO_PA29) //  USART 1 Ring Indicator
#define AT91C_PA29_TCLK2    ((unsigned int) AT91C_PIO_PA29) //  Timer Counter 2 external clock input
#define AT91C_PIO_PA3        ((unsigned int) 1 <<  3) // Pin Controlled by PA3
#define AT91C_PA3_TWD      ((unsigned int) AT91C_PIO_PA3) //  TWI Two-wire Serial Data
#define AT91C_PA3_NPCS3    ((unsigned int) AT91C_PIO_PA3) //  SPI Peripheral Chip Select 3
#define AT91C_PIO_PA30       ((unsigned int) 1 << 30) // Pin Controlled by PA30
#define AT91C_PA30_IRQ1     ((unsigned int) AT91C_PIO_PA30) //  External Interrupt 1
#define AT91C_PA30_NPCS2    ((unsigned int) AT91C_PIO_PA30) //  SPI Peripheral Chip Select 2
#define AT91C_PIO_PA31       ((unsigned int) 1 << 31) // Pin Controlled by PA31
#define AT91C_PA31_NPCS1    ((unsigned int) AT91C_PIO_PA31) //  SPI Peripheral Chip Select 1
#define AT91C_PA31_PCK2     ((unsigned int) AT91C_PIO_PA31) //  PMC Programmable Clock Output 2
#define AT91C_PIO_PA4        ((unsigned int) 1 <<  4) // Pin Controlled by PA4
#define AT91C_PA4_TWCK     ((unsigned int) AT91C_PIO_PA4) //  TWI Two-wire Serial Clock
#define AT91C_PA4_TCLK0    ((unsigned int) AT91C_PIO_PA4) //  Timer Counter 0 external clock input
#define AT91C_PIO_PA5        ((unsigned int) 1 <<  5) // Pin Controlled by PA5
#define AT91C_PA5_RXD0     ((unsigned int) AT91C_PIO_PA5) //  USART 0 Receive Data
#define AT91C_PA5_NPCS3    ((unsigned int) AT91C_PIO_PA5) //  SPI Peripheral Chip Select 3
#define AT91C_PIO_PA6        ((unsigned int) 1 <<  6) // Pin Controlled by PA6
#define AT91C_PA6_TXD0     ((unsigned int) AT91C_PIO_PA6) //  USART 0 Transmit Data
#define AT91C_PA6_PCK0     ((unsigned int) AT91C_PIO_PA6) //  PMC Programmable Clock Output 0
#define AT91C_PIO_PA7        ((unsigned int) 1 <<  7) // Pin Controlled by PA7
#define AT91C_PA7_RTS0     ((unsigned int) AT91C_PIO_PA7) //  USART 0 Ready To Send
#define AT91C_PA7_PWM3     ((unsigned int) AT91C_PIO_PA7) //  PWM Channel 3
#define AT91C_PIO_PA8        ((unsigned int) 1 <<  8) // Pin Controlled by PA8
#define AT91C_PA8_CTS0     ((unsigned int) AT91C_PIO_PA8) //  USART 0 Clear To Send
#define AT91C_PA8_ADTRG    ((unsigned int) AT91C_PIO_PA8) //  ADC External Trigger
#define AT91C_PIO_PA9        ((unsigned int) 1 <<  9) // Pin Controlled by PA9
#define AT91C_PA9_DRXD     ((unsigned int) AT91C_PIO_PA9) //  DBGU Debug Receive Data
#define AT91C_PA9_NPCS1    ((unsigned int) AT91C_PIO_PA9) //  SPI Peripheral Chip Select 1
// ========== Register definition for PIOA peripheral ==========
#define AT91C_PIOA_ODR  ((at91_reg_t *) 	0xFFFFF414) // (PIOA) Output Disable Registerr
#define AT91C_PIOA_SODR ((at91_reg_t *) 	0xFFFFF430) // (PIOA) Set Output Data Register
#define AT91C_PIOA_ISR  ((at91_reg_t *) 	0xFFFFF44C) // (PIOA) Interrupt Status Register
#define AT91C_PIOA_ABSR ((at91_reg_t *) 	0xFFFFF478) // (PIOA) AB Select Status Register
#define AT91C_PIOA_IER  ((at91_reg_t *) 	0xFFFFF440) // (PIOA) Interrupt Enable Register
#define AT91C_PIOA_PPUDR ((at91_reg_t *) 	0xFFFFF460) // (PIOA) Pull-up Disable Register
#define AT91C_PIOA_IMR  ((at91_reg_t *) 	0xFFFFF448) // (PIOA) Interrupt Mask Register
#define AT91C_PIOA_PER  ((at91_reg_t *) 	0xFFFFF400) // (PIOA) PIO Enable Register
#define AT91C_PIOA_IFDR ((at91_reg_t *) 	0xFFFFF424) // (PIOA) Input Filter Disable Register
#define AT91C_PIOA_OWDR ((at91_reg_t *) 	0xFFFFF4A4) // (PIOA) Output Write Disable Register
#define AT91C_PIOA_MDSR ((at91_reg_t *) 	0xFFFFF458) // (PIOA) Multi-driver Status Register
#define AT91C_PIOA_IDR  ((at91_reg_t *) 	0xFFFFF444) // (PIOA) Interrupt Disable Register
#define AT91C_PIOA_ODSR ((at91_reg_t *) 	0xFFFFF438) // (PIOA) Output Data Status Register
#define AT91C_PIOA_PPUSR ((at91_reg_t *) 	0xFFFFF468) // (PIOA) Pull-up Status Register
#define AT91C_PIOA_OWSR ((at91_reg_t *) 	0xFFFFF4A8) // (PIOA) Output Write Status Register
#define AT91C_PIOA_BSR  ((at91_reg_t *) 	0xFFFFF474) // (PIOA) Select B Register
#define AT91C_PIOA_OWER ((at91_reg_t *) 	0xFFFFF4A0) // (PIOA) Output Write Enable Register
#define AT91C_PIOA_IFER ((at91_reg_t *) 	0xFFFFF420) // (PIOA) Input Filter Enable Register
#define AT91C_PIOA_PDSR ((at91_reg_t *) 	0xFFFFF43C) // (PIOA) Pin Data Status Register
#define AT91C_PIOA_PPUER ((at91_reg_t *) 	0xFFFFF464) // (PIOA) Pull-up Enable Register
#define AT91C_PIOA_OSR  ((at91_reg_t *) 	0xFFFFF418) // (PIOA) Output Status Register
#define AT91C_PIOA_ASR  ((at91_reg_t *) 	0xFFFFF470) // (PIOA) Select A Register
#define AT91C_PIOA_MDDR ((at91_reg_t *) 	0xFFFFF454) // (PIOA) Multi-driver Disable Register
#define AT91C_PIOA_CODR ((at91_reg_t *) 	0xFFFFF434) // (PIOA) Clear Output Data Register
#define AT91C_PIOA_MDER ((at91_reg_t *) 	0xFFFFF450) // (PIOA) Multi-driver Enable Register
#define AT91C_PIOA_PDR  ((at91_reg_t *) 	0xFFFFF404) // (PIOA) PIO Disable Register
#define AT91C_PIOA_IFSR ((at91_reg_t *) 	0xFFFFF428) // (PIOA) Input Filter Status Register
#define AT91C_PIOA_OER  ((at91_reg_t *) 	0xFFFFF410) // (PIOA) Output Enable Register
#define AT91C_PIOA_PSR  ((at91_reg_t *) 	0xFFFFF408) // (PIOA) PIO Status Register

#endif /* AT91SAM7_PIO_H_ */
