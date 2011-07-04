/**
 * @file
 * @brief Clock Generator Controller interface
 *
 * @date 29.09.10
 * @author Anton Kozlov
 */

#ifndef AT91SAM7_CKGR_H_
#define AT91SAM7_CKGR_H_

#include <types.h>

typedef struct _AT91S_CKGR {
	at91_reg_t	 CKGR_MOR; 	// Main Oscillator Register
	at91_reg_t	 CKGR_MCFR; 	// Main Clock  Frequency Register
	at91_reg_t	 Reserved0[1]; 	//
	at91_reg_t	 CKGR_PLLR; 	// PLL Register
} AT91S_CKGR, *AT91PS_CKGR;

// -------- CKGR_MOR : (CKGR Offset: 0x0) Main Oscillator Register --------
#define AT91C_CKGR_MOSCEN     ((unsigned int) 0x1 <<  0) // (CKGR) Main Oscillator Enable
#define AT91C_CKGR_OSCBYPASS  ((unsigned int) 0x1 <<  1) // (CKGR) Main Oscillator Bypass
#define AT91C_CKGR_OSCOUNT    ((unsigned int) 0xFF <<  8) // (CKGR) Main Oscillator Start-up Time
// -------- CKGR_MCFR : (CKGR Offset: 0x4) Main Clock Frequency Register --------
#define AT91C_CKGR_MAINF      ((unsigned int) 0xFFFF <<  0) // (CKGR) Main Clock Frequency
#define AT91C_CKGR_MAINRDY    ((unsigned int) 0x1 << 16) // (CKGR) Main Clock Ready
// -------- CKGR_PLLR : (CKGR Offset: 0xc) PLL B Register --------
#define AT91C_CKGR_DIV        ((unsigned int) 0xFF <<  0) // (CKGR) Divider Selected
#define AT91C_CKGR_DIV_OFFSET	0
#define 	AT91C_CKGR_DIV_0                    ((unsigned int) 0x0) // (CKGR) Divider output is 0
#define 	AT91C_CKGR_DIV_BYPASS               ((unsigned int) 0x1) // (CKGR) Divider is bypassed
#define AT91C_CKGR_PLLCOUNT   ((unsigned int) 0x3F <<  8) // (CKGR) PLL Counter
#define AT91C_CKGR_PLLCOUNT_OFFSET 8
#define AT91C_CKGR_OUT        ((unsigned int) 0x3 << 14) // (CKGR) PLL Output Frequency Range
#define 	AT91C_CKGR_OUT_0                    ((unsigned int) 0x0 << 14) // (CKGR) Please refer to the PLL datasheet
#define 	AT91C_CKGR_OUT_1                    ((unsigned int) 0x1 << 14) // (CKGR) Please refer to the PLL datasheet
#define 	AT91C_CKGR_OUT_2                    ((unsigned int) 0x2 << 14) // (CKGR) Please refer to the PLL datasheet
#define 	AT91C_CKGR_OUT_3                    ((unsigned int) 0x3 << 14) // (CKGR) Please refer to the PLL datasheet
#define AT91C_CKGR_MUL        ((unsigned int) 0x7FF << 16) // (CKGR) PLL Multiplier
#define AT91C_CKGR_MUL_OFFSET	16
#define AT91C_CKGR_USBDIV     ((unsigned int) 0x3 << 28) // (CKGR) Divider for USB Clocks
#define 	AT91C_CKGR_USBDIV_0                    ((unsigned int) 0x0 << 28) // (CKGR) Divider output is PLL clock output
#define 	AT91C_CKGR_USBDIV_1                    ((unsigned int) 0x1 << 28) // (CKGR) Divider output is PLL clock output divided by 2
#define 	AT91C_CKGR_USBDIV_2                    ((unsigned int) 0x2 << 28) // (CKGR) Divider output is PLL clock output divided by 4

// ========== Register definition for CKGR peripheral ==========
#define AT91C_CKGR_MOR  ((AT91_REG *) 	0xFFFFFC20) // (CKGR) Main Oscillator Register
#define AT91C_CKGR_PLLR ((AT91_REG *) 	0xFFFFFC2C) // (CKGR) PLL Register
#define AT91C_CKGR_MCFR ((AT91_REG *) 	0xFFFFFC24) // (CKGR) Main Clock  Frequency Register

#endif /* AT91SAM7_CKGR_H_ */
