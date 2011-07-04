/**
 * @file
 * @brief Power Managment Controller interface
 *
 * @date 26.09.10
 * @author Anton Kozlov
 */

#ifndef AT91SAM7_PMC_H_
#define AT91SAM7_PMC_H_

typedef struct _AT91S_PMC {
	at91_reg_t	 PMC_SCER; 	// System Clock Enable Register
	at91_reg_t	 PMC_SCDR; 	// System Clock Disable Register
	at91_reg_t	 PMC_SCSR; 	// System Clock Status Register
	at91_reg_t	 Reserved0[1]; 	//
	at91_reg_t	 PMC_PCER; 	// Peripheral Clock Enable Register
	at91_reg_t	 PMC_PCDR; 	// Peripheral Clock Disable Register
	at91_reg_t	 PMC_PCSR; 	// Peripheral Clock Status Register
	at91_reg_t	 Reserved1[1]; 	//
	at91_reg_t	 PMC_MOR; 	// Main Oscillator Register
	at91_reg_t	 PMC_MCFR; 	// Main Clock  Frequency Register
	at91_reg_t	 Reserved2[1]; 	//
	at91_reg_t	 PMC_PLLR; 	// PLL Register
	at91_reg_t	 PMC_MCKR; 	// Master Clock Register
	at91_reg_t	 Reserved3[3]; 	//
	at91_reg_t	 PMC_PCKR[3]; 	// Programmable Clock Register
	at91_reg_t	 Reserved4[5]; 	//
	at91_reg_t	 PMC_IER; 	// Interrupt Enable Register
	at91_reg_t	 PMC_IDR; 	// Interrupt Disable Register
	at91_reg_t	 PMC_SR; 	// Status Register
	at91_reg_t	 PMC_IMR; 	// Interrupt Mask Register
} AT91S_PMC, *AT91PS_PMC;

// -------- PMC_SCER : (PMC Offset: 0x0) System Clock Enable Register --------
#define AT91C_PMC_PCK         ((unsigned int) 0x1 <<  0) // (PMC) Processor Clock
#define AT91C_PMC_UDP         ((unsigned int) 0x1 <<  7) // (PMC) USB Device Port Clock
#define AT91C_PMC_PCK0        ((unsigned int) 0x1 <<  8) // (PMC) Programmable Clock Output
#define AT91C_PMC_PCK1        ((unsigned int) 0x1 <<  9) // (PMC) Programmable Clock Output
#define AT91C_PMC_PCK2        ((unsigned int) 0x1 << 10) // (PMC) Programmable Clock Output
// -------- PMC_SCDR : (PMC Offset: 0x4) System Clock Disable Register --------
// -------- PMC_SCSR : (PMC Offset: 0x8) System Clock Status Register --------
// -------- CKGR_MOR : (PMC Offset: 0x20) Main Oscillator Register --------
// -------- CKGR_MCFR : (PMC Offset: 0x24) Main Clock Frequency Register --------
// -------- CKGR_PLLR : (PMC Offset: 0x2c) PLL B Register --------
// -------- PMC_MCKR : (PMC Offset: 0x30) Master Clock Register --------
#define AT91C_PMC_CSS         ((unsigned int) 0x3 <<  0) // (PMC) Programmable Clock Selection
#define 	AT91C_PMC_CSS_SLOW_CLK             ((unsigned int) 0x0) // (PMC) Slow Clock is selected
#define 	AT91C_PMC_CSS_MAIN_CLK             ((unsigned int) 0x1) // (PMC) Main Clock is selected
#define 	AT91C_PMC_CSS_PLL_CLK              ((unsigned int) 0x3) // (PMC) Clock from PLL is selected
#define AT91C_PMC_PRES        ((unsigned int) 0x7 <<  2) // (PMC) Programmable Clock Prescaler
#define 	AT91C_PMC_PRES_CLK                  ((unsigned int) 0x0 <<  2) // (PMC) Selected clock
#define 	AT91C_PMC_PRES_CLK_2                ((unsigned int) 0x1 <<  2) // (PMC) Selected clock divided by 2
#define 	AT91C_PMC_PRES_CLK_4                ((unsigned int) 0x2 <<  2) // (PMC) Selected clock divided by 4
#define 	AT91C_PMC_PRES_CLK_8                ((unsigned int) 0x3 <<  2) // (PMC) Selected clock divided by 8
#define 	AT91C_PMC_PRES_CLK_16               ((unsigned int) 0x4 <<  2) // (PMC) Selected clock divided by 16
#define 	AT91C_PMC_PRES_CLK_32               ((unsigned int) 0x5 <<  2) // (PMC) Selected clock divided by 32
#define 	AT91C_PMC_PRES_CLK_64               ((unsigned int) 0x6 <<  2) // (PMC) Selected clock divided by 64
// -------- PMC_PCKR : (PMC Offset: 0x40) Programmable Clock Register --------
// -------- PMC_IER : (PMC Offset: 0x60) PMC Interrupt Enable Register --------
#define AT91C_PMC_MOSCS       ((unsigned int) 0x1 <<  0) // (PMC) MOSC Status/Enable/Disable/Mask
#define AT91C_PMC_LOCK        ((unsigned int) 0x1 <<  2) // (PMC) PLL Status/Enable/Disable/Mask
#define AT91C_PMC_MCKRDY      ((unsigned int) 0x1 <<  3) // (PMC) MCK_RDY Status/Enable/Disable/Mask
#define AT91C_PMC_PCK0RDY     ((unsigned int) 0x1 <<  8) // (PMC) PCK0_RDY Status/Enable/Disable/Mask
#define AT91C_PMC_PCK1RDY     ((unsigned int) 0x1 <<  9) // (PMC) PCK1_RDY Status/Enable/Disable/Mask
#define AT91C_PMC_PCK2RDY     ((unsigned int) 0x1 << 10) // (PMC) PCK2_RDY Status/Enable/Disable/Mask
// -------- PMC_IDR : (PMC Offset: 0x64) PMC Interrupt Disable Register --------
// -------- PMC_SR : (PMC Offset: 0x68) PMC Status Register --------
// -------- PMC_IMR : (PMC Offset: 0x6c) PMC Interrupt Mask Register --------

// ========== Register definition for PMC peripheral ==========
#define AT91C_PMC_IDR   ((at91_reg_t *) 	0xFFFFFC64) // (PMC) Interrupt Disable Register
#define AT91C_PMC_MOR   ((at91_reg_t *) 	0xFFFFFC20) // (PMC) Main Oscillator Register
#define AT91C_PMC_PLLR  ((at91_reg_t *) 	0xFFFFFC2C) // (PMC) PLL Register
#define AT91C_PMC_PCER  ((at91_reg_t *) 	0xFFFFFC10) // (PMC) Peripheral Clock Enable Register
#define AT91C_PMC_PCKR  ((at91_reg_t *) 	0xFFFFFC40) // (PMC) Programmable Clock Register
#define AT91C_PMC_MCKR  ((at91_reg_t *) 	0xFFFFFC30) // (PMC) Master Clock Register
#define AT91C_PMC_SCDR  ((at91_reg_t *) 	0xFFFFFC04) // (PMC) System Clock Disable Register
#define AT91C_PMC_PCDR  ((at91_reg_t *) 	0xFFFFFC14) // (PMC) Peripheral Clock Disable Register
#define AT91C_PMC_SCSR  ((at91_reg_t *) 	0xFFFFFC08) // (PMC) System Clock Status Register
#define AT91C_PMC_PCSR  ((at91_reg_t *) 	0xFFFFFC18) // (PMC) Peripheral Clock Status Register
#define AT91C_PMC_MCFR  ((at91_reg_t *) 	0xFFFFFC24) // (PMC) Main Clock  Frequency Register
#define AT91C_PMC_SCER  ((at91_reg_t *) 	0xFFFFFC00) // (PMC) System Clock Enable Register
#define AT91C_PMC_IMR   ((at91_reg_t *) 	0xFFFFFC6C) // (PMC) Interrupt Mask Register
#define AT91C_PMC_IER   ((at91_reg_t *) 	0xFFFFFC60) // (PMC) Interrupt Enable Register
#define AT91C_PMC_SR    ((at91_reg_t *) 	0xFFFFFC68) // (PMC) Status Register

#endif /* AT91SAM7_PMC_H_ */
