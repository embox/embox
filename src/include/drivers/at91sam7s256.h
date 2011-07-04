//  ----------------------------------------------------------------------------
//          ATMEL Microcontroller Software Support  -  ROUSSET  -
//  ----------------------------------------------------------------------------
//  DISCLAIMER:  THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
//  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
//  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
//  DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
//  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
//  OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
//  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//  ----------------------------------------------------------------------------
// File Name           : AT91SAM7S256.h
// Object              : AT91SAM7S256 definitions
// Generated           : AT91 SW Application Group  03/08/2005 (15:46:13)
//
// CVS Reference       : /AT91SAM7S256.pl/1.8/Wed Feb  9 15:29:26 2005//
// CVS Reference       : /SYS_SAM7S.pl/1.2/Tue Feb  1 17:01:52 2005//
// CVS Reference       : /MC_SAM7S.pl/1.2/Tue Feb  1 17:01:00 2005//
// CVS Reference       : /PMC_SAM7S_USB.pl/1.4/Tue Feb  8 13:58:22 2005//
// CVS Reference       : /RSTC_SAM7S.pl/1.1/Tue Feb  1 16:16:35 2005//
// CVS Reference       : /RTTC_6081A.pl/1.2/Tue Nov  9 14:43:58 2004//
// CVS Reference       : /PITC_6079A.pl/1.2/Tue Nov  9 14:43:56 2004//
// CVS Reference       : /WDTC_6080A.pl/1.3/Tue Nov  9 14:44:00 2004//
// CVS Reference       : /VREG_6085B.pl/1.1/Tue Feb  1 16:05:48 2005//
// CVS Reference       : /UDP_6083C.pl/1.1/Mon Jan 31 13:01:46 2005//
// CVS Reference       : /AIC_6075A.pl/1.1/Fri Jun 28 10:36:48 2002//
// CVS Reference       : /PIO_6057A.pl/1.2/Thu Feb  3 10:18:28 2005//
// CVS Reference       : /DBGU_6059D.pl/1.1/Mon Jan 31 13:15:32 2005//
// CVS Reference       : /US_6089C.pl/1.1/Mon Jul 12 18:23:26 2004//
// CVS Reference       : /SPI_6088D.pl/1.2/Mon Feb 14 07:24:18 2005//
// CVS Reference       : /SSC_6078A.pl/1.1/Tue Jul 13 07:45:40 2004//
// CVS Reference       : /TC_6082A.pl/1.6/Fri Feb 18 13:53:30 2005//
// CVS Reference       : /TWI_6061A.pl/1.1/Tue Jul 13 07:38:06 2004//
// CVS Reference       : /PDC_6074C.pl/1.2/Thu Feb  3 08:48:54 2005//
// CVS Reference       : /ADC_6051C.pl/1.1/Fri Oct 17 09:12:38 2003//
// CVS Reference       : /PWM_6044D.pl/1.1/Tue Apr 27 14:53:52 2004//
//  ----------------------------------------------------------------------------

#ifndef AT91SAM7S256_H_
#define AT91SAM7S256_H_

#include <types.h>

#ifndef AT91SAM7S256_H
#define AT91SAM7S256_H

#include <types.h>
#include <hal/reg.h>

// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR System Peripherals
// *****************************************************************************
typedef struct _AT91S_SYS {
	at91_reg_t	 AIC_SMR[32]; 	// Source Mode Register
	at91_reg_t	 AIC_SVR[32]; 	// Source Vector Register
	at91_reg_t	 AIC_IVR; 	// IRQ Vector Register
	at91_reg_t	 AIC_FVR; 	// FIQ Vector Register
	at91_reg_t	 AIC_ISR; 	// Interrupt Status Register
	at91_reg_t	 AIC_IPR; 	// Interrupt Pending Register
	at91_reg_t	 AIC_IMR; 	// Interrupt Mask Register
	at91_reg_t	 AIC_CISR; 	// Core Interrupt Status Register
	at91_reg_t	 Reserved0[2]; 	//
	at91_reg_t	 AIC_IECR; 	// Interrupt Enable Command Register
	at91_reg_t	 AIC_IDCR; 	// Interrupt Disable Command Register
	at91_reg_t	 AIC_ICCR; 	// Interrupt Clear Command Register
	at91_reg_t	 AIC_ISCR; 	// Interrupt Set Command Register
	at91_reg_t	 AIC_EOICR; 	// End of Interrupt Command Register
	at91_reg_t	 AIC_SPU; 	// Spurious Vector Register
	at91_reg_t	 AIC_DCR; 	// Debug Control Register (Protect)
	at91_reg_t	 Reserved1[1]; 	//
	at91_reg_t	 AIC_FFER; 	// Fast Forcing Enable Register
	at91_reg_t	 AIC_FFDR; 	// Fast Forcing Disable Register
	at91_reg_t	 AIC_FFSR; 	// Fast Forcing Status Register
	at91_reg_t	 Reserved2[45]; 	//
	at91_reg_t	 DBGU_CR; 	// Control Register
	at91_reg_t	 DBGU_MR; 	// Mode Register
	at91_reg_t	 DBGU_IER; 	// Interrupt Enable Register
	at91_reg_t	 DBGU_IDR; 	// Interrupt Disable Register
	at91_reg_t	 DBGU_IMR; 	// Interrupt Mask Register
	at91_reg_t	 DBGU_CSR; 	// Channel Status Register
	at91_reg_t	 DBGU_RHR; 	// Receiver Holding Register
	at91_reg_t	 DBGU_THR; 	// Transmitter Holding Register
	at91_reg_t	 DBGU_BRGR; 	// Baud Rate Generator Register
	at91_reg_t	 Reserved3[7]; 	//
	at91_reg_t	 DBGU_CIDR; 	// Chip ID Register
	at91_reg_t	 DBGU_EXID; 	// Chip ID Extension Register
	at91_reg_t	 DBGU_FNTR; 	// Force NTRST Register
	at91_reg_t	 Reserved4[45]; 	//
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
	at91_reg_t	 Reserved5[54]; 	//
	at91_reg_t	 PIOA_PER; 	// PIO Enable Register
	at91_reg_t	 PIOA_PDR; 	// PIO Disable Register
	at91_reg_t	 PIOA_PSR; 	// PIO Status Register
	at91_reg_t	 Reserved6[1]; 	//
	at91_reg_t	 PIOA_OER; 	// Output Enable Register
	at91_reg_t	 PIOA_ODR; 	// Output Disable Registerr
	at91_reg_t	 PIOA_OSR; 	// Output Status Register
	at91_reg_t	 Reserved7[1]; 	//
	at91_reg_t	 PIOA_IFER; 	// Input Filter Enable Register
	at91_reg_t	 PIOA_IFDR; 	// Input Filter Disable Register
	at91_reg_t	 PIOA_IFSR; 	// Input Filter Status Register
	at91_reg_t	 Reserved8[1]; 	//
	at91_reg_t	 PIOA_SODR; 	// Set Output Data Register
	at91_reg_t	 PIOA_CODR; 	// Clear Output Data Register
	at91_reg_t	 PIOA_ODSR; 	// Output Data Status Register
	at91_reg_t	 PIOA_PDSR; 	// Pin Data Status Register
	at91_reg_t	 PIOA_IER; 	// Interrupt Enable Register
	at91_reg_t	 PIOA_IDR; 	// Interrupt Disable Register
	at91_reg_t	 PIOA_IMR; 	// Interrupt Mask Register
	at91_reg_t	 PIOA_ISR; 	// Interrupt Status Register
	at91_reg_t	 PIOA_MDER; 	// Multi-driver Enable Register
	at91_reg_t	 PIOA_MDDR; 	// Multi-driver Disable Register
	at91_reg_t	 PIOA_MDSR; 	// Multi-driver Status Register
	at91_reg_t	 Reserved9[1]; 	//
	at91_reg_t	 PIOA_PPUDR; 	// Pull-up Disable Register
	at91_reg_t	 PIOA_PPUER; 	// Pull-up Enable Register
	at91_reg_t	 PIOA_PPUSR; 	// Pull-up Status Register
	at91_reg_t	 Reserved10[1]; 	//
	at91_reg_t	 PIOA_ASR; 	// Select A Register
	at91_reg_t	 PIOA_BSR; 	// Select B Register
	at91_reg_t	 PIOA_ABSR; 	// AB Select Status Register
	at91_reg_t	 Reserved11[9]; 	//
	at91_reg_t	 PIOA_OWER; 	// Output Write Enable Register
	at91_reg_t	 PIOA_OWDR; 	// Output Write Disable Register
	at91_reg_t	 PIOA_OWSR; 	// Output Write Status Register
	at91_reg_t	 Reserved12[469]; 	//
	at91_reg_t	 PMC_SCER; 	// System Clock Enable Register
	at91_reg_t	 PMC_SCDR; 	// System Clock Disable Register
	at91_reg_t	 PMC_SCSR; 	// System Clock Status Register
	at91_reg_t	 Reserved13[1]; 	//
	at91_reg_t	 PMC_PCER; 	// Peripheral Clock Enable Register
	at91_reg_t	 PMC_PCDR; 	// Peripheral Clock Disable Register
	at91_reg_t	 PMC_PCSR; 	// Peripheral Clock Status Register
	at91_reg_t	 Reserved14[1]; 	//
	at91_reg_t	 PMC_MOR; 	// Main Oscillator Register
	at91_reg_t	 PMC_MCFR; 	// Main Clock  Frequency Register
	at91_reg_t	 Reserved15[1]; 	//
	at91_reg_t	 PMC_PLLR; 	// PLL Register
	at91_reg_t	 PMC_MCKR; 	// Master Clock Register
	at91_reg_t	 Reserved16[3]; 	//
	at91_reg_t	 PMC_PCKR[3]; 	// Programmable Clock Register
	at91_reg_t	 Reserved17[5]; 	//
	at91_reg_t	 PMC_IER; 	// Interrupt Enable Register
	at91_reg_t	 PMC_IDR; 	// Interrupt Disable Register
	at91_reg_t	 PMC_SR; 	// Status Register
	at91_reg_t	 PMC_IMR; 	// Interrupt Mask Register
	at91_reg_t	 Reserved18[36]; 	//
	at91_reg_t	 RSTC_RCR; 	// Reset Control Register
	at91_reg_t	 RSTC_RSR; 	// Reset Status Register
	at91_reg_t	 RSTC_RMR; 	// Reset Mode Register
	at91_reg_t	 Reserved19[5]; 	//
	at91_reg_t	 RTTC_RTMR; 	// Real-time Mode Register
	at91_reg_t	 RTTC_RTAR; 	// Real-time Alarm Register
	at91_reg_t	 RTTC_RTVR; 	// Real-time Value Register
	at91_reg_t	 RTTC_RTSR; 	// Real-time Status Register
	at91_reg_t	 PITC_PIMR; 	// Period Interval Mode Register
	at91_reg_t	 PITC_PISR; 	// Period Interval Status Register
	at91_reg_t	 PITC_PIVR; 	// Period Interval Value Register
	at91_reg_t	 PITC_PIIR; 	// Period Interval Image Register
	at91_reg_t	 WDTC_WDCR; 	// Watchdog Control Register
	at91_reg_t	 WDTC_WDMR; 	// Watchdog Mode Register
	at91_reg_t	 WDTC_WDSR; 	// Watchdog Status Register
	at91_reg_t	 Reserved20[5]; 	//
	at91_reg_t	 VREG_MR; 	// Voltage Regulator Mode Register
} AT91S_SYS, *AT91PS_SYS;


// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR Advanced Interrupt Controller
// *****************************************************************************
#include <drivers/at91sam7_aic.h>

// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR Perepherial Input Output Controller
// *****************************************************************************
#include <drivers/at91sam7_pio.h>

// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR Power Management Controller
// *****************************************************************************
#include <drivers/at91sam7_pmc.h>

// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR USART Controller
// *****************************************************************************
#include <drivers/at91sam7_us.h>

// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR Peripheral DMA Controller
// *****************************************************************************
typedef struct _AT91S_PDC {
	at91_reg_t	 PDC_RPR; 	// Receive Pointer Register
	at91_reg_t	 PDC_RCR; 	// Receive Counter Register
	at91_reg_t	 PDC_TPR; 	// Transmit Pointer Register
	at91_reg_t	 PDC_TCR; 	// Transmit Counter Register
	at91_reg_t	 PDC_RNPR; 	// Receive Next Pointer Register
	at91_reg_t	 PDC_RNCR; 	// Receive Next Counter Register
	at91_reg_t	 PDC_TNPR; 	// Transmit Next Pointer Register
	at91_reg_t	 PDC_TNCR; 	// Transmit Next Counter Register
	at91_reg_t	 PDC_PTCR; 	// PDC Transfer Control Register
	at91_reg_t	 PDC_PTSR; 	// PDC Transfer Status Register
} AT91S_PDC, *AT91PS_PDC;

// -------- PDC_PTCR : (PDC Offset: 0x20) PDC Transfer Control Register --------
#define AT91C_PDC_RXTEN       ((unsigned int) 0x1 <<  0) // (PDC) Receiver Transfer Enable
#define AT91C_PDC_RXTDIS      ((unsigned int) 0x1 <<  1) // (PDC) Receiver Transfer Disable
#define AT91C_PDC_TXTEN       ((unsigned int) 0x1 <<  8) // (PDC) Transmitter Transfer Enable
#define AT91C_PDC_TXTDIS      ((unsigned int) 0x1 <<  9) // (PDC) Transmitter Transfer Disable
// -------- PDC_PTSR : (PDC Offset: 0x24) PDC Transfer Status Register --------


// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR Clock Generator Controler
// *****************************************************************************
#include <drivers/at91sam7_ckgr.h>

// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR Reset Controller Interface
// *****************************************************************************
typedef struct _AT91S_RSTC {
	at91_reg_t	 RSTC_RCR; 	// Reset Control Register
	at91_reg_t	 RSTC_RSR; 	// Reset Status Register
	at91_reg_t	 RSTC_RMR; 	// Reset Mode Register
} AT91S_RSTC, *AT91PS_RSTC;

// -------- RSTC_RCR : (RSTC Offset: 0x0) Reset Control Register --------
#define AT91C_RSTC_PROCRST    ((unsigned int) 0x1 <<  0) // (RSTC) Processor Reset
#define AT91C_RSTC_PERRST     ((unsigned int) 0x1 <<  2) // (RSTC) Peripheral Reset
#define AT91C_RSTC_EXTRST     ((unsigned int) 0x1 <<  3) // (RSTC) External Reset
#define AT91C_RSTC_KEY        ((unsigned int) 0xFF << 24) // (RSTC) Password
// -------- RSTC_RSR : (RSTC Offset: 0x4) Reset Status Register --------
#define AT91C_RSTC_URSTS      ((unsigned int) 0x1 <<  0) // (RSTC) User Reset Status
#define AT91C_RSTC_BODSTS     ((unsigned int) 0x1 <<  1) // (RSTC) Brownout Detection Status
#define AT91C_RSTC_RSTTYP     ((unsigned int) 0x7 <<  8) // (RSTC) Reset Type
#define 	AT91C_RSTC_RSTTYP_POWERUP              ((unsigned int) 0x0 <<  8) // (RSTC) Power-up Reset. VDDCORE rising.
#define 	AT91C_RSTC_RSTTYP_WAKEUP               ((unsigned int) 0x1 <<  8) // (RSTC) WakeUp Reset. VDDCORE rising.
#define 	AT91C_RSTC_RSTTYP_WATCHDOG             ((unsigned int) 0x2 <<  8) // (RSTC) Watchdog Reset. Watchdog overflow occured.
#define 	AT91C_RSTC_RSTTYP_SOFTWARE             ((unsigned int) 0x3 <<  8) // (RSTC) Software Reset. Processor reset required by the software.
#define 	AT91C_RSTC_RSTTYP_USER                 ((unsigned int) 0x4 <<  8) // (RSTC) User Reset. NRST pin detected low.
#define 	AT91C_RSTC_RSTTYP_BROWNOUT             ((unsigned int) 0x5 <<  8) // (RSTC) Brownout Reset occured.
#define AT91C_RSTC_NRSTL      ((unsigned int) 0x1 << 16) // (RSTC) NRST pin level
#define AT91C_RSTC_SRCMP      ((unsigned int) 0x1 << 17) // (RSTC) Software Reset Command in Progress.
// -------- RSTC_RMR : (RSTC Offset: 0x8) Reset Mode Register --------
#define AT91C_RSTC_URSTEN     ((unsigned int) 0x1 <<  0) // (RSTC) User Reset Enable
#define AT91C_RSTC_URSTIEN    ((unsigned int) 0x1 <<  4) // (RSTC) User Reset Interrupt Enable
#define AT91C_RSTC_ERSTL      ((unsigned int) 0xF <<  8) // (RSTC) User Reset Enable
#define AT91C_RSTC_BODIEN     ((unsigned int) 0x1 << 16) // (RSTC) Brownout Detection Interrupt Enable

// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR Real Time Timer Controller Interface
// *****************************************************************************
typedef struct _AT91S_RTTC {
	at91_reg_t	 RTTC_RTMR; 	// Real-time Mode Register
	at91_reg_t	 RTTC_RTAR; 	// Real-time Alarm Register
	at91_reg_t	 RTTC_RTVR; 	// Real-time Value Register
	at91_reg_t	 RTTC_RTSR; 	// Real-time Status Register
} AT91S_RTTC, *AT91PS_RTTC;

// -------- RTTC_RTMR : (RTTC Offset: 0x0) Real-time Mode Register --------
#define AT91C_RTTC_RTPRES     ((unsigned int) 0xFFFF <<  0) // (RTTC) Real-time Timer Prescaler Value
#define AT91C_RTTC_ALMIEN     ((unsigned int) 0x1 << 16) // (RTTC) Alarm Interrupt Enable
#define AT91C_RTTC_RTTINCIEN  ((unsigned int) 0x1 << 17) // (RTTC) Real Time Timer Increment Interrupt Enable
#define AT91C_RTTC_RTTRST     ((unsigned int) 0x1 << 18) // (RTTC) Real Time Timer Restart
// -------- RTTC_RTAR : (RTTC Offset: 0x4) Real-time Alarm Register --------
#define AT91C_RTTC_ALMV       ((unsigned int) 0x0 <<  0) // (RTTC) Alarm Value
// -------- RTTC_RTVR : (RTTC Offset: 0x8) Current Real-time Value Register --------
#define AT91C_RTTC_CRTV       ((unsigned int) 0x0 <<  0) // (RTTC) Current Real-time Value
// -------- RTTC_RTSR : (RTTC Offset: 0xc) Real-time Status Register --------
#define AT91C_RTTC_ALMS       ((unsigned int) 0x1 <<  0) // (RTTC) Real-time Alarm Status
#define AT91C_RTTC_RTTINC     ((unsigned int) 0x1 <<  1) // (RTTC) Real-time Timer Increment

// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR Periodic Interval Timer Controller Interface
// *****************************************************************************
#include <drivers/at91sam7_pit.h>

// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR Watchdog Timer Controller Interface
// *****************************************************************************
#include <drivers/at91sam7_wd.h>

// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR Voltage Regulator Mode Controller Interface
// *****************************************************************************
typedef struct _AT91S_VREG {
	at91_reg_t	 VREG_MR; 	// Voltage Regulator Mode Register
} AT91S_VREG, *AT91PS_VREG;

// -------- VREG_MR : (VREG Offset: 0x0) Voltage Regulator Mode Register --------
#define AT91C_VREG_PSTDBY     ((unsigned int) 0x1 <<  0) // (VREG) Voltage Regulator Power Standby Mode

// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR Memory Controller Interface
// *****************************************************************************
typedef struct _AT91S_MC {
	at91_reg_t	 MC_RCR; 	// MC Remap Control Register
	at91_reg_t	 MC_ASR; 	// MC Abort Status Register
	at91_reg_t	 MC_AASR; 	// MC Abort Address Status Register
	at91_reg_t	 Reserved0[21]; 	//
	at91_reg_t	 MC_FMR; 	// MC Flash Mode Register
	at91_reg_t	 MC_FCR; 	// MC Flash Command Register
	at91_reg_t	 MC_FSR; 	// MC Flash Status Register
} AT91S_MC, *AT91PS_MC;

// -------- MC_RCR : (MC Offset: 0x0) MC Remap Control Register --------
#define AT91C_MC_RCB          ((unsigned int) 0x1 <<  0) // (MC) Remap Command Bit
// -------- MC_ASR : (MC Offset: 0x4) MC Abort Status Register --------
#define AT91C_MC_UNDADD       ((unsigned int) 0x1 <<  0) // (MC) Undefined Addess Abort Status
#define AT91C_MC_MISADD       ((unsigned int) 0x1 <<  1) // (MC) Misaligned Addess Abort Status
#define AT91C_MC_ABTSZ        ((unsigned int) 0x3 <<  8) // (MC) Abort Size Status
#define 	AT91C_MC_ABTSZ_BYTE                 ((unsigned int) 0x0 <<  8) // (MC) Byte
#define 	AT91C_MC_ABTSZ_HWORD                ((unsigned int) 0x1 <<  8) // (MC) Half-word
#define 	AT91C_MC_ABTSZ_WORD                 ((unsigned int) 0x2 <<  8) // (MC) Word
#define AT91C_MC_ABTTYP       ((unsigned int) 0x3 << 10) // (MC) Abort Type Status
#define 	AT91C_MC_ABTTYP_DATAR                ((unsigned int) 0x0 << 10) // (MC) Data Read
#define 	AT91C_MC_ABTTYP_DATAW                ((unsigned int) 0x1 << 10) // (MC) Data Write
#define 	AT91C_MC_ABTTYP_FETCH                ((unsigned int) 0x2 << 10) // (MC) Code Fetch
#define AT91C_MC_MST0         ((unsigned int) 0x1 << 16) // (MC) Master 0 Abort Source
#define AT91C_MC_MST1         ((unsigned int) 0x1 << 17) // (MC) Master 1 Abort Source
#define AT91C_MC_SVMST0       ((unsigned int) 0x1 << 24) // (MC) Saved Master 0 Abort Source
#define AT91C_MC_SVMST1       ((unsigned int) 0x1 << 25) // (MC) Saved Master 1 Abort Source
// -------- MC_FMR : (MC Offset: 0x60) MC Flash Mode Register --------
#define AT91C_MC_FRDY         ((unsigned int) 0x1 <<  0) // (MC) Flash Ready
#define AT91C_MC_LOCKE        ((unsigned int) 0x1 <<  2) // (MC) Lock Error
#define AT91C_MC_PROGE        ((unsigned int) 0x1 <<  3) // (MC) Programming Error
#define AT91C_MC_NEBP         ((unsigned int) 0x1 <<  7) // (MC) No Erase Before Programming
#define AT91C_MC_FWS          ((unsigned int) 0x3 <<  8) // (MC) Flash Wait State
#define 	AT91C_MC_FWS_0FWS                 ((unsigned int) 0x0 <<  8) // (MC) 1 cycle for Read, 2 for Write operations
#define 	AT91C_MC_FWS_1FWS                 ((unsigned int) 0x1 <<  8) // (MC) 2 cycles for Read, 3 for Write operations
#define 	AT91C_MC_FWS_2FWS                 ((unsigned int) 0x2 <<  8) // (MC) 3 cycles for Read, 4 for Write operations
#define 	AT91C_MC_FWS_3FWS                 ((unsigned int) 0x3 <<  8) // (MC) 4 cycles for Read, 4 for Write operations
#define AT91C_MC_FMCN         ((unsigned int) 0xFF << 16) // (MC) Flash Microsecond Cycle Number
// -------- MC_FCR : (MC Offset: 0x64) MC Flash Command Register --------
#define AT91C_MC_FCMD         ((unsigned int) 0xF <<  0) // (MC) Flash Command
#define 	AT91C_MC_FCMD_START_PROG           ((unsigned int) 0x1) // (MC) Starts the programming of th epage specified by PAGEN.
#define 	AT91C_MC_FCMD_LOCK                 ((unsigned int) 0x2) // (MC) Starts a lock sequence of the sector defined by the bits 4 to 7 of the field PAGEN.
#define 	AT91C_MC_FCMD_PROG_AND_LOCK        ((unsigned int) 0x3) // (MC) The lock sequence automatically happens after the programming sequence is completed.
#define 	AT91C_MC_FCMD_UNLOCK               ((unsigned int) 0x4) // (MC) Starts an unlock sequence of the sector defined by the bits 4 to 7 of the field PAGEN.
#define 	AT91C_MC_FCMD_ERASE_ALL            ((unsigned int) 0x8) // (MC) Starts the erase of the entire flash.If at least a page is locked, the command is cancelled.
#define 	AT91C_MC_FCMD_SET_GP_NVM           ((unsigned int) 0xB) // (MC) Set General Purpose NVM bits.
#define 	AT91C_MC_FCMD_CLR_GP_NVM           ((unsigned int) 0xD) // (MC) Clear General Purpose NVM bits.
#define 	AT91C_MC_FCMD_SET_SECURITY         ((unsigned int) 0xF) // (MC) Set Security Bit.
#define AT91C_MC_PAGEN        ((unsigned int) 0x3FF <<  8) // (MC) Page Number
#define AT91C_MC_KEY          ((unsigned int) 0xFF << 24) // (MC) Writing Protect Key
// -------- MC_FSR : (MC Offset: 0x68) MC Flash Command Register --------
#define AT91C_MC_SECURITY     ((unsigned int) 0x1 <<  4) // (MC) Security Bit Status
#define AT91C_MC_GPNVM0       ((unsigned int) 0x1 <<  8) // (MC) Sector 0 Lock Status
#define AT91C_MC_GPNVM1       ((unsigned int) 0x1 <<  9) // (MC) Sector 1 Lock Status
#define AT91C_MC_GPNVM2       ((unsigned int) 0x1 << 10) // (MC) Sector 2 Lock Status
#define AT91C_MC_GPNVM3       ((unsigned int) 0x1 << 11) // (MC) Sector 3 Lock Status
#define AT91C_MC_GPNVM4       ((unsigned int) 0x1 << 12) // (MC) Sector 4 Lock Status
#define AT91C_MC_GPNVM5       ((unsigned int) 0x1 << 13) // (MC) Sector 5 Lock Status
#define AT91C_MC_GPNVM6       ((unsigned int) 0x1 << 14) // (MC) Sector 6 Lock Status
#define AT91C_MC_GPNVM7       ((unsigned int) 0x1 << 15) // (MC) Sector 7 Lock Status
#define AT91C_MC_LOCKS0       ((unsigned int) 0x1 << 16) // (MC) Sector 0 Lock Status
#define AT91C_MC_LOCKS1       ((unsigned int) 0x1 << 17) // (MC) Sector 1 Lock Status
#define AT91C_MC_LOCKS2       ((unsigned int) 0x1 << 18) // (MC) Sector 2 Lock Status
#define AT91C_MC_LOCKS3       ((unsigned int) 0x1 << 19) // (MC) Sector 3 Lock Status
#define AT91C_MC_LOCKS4       ((unsigned int) 0x1 << 20) // (MC) Sector 4 Lock Status
#define AT91C_MC_LOCKS5       ((unsigned int) 0x1 << 21) // (MC) Sector 5 Lock Status
#define AT91C_MC_LOCKS6       ((unsigned int) 0x1 << 22) // (MC) Sector 6 Lock Status
#define AT91C_MC_LOCKS7       ((unsigned int) 0x1 << 23) // (MC) Sector 7 Lock Status
#define AT91C_MC_LOCKS8       ((unsigned int) 0x1 << 24) // (MC) Sector 8 Lock Status
#define AT91C_MC_LOCKS9       ((unsigned int) 0x1 << 25) // (MC) Sector 9 Lock Status
#define AT91C_MC_LOCKS10      ((unsigned int) 0x1 << 26) // (MC) Sector 10 Lock Status
#define AT91C_MC_LOCKS11      ((unsigned int) 0x1 << 27) // (MC) Sector 11 Lock Status
#define AT91C_MC_LOCKS12      ((unsigned int) 0x1 << 28) // (MC) Sector 12 Lock Status
#define AT91C_MC_LOCKS13      ((unsigned int) 0x1 << 29) // (MC) Sector 13 Lock Status
#define AT91C_MC_LOCKS14      ((unsigned int) 0x1 << 30) // (MC) Sector 14 Lock Status
#define AT91C_MC_LOCKS15      ((unsigned int) 0x1 << 31) // (MC) Sector 15 Lock Status

// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR Serial Parallel Interface
// *****************************************************************************
typedef struct _AT91S_SPI {
	at91_reg_t	 SPI_CR; 	// Control Register
	at91_reg_t	 SPI_MR; 	// Mode Register
	at91_reg_t	 SPI_RDR; 	// Receive Data Register
	at91_reg_t	 SPI_TDR; 	// Transmit Data Register
	at91_reg_t	 SPI_SR; 	// Status Register
	at91_reg_t	 SPI_IER; 	// Interrupt Enable Register
	at91_reg_t	 SPI_IDR; 	// Interrupt Disable Register
	at91_reg_t	 SPI_IMR; 	// Interrupt Mask Register
	at91_reg_t	 Reserved0[4]; 	//
	at91_reg_t	 SPI_CSR[4]; 	// Chip Select Register
	at91_reg_t	 Reserved1[48]; 	//
	at91_reg_t	 SPI_RPR; 	// Receive Pointer Register
	at91_reg_t	 SPI_RCR; 	// Receive Counter Register
	at91_reg_t	 SPI_TPR; 	// Transmit Pointer Register
	at91_reg_t	 SPI_TCR; 	// Transmit Counter Register
	at91_reg_t	 SPI_RNPR; 	// Receive Next Pointer Register
	at91_reg_t	 SPI_RNCR; 	// Receive Next Counter Register
	at91_reg_t	 SPI_TNPR; 	// Transmit Next Pointer Register
	at91_reg_t	 SPI_TNCR; 	// Transmit Next Counter Register
	at91_reg_t	 SPI_PTCR; 	// PDC Transfer Control Register
	at91_reg_t	 SPI_PTSR; 	// PDC Transfer Status Register
} AT91S_SPI, *AT91PS_SPI;

// -------- SPI_CR : (SPI Offset: 0x0) SPI Control Register --------
#define AT91C_SPI_SPIEN       ((unsigned int) 0x1 <<  0) // (SPI) SPI Enable
#define AT91C_SPI_SPIDIS      ((unsigned int) 0x1 <<  1) // (SPI) SPI Disable
#define AT91C_SPI_SWRST       ((unsigned int) 0x1 <<  7) // (SPI) SPI Software reset
#define AT91C_SPI_LASTXFER    ((unsigned int) 0x1 << 24) // (SPI) SPI Last Transfer
// -------- SPI_MR : (SPI Offset: 0x4) SPI Mode Register --------
#define AT91C_SPI_MSTR        ((unsigned int) 0x1 <<  0) // (SPI) Master/Slave Mode
#define AT91C_SPI_PS          ((unsigned int) 0x1 <<  1) // (SPI) Peripheral Select
#define 	AT91C_SPI_PS_FIXED                ((unsigned int) 0x0 <<  1) // (SPI) Fixed Peripheral Select
#define 	AT91C_SPI_PS_VARIABLE             ((unsigned int) 0x1 <<  1) // (SPI) Variable Peripheral Select
#define AT91C_SPI_PCSDEC      ((unsigned int) 0x1 <<  2) // (SPI) Chip Select Decode
#define AT91C_SPI_FDIV        ((unsigned int) 0x1 <<  3) // (SPI) Clock Selection
#define AT91C_SPI_MODFDIS     ((unsigned int) 0x1 <<  4) // (SPI) Mode Fault Detection
#define AT91C_SPI_LLB         ((unsigned int) 0x1 <<  7) // (SPI) Clock Selection
#define AT91C_SPI_PCS         ((unsigned int) 0xF << 16) // (SPI) Peripheral Chip Select
#define AT91C_SPI_DLYBCS      ((unsigned int) 0xFF << 24) // (SPI) Delay Between Chip Selects
// -------- SPI_RDR : (SPI Offset: 0x8) Receive Data Register --------
#define AT91C_SPI_RD          ((unsigned int) 0xFFFF <<  0) // (SPI) Receive Data
#define AT91C_SPI_RPCS        ((unsigned int) 0xF << 16) // (SPI) Peripheral Chip Select Status
// -------- SPI_TDR : (SPI Offset: 0xc) Transmit Data Register --------
#define AT91C_SPI_TD          ((unsigned int) 0xFFFF <<  0) // (SPI) Transmit Data
#define AT91C_SPI_TPCS        ((unsigned int) 0xF << 16) // (SPI) Peripheral Chip Select Status
// -------- SPI_SR : (SPI Offset: 0x10) Status Register --------
#define AT91C_SPI_RDRF        ((unsigned int) 0x1 <<  0) // (SPI) Receive Data Register Full
#define AT91C_SPI_TDRE        ((unsigned int) 0x1 <<  1) // (SPI) Transmit Data Register Empty
#define AT91C_SPI_MODF        ((unsigned int) 0x1 <<  2) // (SPI) Mode Fault Error
#define AT91C_SPI_OVRES       ((unsigned int) 0x1 <<  3) // (SPI) Overrun Error Status
#define AT91C_SPI_ENDRX       ((unsigned int) 0x1 <<  4) // (SPI) End of Receiver Transfer
#define AT91C_SPI_ENDTX       ((unsigned int) 0x1 <<  5) // (SPI) End of Receiver Transfer
#define AT91C_SPI_RXBUFF      ((unsigned int) 0x1 <<  6) // (SPI) RXBUFF Interrupt
#define AT91C_SPI_TXBUFE      ((unsigned int) 0x1 <<  7) // (SPI) TXBUFE Interrupt
#define AT91C_SPI_NSSR        ((unsigned int) 0x1 <<  8) // (SPI) NSSR Interrupt
#define AT91C_SPI_TXEMPTY     ((unsigned int) 0x1 <<  9) // (SPI) TXEMPTY Interrupt
#define AT91C_SPI_SPIENS      ((unsigned int) 0x1 << 16) // (SPI) Enable Status
// -------- SPI_IER : (SPI Offset: 0x14) Interrupt Enable Register --------
// -------- SPI_IDR : (SPI Offset: 0x18) Interrupt Disable Register --------
// -------- SPI_IMR : (SPI Offset: 0x1c) Interrupt Mask Register --------
// -------- SPI_CSR : (SPI Offset: 0x30) Chip Select Register --------
#define AT91C_SPI_CPOL        ((unsigned int) 0x1 <<  0) // (SPI) Clock Polarity
#define AT91C_SPI_NCPHA       ((unsigned int) 0x1 <<  1) // (SPI) Clock Phase
#define AT91C_SPI_CSAAT       ((unsigned int) 0x1 <<  3) // (SPI) Chip Select Active After Transfer
#define AT91C_SPI_BITS        ((unsigned int) 0xF <<  4) // (SPI) Bits Per Transfer
#define 	AT91C_SPI_BITS_8                    ((unsigned int) 0x0 <<  4) // (SPI) 8 Bits Per transfer
#define 	AT91C_SPI_BITS_9                    ((unsigned int) 0x1 <<  4) // (SPI) 9 Bits Per transfer
#define 	AT91C_SPI_BITS_10                   ((unsigned int) 0x2 <<  4) // (SPI) 10 Bits Per transfer
#define 	AT91C_SPI_BITS_11                   ((unsigned int) 0x3 <<  4) // (SPI) 11 Bits Per transfer
#define 	AT91C_SPI_BITS_12                   ((unsigned int) 0x4 <<  4) // (SPI) 12 Bits Per transfer
#define 	AT91C_SPI_BITS_13                   ((unsigned int) 0x5 <<  4) // (SPI) 13 Bits Per transfer
#define 	AT91C_SPI_BITS_14                   ((unsigned int) 0x6 <<  4) // (SPI) 14 Bits Per transfer
#define 	AT91C_SPI_BITS_15                   ((unsigned int) 0x7 <<  4) // (SPI) 15 Bits Per transfer
#define 	AT91C_SPI_BITS_16                   ((unsigned int) 0x8 <<  4) // (SPI) 16 Bits Per transfer
#define AT91C_SPI_SCBR        ((unsigned int) 0xFF <<  8) // (SPI) Serial Clock Baud Rate
#define AT91C_SPI_DLYBS       ((unsigned int) 0xFF << 16) // (SPI) Serial Clock Baud Rate
#define AT91C_SPI_DLYBCT      ((unsigned int) 0xFF << 24) // (SPI) Delay Between Consecutive Transfers

// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR Analog to Digital Convertor
// *****************************************************************************
typedef struct _AT91S_ADC {
	at91_reg_t	 ADC_CR; 	// ADC Control Register
	at91_reg_t	 ADC_MR; 	// ADC Mode Register
	at91_reg_t	 Reserved0[2]; 	//
	at91_reg_t	 ADC_CHER; 	// ADC Channel Enable Register
	at91_reg_t	 ADC_CHDR; 	// ADC Channel Disable Register
	at91_reg_t	 ADC_CHSR; 	// ADC Channel Status Register
	at91_reg_t	 ADC_SR; 	// ADC Status Register
	at91_reg_t	 ADC_LCDR; 	// ADC Last Converted Data Register
	at91_reg_t	 ADC_IER; 	// ADC Interrupt Enable Register
	at91_reg_t	 ADC_IDR; 	// ADC Interrupt Disable Register
	at91_reg_t	 ADC_IMR; 	// ADC Interrupt Mask Register
	at91_reg_t	 ADC_CDR0; 	// ADC Channel Data Register 0
	at91_reg_t	 ADC_CDR1; 	// ADC Channel Data Register 1
	at91_reg_t	 ADC_CDR2; 	// ADC Channel Data Register 2
	at91_reg_t	 ADC_CDR3; 	// ADC Channel Data Register 3
	at91_reg_t	 ADC_CDR4; 	// ADC Channel Data Register 4
	at91_reg_t	 ADC_CDR5; 	// ADC Channel Data Register 5
	at91_reg_t	 ADC_CDR6; 	// ADC Channel Data Register 6
	at91_reg_t	 ADC_CDR7; 	// ADC Channel Data Register 7
	at91_reg_t	 Reserved1[44]; 	//
	at91_reg_t	 ADC_RPR; 	// Receive Pointer Register
	at91_reg_t	 ADC_RCR; 	// Receive Counter Register
	at91_reg_t	 ADC_TPR; 	// Transmit Pointer Register
	at91_reg_t	 ADC_TCR; 	// Transmit Counter Register
	at91_reg_t	 ADC_RNPR; 	// Receive Next Pointer Register
	at91_reg_t	 ADC_RNCR; 	// Receive Next Counter Register
	at91_reg_t	 ADC_TNPR; 	// Transmit Next Pointer Register
	at91_reg_t	 ADC_TNCR; 	// Transmit Next Counter Register
	at91_reg_t	 ADC_PTCR; 	// PDC Transfer Control Register
	at91_reg_t	 ADC_PTSR; 	// PDC Transfer Status Register
} AT91S_ADC, *AT91PS_ADC;

// -------- ADC_CR : (ADC Offset: 0x0) ADC Control Register --------
#define AT91C_ADC_SWRST       ((unsigned int) 0x1 <<  0) // (ADC) Software Reset
#define AT91C_ADC_START       ((unsigned int) 0x1 <<  1) // (ADC) Start Conversion
// -------- ADC_MR : (ADC Offset: 0x4) ADC Mode Register --------
#define AT91C_ADC_TRGEN       ((unsigned int) 0x1 <<  0) // (ADC) Trigger Enable
#define 	AT91C_ADC_TRGEN_DIS                  ((unsigned int) 0x0) // (ADC) Hradware triggers are disabled. Starting a conversion is only possible by software
#define 	AT91C_ADC_TRGEN_EN                   ((unsigned int) 0x1) // (ADC) Hardware trigger selected by TRGSEL field is enabled.
#define AT91C_ADC_TRGSEL      ((unsigned int) 0x7 <<  1) // (ADC) Trigger Selection
#define 	AT91C_ADC_TRGSEL_TIOA0                ((unsigned int) 0x0 <<  1) // (ADC) Selected TRGSEL = TIAO0
#define 	AT91C_ADC_TRGSEL_TIOA1                ((unsigned int) 0x1 <<  1) // (ADC) Selected TRGSEL = TIAO1
#define 	AT91C_ADC_TRGSEL_TIOA2                ((unsigned int) 0x2 <<  1) // (ADC) Selected TRGSEL = TIAO2
#define 	AT91C_ADC_TRGSEL_TIOA3                ((unsigned int) 0x3 <<  1) // (ADC) Selected TRGSEL = TIAO3
#define 	AT91C_ADC_TRGSEL_TIOA4                ((unsigned int) 0x4 <<  1) // (ADC) Selected TRGSEL = TIAO4
#define 	AT91C_ADC_TRGSEL_TIOA5                ((unsigned int) 0x5 <<  1) // (ADC) Selected TRGSEL = TIAO5
#define 	AT91C_ADC_TRGSEL_EXT                  ((unsigned int) 0x6 <<  1) // (ADC) Selected TRGSEL = External Trigger
#define AT91C_ADC_LOWRES      ((unsigned int) 0x1 <<  4) // (ADC) Resolution.
#define 	AT91C_ADC_LOWRES_10_BIT               ((unsigned int) 0x0 <<  4) // (ADC) 10-bit resolution
#define 	AT91C_ADC_LOWRES_8_BIT                ((unsigned int) 0x1 <<  4) // (ADC) 8-bit resolution
#define AT91C_ADC_SLEEP       ((unsigned int) 0x1 <<  5) // (ADC) Sleep Mode
#define 	AT91C_ADC_SLEEP_NORMAL_MODE          ((unsigned int) 0x0 <<  5) // (ADC) Normal Mode
#define 	AT91C_ADC_SLEEP_MODE                 ((unsigned int) 0x1 <<  5) // (ADC) Sleep Mode
#define AT91C_ADC_PRESCAL     ((unsigned int) 0x3F <<  8) // (ADC) Prescaler rate selection
#define AT91C_ADC_STARTUP     ((unsigned int) 0x1F << 16) // (ADC) Startup Time
#define AT91C_ADC_SHTIM       ((unsigned int) 0xF << 24) // (ADC) Sample & Hold Time
// -------- 	ADC_CHER : (ADC Offset: 0x10) ADC Channel Enable Register --------
#define AT91C_ADC_CH0         ((unsigned int) 0x1 <<  0) // (ADC) Channel 0
#define AT91C_ADC_CH1         ((unsigned int) 0x1 <<  1) // (ADC) Channel 1
#define AT91C_ADC_CH2         ((unsigned int) 0x1 <<  2) // (ADC) Channel 2
#define AT91C_ADC_CH3         ((unsigned int) 0x1 <<  3) // (ADC) Channel 3
#define AT91C_ADC_CH4         ((unsigned int) 0x1 <<  4) // (ADC) Channel 4
#define AT91C_ADC_CH5         ((unsigned int) 0x1 <<  5) // (ADC) Channel 5
#define AT91C_ADC_CH6         ((unsigned int) 0x1 <<  6) // (ADC) Channel 6
#define AT91C_ADC_CH7         ((unsigned int) 0x1 <<  7) // (ADC) Channel 7
// -------- 	ADC_CHDR : (ADC Offset: 0x14) ADC Channel Disable Register --------
// -------- 	ADC_CHSR : (ADC Offset: 0x18) ADC Channel Status Register --------
// -------- ADC_SR : (ADC Offset: 0x1c) ADC Status Register --------
#define AT91C_ADC_EOC0        ((unsigned int) 0x1 <<  0) // (ADC) End of Conversion
#define AT91C_ADC_EOC1        ((unsigned int) 0x1 <<  1) // (ADC) End of Conversion
#define AT91C_ADC_EOC2        ((unsigned int) 0x1 <<  2) // (ADC) End of Conversion
#define AT91C_ADC_EOC3        ((unsigned int) 0x1 <<  3) // (ADC) End of Conversion
#define AT91C_ADC_EOC4        ((unsigned int) 0x1 <<  4) // (ADC) End of Conversion
#define AT91C_ADC_EOC5        ((unsigned int) 0x1 <<  5) // (ADC) End of Conversion
#define AT91C_ADC_EOC6        ((unsigned int) 0x1 <<  6) // (ADC) End of Conversion
#define AT91C_ADC_EOC7        ((unsigned int) 0x1 <<  7) // (ADC) End of Conversion
#define AT91C_ADC_OVRE0       ((unsigned int) 0x1 <<  8) // (ADC) Overrun Error
#define AT91C_ADC_OVRE1       ((unsigned int) 0x1 <<  9) // (ADC) Overrun Error
#define AT91C_ADC_OVRE2       ((unsigned int) 0x1 << 10) // (ADC) Overrun Error
#define AT91C_ADC_OVRE3       ((unsigned int) 0x1 << 11) // (ADC) Overrun Error
#define AT91C_ADC_OVRE4       ((unsigned int) 0x1 << 12) // (ADC) Overrun Error
#define AT91C_ADC_OVRE5       ((unsigned int) 0x1 << 13) // (ADC) Overrun Error
#define AT91C_ADC_OVRE6       ((unsigned int) 0x1 << 14) // (ADC) Overrun Error
#define AT91C_ADC_OVRE7       ((unsigned int) 0x1 << 15) // (ADC) Overrun Error
#define AT91C_ADC_DRDY        ((unsigned int) 0x1 << 16) // (ADC) Data Ready
#define AT91C_ADC_GOVRE       ((unsigned int) 0x1 << 17) // (ADC) General Overrun
#define AT91C_ADC_ENDRX       ((unsigned int) 0x1 << 18) // (ADC) End of Receiver Transfer
#define AT91C_ADC_RXBUFF      ((unsigned int) 0x1 << 19) // (ADC) RXBUFF Interrupt
// -------- ADC_LCDR : (ADC Offset: 0x20) ADC Last Converted Data Register --------
#define AT91C_ADC_LDATA       ((unsigned int) 0x3FF <<  0) // (ADC) Last Data Converted
// -------- ADC_IER : (ADC Offset: 0x24) ADC Interrupt Enable Register --------
// -------- ADC_IDR : (ADC Offset: 0x28) ADC Interrupt Disable Register --------
// -------- ADC_IMR : (ADC Offset: 0x2c) ADC Interrupt Mask Register --------
// -------- ADC_CDR0 : (ADC Offset: 0x30) ADC Channel Data Register 0 --------
#define AT91C_ADC_DATA        ((unsigned int) 0x3FF <<  0) // (ADC) Converted Data
// -------- ADC_CDR1 : (ADC Offset: 0x34) ADC Channel Data Register 1 --------
// -------- ADC_CDR2 : (ADC Offset: 0x38) ADC Channel Data Register 2 --------
// -------- ADC_CDR3 : (ADC Offset: 0x3c) ADC Channel Data Register 3 --------
// -------- ADC_CDR4 : (ADC Offset: 0x40) ADC Channel Data Register 4 --------
// -------- ADC_CDR5 : (ADC Offset: 0x44) ADC Channel Data Register 5 --------
// -------- ADC_CDR6 : (ADC Offset: 0x48) ADC Channel Data Register 6 --------
// -------- ADC_CDR7 : (ADC Offset: 0x4c) ADC Channel Data Register 7 --------

// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR Synchronous Serial Controller Interface
// *****************************************************************************
#include <drivers/at91sam7_ssc.h>

// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR Two-wire Interface
// *****************************************************************************
typedef struct _AT91S_TWI {
	at91_reg_t	 TWI_CR; 	// Control Register
	at91_reg_t	 TWI_MMR; 	// Master Mode Register
	at91_reg_t	 Reserved0[1]; 	//
	at91_reg_t	 TWI_IADR; 	// Internal Address Register
	at91_reg_t	 TWI_CWGR; 	// Clock Waveform Generator Register
	at91_reg_t	 Reserved1[3]; 	//
	at91_reg_t	 TWI_SR; 	// Status Register
	at91_reg_t	 TWI_IER; 	// Interrupt Enable Register
	at91_reg_t	 TWI_IDR; 	// Interrupt Disable Register
	at91_reg_t	 TWI_IMR; 	// Interrupt Mask Register
	at91_reg_t	 TWI_RHR; 	// Receive Holding Register
	at91_reg_t	 TWI_THR; 	// Transmit Holding Register
} AT91S_TWI, *AT91PS_TWI;

// -------- TWI_CR : (TWI Offset: 0x0) TWI Control Register --------
#define AT91C_TWI_START       ((unsigned int) 0x1 <<  0) // (TWI) Send a START Condition
#define AT91C_TWI_STOP        ((unsigned int) 0x1 <<  1) // (TWI) Send a STOP Condition
#define AT91C_TWI_MSEN        ((unsigned int) 0x1 <<  2) // (TWI) TWI Master Transfer Enabled
#define AT91C_TWI_MSDIS       ((unsigned int) 0x1 <<  3) // (TWI) TWI Master Transfer Disabled
#define AT91C_TWI_SWRST       ((unsigned int) 0x1 <<  7) // (TWI) Software Reset
// -------- TWI_MMR : (TWI Offset: 0x4) TWI Master Mode Register --------
#define AT91C_TWI_IADRSZ      ((unsigned int) 0x3 <<  8) // (TWI) Internal Device Address Size
#define 	AT91C_TWI_IADRSZ_NO                   ((unsigned int) 0x0 <<  8) // (TWI) No internal device address
#define 	AT91C_TWI_IADRSZ_1_BYTE               ((unsigned int) 0x1 <<  8) // (TWI) One-byte internal device address
#define 	AT91C_TWI_IADRSZ_2_BYTE               ((unsigned int) 0x2 <<  8) // (TWI) Two-byte internal device address
#define 	AT91C_TWI_IADRSZ_3_BYTE               ((unsigned int) 0x3 <<  8) // (TWI) Three-byte internal device address
#define AT91C_TWI_MREAD       ((unsigned int) 0x1 << 12) // (TWI) Master Read Direction
#define AT91C_TWI_DADR        ((unsigned int) 0x7F << 16) // (TWI) Device Address
// -------- TWI_CWGR : (TWI Offset: 0x10) TWI Clock Waveform Generator Register --------
#define AT91C_TWI_CLDIV       ((unsigned int) 0xFF <<  0) // (TWI) Clock Low Divider
#define AT91C_TWI_CHDIV       ((unsigned int) 0xFF <<  8) // (TWI) Clock High Divider
#define AT91C_TWI_CKDIV       ((unsigned int) 0x7 << 16) // (TWI) Clock Divider
// -------- TWI_SR : (TWI Offset: 0x20) TWI Status Register --------
#define AT91C_TWI_TXCOMP      ((unsigned int) 0x1 <<  0) // (TWI) Transmission Completed
#define AT91C_TWI_RXRDY       ((unsigned int) 0x1 <<  1) // (TWI) Receive holding register ReaDY
#define AT91C_TWI_TXRDY       ((unsigned int) 0x1 <<  2) // (TWI) Transmit holding register ReaDY
#define AT91C_TWI_OVRE        ((unsigned int) 0x1 <<  6) // (TWI) Overrun Error
#define AT91C_TWI_UNRE        ((unsigned int) 0x1 <<  7) // (TWI) Underrun Error
#define AT91C_TWI_NACK        ((unsigned int) 0x1 <<  8) // (TWI) Not Acknowledged
// -------- TWI_IER : (TWI Offset: 0x24) TWI Interrupt Enable Register --------
// -------- TWI_IDR : (TWI Offset: 0x28) TWI Interrupt Disable Register --------
// -------- TWI_IMR : (TWI Offset: 0x2c) TWI Interrupt Mask Register --------


// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR Timer Counter Channel Interface
// *****************************************************************************
#include <drivers/at91sam7_tcc.h>

// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR PWMC Channel Interface
// *****************************************************************************
typedef struct _AT91S_PWMC_CH {
	at91_reg_t	 PWMC_CMR; 	// Channel Mode Register
	at91_reg_t	 PWMC_CDTYR; 	// Channel Duty Cycle Register
	at91_reg_t	 PWMC_CPRDR; 	// Channel Period Register
	at91_reg_t	 PWMC_CCNTR; 	// Channel Counter Register
	at91_reg_t	 PWMC_CUPDR; 	// Channel Update Register
	at91_reg_t	 PWMC_Reserved[3]; 	// Reserved
} AT91S_PWMC_CH, *AT91PS_PWMC_CH;

// -------- PWMC_CMR : (PWMC_CH Offset: 0x0) PWMC Channel Mode Register --------
#define AT91C_PWMC_CPRE       ((unsigned int) 0xF <<  0) // (PWMC_CH) Channel Pre-scaler : PWMC_CLKx
#define 	AT91C_PWMC_CPRE_MCK                  ((unsigned int) 0x0) // (PWMC_CH)
#define 	AT91C_PWMC_CPRE_MCKA                 ((unsigned int) 0xB) // (PWMC_CH)
#define 	AT91C_PWMC_CPRE_MCKB                 ((unsigned int) 0xC) // (PWMC_CH)
#define AT91C_PWMC_CALG       ((unsigned int) 0x1 <<  8) // (PWMC_CH) Channel Alignment
#define AT91C_PWMC_CPOL       ((unsigned int) 0x1 <<  9) // (PWMC_CH) Channel Polarity
#define AT91C_PWMC_CPD        ((unsigned int) 0x1 << 10) // (PWMC_CH) Channel Update Period
// -------- PWMC_CDTYR : (PWMC_CH Offset: 0x4) PWMC Channel Duty Cycle Register --------
#define AT91C_PWMC_CDTY       ((unsigned int) 0x0 <<  0) // (PWMC_CH) Channel Duty Cycle
// -------- PWMC_CPRDR : (PWMC_CH Offset: 0x8) PWMC Channel Period Register --------
#define AT91C_PWMC_CPRD       ((unsigned int) 0x0 <<  0) // (PWMC_CH) Channel Period
// -------- PWMC_CCNTR : (PWMC_CH Offset: 0xc) PWMC Channel Counter Register --------
#define AT91C_PWMC_CCNT       ((unsigned int) 0x0 <<  0) // (PWMC_CH) Channel Counter
// -------- PWMC_CUPDR : (PWMC_CH Offset: 0x10) PWMC Channel Update Register --------
#define AT91C_PWMC_CUPD       ((unsigned int) 0x0 <<  0) // (PWMC_CH) Channel Update

// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR Pulse Width Modulation Controller Interface
// *****************************************************************************
typedef struct _AT91S_PWMC {
	at91_reg_t	 PWMC_MR; 	// PWMC Mode Register
	at91_reg_t	 PWMC_ENA; 	// PWMC Enable Register
	at91_reg_t	 PWMC_DIS; 	// PWMC Disable Register
	at91_reg_t	 PWMC_SR; 	// PWMC Status Register
	at91_reg_t	 PWMC_IER; 	// PWMC Interrupt Enable Register
	at91_reg_t	 PWMC_IDR; 	// PWMC Interrupt Disable Register
	at91_reg_t	 PWMC_IMR; 	// PWMC Interrupt Mask Register
	at91_reg_t	 PWMC_ISR; 	// PWMC Interrupt Status Register
	at91_reg_t	 Reserved0[55]; 	//
	at91_reg_t	 PWMC_VR; 	// PWMC Version Register
	at91_reg_t	 Reserved1[64]; 	//
	AT91S_PWMC_CH	 PWMC_CH[32]; 	// PWMC Channel 0
} AT91S_PWMC, *AT91PS_PWMC;

// -------- PWMC_MR : (PWMC Offset: 0x0) PWMC Mode Register --------
#define AT91C_PWMC_DIVA       ((unsigned int) 0xFF <<  0) // (PWMC) CLKA divide factor.
#define AT91C_PWMC_PREA       ((unsigned int) 0xF <<  8) // (PWMC) Divider Input Clock Prescaler A
#define 	AT91C_PWMC_PREA_MCK                  ((unsigned int) 0x0 <<  8) // (PWMC)
#define AT91C_PWMC_DIVB       ((unsigned int) 0xFF << 16) // (PWMC) CLKB divide factor.
#define AT91C_PWMC_PREB       ((unsigned int) 0xF << 24) // (PWMC) Divider Input Clock Prescaler B
#define 	AT91C_PWMC_PREB_MCK                  ((unsigned int) 0x0 << 24) // (PWMC)
// -------- PWMC_ENA : (PWMC Offset: 0x4) PWMC Enable Register --------
#define AT91C_PWMC_CHID0      ((unsigned int) 0x1 <<  0) // (PWMC) Channel ID 0
#define AT91C_PWMC_CHID1      ((unsigned int) 0x1 <<  1) // (PWMC) Channel ID 1
#define AT91C_PWMC_CHID2      ((unsigned int) 0x1 <<  2) // (PWMC) Channel ID 2
#define AT91C_PWMC_CHID3      ((unsigned int) 0x1 <<  3) // (PWMC) Channel ID 3
#define AT91C_PWMC_CHID4      ((unsigned int) 0x1 <<  4) // (PWMC) Channel ID 4
#define AT91C_PWMC_CHID5      ((unsigned int) 0x1 <<  5) // (PWMC) Channel ID 5
#define AT91C_PWMC_CHID6      ((unsigned int) 0x1 <<  6) // (PWMC) Channel ID 6
#define AT91C_PWMC_CHID7      ((unsigned int) 0x1 <<  7) // (PWMC) Channel ID 7
// -------- PWMC_DIS : (PWMC Offset: 0x8) PWMC Disable Register --------
// -------- PWMC_SR : (PWMC Offset: 0xc) PWMC Status Register --------
// -------- PWMC_IER : (PWMC Offset: 0x10) PWMC Interrupt Enable Register --------
// -------- PWMC_IDR : (PWMC Offset: 0x14) PWMC Interrupt Disable Register --------
// -------- PWMC_IMR : (PWMC Offset: 0x18) PWMC Interrupt Mask Register --------
// -------- PWMC_ISR : (PWMC Offset: 0x1c) PWMC Interrupt Status Register --------

// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR USB Device Interface
// *****************************************************************************
typedef struct _AT91S_UDP {
	at91_reg_t	 UDP_NUM; 	// Frame Number Register
	at91_reg_t	 UDP_GLBSTATE; 	// Global State Register
	at91_reg_t	 UDP_FADDR; 	// Function Address Register
	at91_reg_t	 Reserved0[1]; 	//
	at91_reg_t	 UDP_IER; 	// Interrupt Enable Register
	at91_reg_t	 UDP_IDR; 	// Interrupt Disable Register
	at91_reg_t	 UDP_IMR; 	// Interrupt Mask Register
	at91_reg_t	 UDP_ISR; 	// Interrupt Status Register
	at91_reg_t	 UDP_ICR; 	// Interrupt Clear Register
	at91_reg_t	 Reserved1[1]; 	//
	at91_reg_t	 UDP_RSTEP; 	// Reset Endpoint Register
	at91_reg_t	 Reserved2[1]; 	//
	at91_reg_t	 UDP_CSR[8]; 	// Endpoint Control and Status Register
	at91_reg_t	 UDP_FDR[8]; 	// Endpoint FIFO Data Register
	at91_reg_t	 Reserved3[1]; 	//
	at91_reg_t	 UDP_TXVC; 	// Transceiver Control Register
} AT91S_UDP, *AT91PS_UDP;

// -------- UDP_FRM_NUM : (UDP Offset: 0x0) USB Frame Number Register --------
#define AT91C_UDP_FRM_NUM     ((unsigned int) 0x7FF <<  0) // (UDP) Frame Number as Defined in the Packet Field Formats
#define AT91C_UDP_FRM_ERR     ((unsigned int) 0x1 << 16) // (UDP) Frame Error
#define AT91C_UDP_FRM_OK      ((unsigned int) 0x1 << 17) // (UDP) Frame OK
// -------- UDP_GLB_STATE : (UDP Offset: 0x4) USB Global State Register --------
#define AT91C_UDP_FADDEN      ((unsigned int) 0x1 <<  0) // (UDP) Function Address Enable
#define AT91C_UDP_CONFG       ((unsigned int) 0x1 <<  1) // (UDP) Configured
#define AT91C_UDP_ESR         ((unsigned int) 0x1 <<  2) // (UDP) Enable Send Resume
#define AT91C_UDP_RSMINPR     ((unsigned int) 0x1 <<  3) // (UDP) A Resume Has Been Sent to the Host
#define AT91C_UDP_RMWUPE      ((unsigned int) 0x1 <<  4) // (UDP) Remote Wake Up Enable
// -------- UDP_FADDR : (UDP Offset: 0x8) USB Function Address Register --------
#define AT91C_UDP_FADD        ((unsigned int) 0xFF <<  0) // (UDP) Function Address Value
#define AT91C_UDP_FEN         ((unsigned int) 0x1 <<  8) // (UDP) Function Enable
// -------- UDP_IER : (UDP Offset: 0x10) USB Interrupt Enable Register --------
#define AT91C_UDP_EPINT0      ((unsigned int) 0x1 <<  0) // (UDP) Endpoint 0 Interrupt
#define AT91C_UDP_EPINT1      ((unsigned int) 0x1 <<  1) // (UDP) Endpoint 0 Interrupt
#define AT91C_UDP_EPINT2      ((unsigned int) 0x1 <<  2) // (UDP) Endpoint 2 Interrupt
#define AT91C_UDP_EPINT3      ((unsigned int) 0x1 <<  3) // (UDP) Endpoint 3 Interrupt
#define AT91C_UDP_EPINT4      ((unsigned int) 0x1 <<  4) // (UDP) Endpoint 4 Interrupt
#define AT91C_UDP_EPINT5      ((unsigned int) 0x1 <<  5) // (UDP) Endpoint 5 Interrupt
#define AT91C_UDP_EPINT6      ((unsigned int) 0x1 <<  6) // (UDP) Endpoint 6 Interrupt
#define AT91C_UDP_EPINT7      ((unsigned int) 0x1 <<  7) // (UDP) Endpoint 7 Interrupt
#define AT91C_UDP_RXSUSP      ((unsigned int) 0x1 <<  8) // (UDP) USB Suspend Interrupt
#define AT91C_UDP_RXRSM       ((unsigned int) 0x1 <<  9) // (UDP) USB Resume Interrupt
#define AT91C_UDP_EXTRSM      ((unsigned int) 0x1 << 10) // (UDP) USB External Resume Interrupt
#define AT91C_UDP_SOFINT      ((unsigned int) 0x1 << 11) // (UDP) USB Start Of frame Interrupt
#define AT91C_UDP_WAKEUP      ((unsigned int) 0x1 << 13) // (UDP) USB Resume Interrupt
// -------- UDP_IDR : (UDP Offset: 0x14) USB Interrupt Disable Register --------
// -------- UDP_IMR : (UDP Offset: 0x18) USB Interrupt Mask Register --------
// -------- UDP_ISR : (UDP Offset: 0x1c) USB Interrupt Status Register --------
#define AT91C_UDP_ENDBUSRES   ((unsigned int) 0x1 << 12) // (UDP) USB End Of Bus Reset Interrupt
// -------- UDP_ICR : (UDP Offset: 0x20) USB Interrupt Clear Register --------
// -------- UDP_RST_EP : (UDP Offset: 0x28) USB Reset Endpoint Register --------
#define AT91C_UDP_EP0         ((unsigned int) 0x1 <<  0) // (UDP) Reset Endpoint 0
#define AT91C_UDP_EP1         ((unsigned int) 0x1 <<  1) // (UDP) Reset Endpoint 1
#define AT91C_UDP_EP2         ((unsigned int) 0x1 <<  2) // (UDP) Reset Endpoint 2
#define AT91C_UDP_EP3         ((unsigned int) 0x1 <<  3) // (UDP) Reset Endpoint 3
#define AT91C_UDP_EP4         ((unsigned int) 0x1 <<  4) // (UDP) Reset Endpoint 4
#define AT91C_UDP_EP5         ((unsigned int) 0x1 <<  5) // (UDP) Reset Endpoint 5
#define AT91C_UDP_EP6         ((unsigned int) 0x1 <<  6) // (UDP) Reset Endpoint 6
#define AT91C_UDP_EP7         ((unsigned int) 0x1 <<  7) // (UDP) Reset Endpoint 7
// -------- UDP_CSR : (UDP Offset: 0x30) USB Endpoint Control and Status Register --------
#define AT91C_UDP_TXCOMP      ((unsigned int) 0x1 <<  0) // (UDP) Generates an IN packet with data previously written in the DPR
#define AT91C_UDP_RX_DATA_BK0 ((unsigned int) 0x1 <<  1) // (UDP) Receive Data Bank 0
#define AT91C_UDP_RXSETUP     ((unsigned int) 0x1 <<  2) // (UDP) Sends STALL to the Host (Control endpoints)
#define AT91C_UDP_ISOERROR    ((unsigned int) 0x1 <<  3) // (UDP) Isochronous error (Isochronous endpoints)
#define AT91C_UDP_TXPKTRDY    ((unsigned int) 0x1 <<  4) // (UDP) Transmit Packet Ready
#define AT91C_UDP_FORCESTALL  ((unsigned int) 0x1 <<  5) // (UDP) Force Stall (used by Control, Bulk and Isochronous endpoints).
#define AT91C_UDP_RX_DATA_BK1 ((unsigned int) 0x1 <<  6) // (UDP) Receive Data Bank 1 (only used by endpoints with ping-pong attributes).
#define AT91C_UDP_DIR         ((unsigned int) 0x1 <<  7) // (UDP) Transfer Direction
#define AT91C_UDP_EPTYPE      ((unsigned int) 0x7 <<  8) // (UDP) Endpoint type
#define 	AT91C_UDP_EPTYPE_CTRL                 ((unsigned int) 0x0 <<  8) // (UDP) Control
#define 	AT91C_UDP_EPTYPE_ISO_OUT              ((unsigned int) 0x1 <<  8) // (UDP) Isochronous OUT
#define 	AT91C_UDP_EPTYPE_BULK_OUT             ((unsigned int) 0x2 <<  8) // (UDP) Bulk OUT
#define 	AT91C_UDP_EPTYPE_INT_OUT              ((unsigned int) 0x3 <<  8) // (UDP) Interrupt OUT
#define 	AT91C_UDP_EPTYPE_ISO_IN               ((unsigned int) 0x5 <<  8) // (UDP) Isochronous IN
#define 	AT91C_UDP_EPTYPE_BULK_IN              ((unsigned int) 0x6 <<  8) // (UDP) Bulk IN
#define 	AT91C_UDP_EPTYPE_INT_IN               ((unsigned int) 0x7 <<  8) // (UDP) Interrupt IN
#define AT91C_UDP_DTGLE       ((unsigned int) 0x1 << 11) // (UDP) Data Toggle
#define AT91C_UDP_EPEDS       ((unsigned int) 0x1 << 15) // (UDP) Endpoint Enable Disable
#define AT91C_UDP_RXBYTECNT   ((unsigned int) 0x7FF << 16) // (UDP) Number Of Bytes Available in the FIFO
// -------- UDP_TXVC : (UDP Offset: 0x74) Transceiver Control Register --------
#define AT91C_UDP_TXVDIS      ((unsigned int) 0x1 <<  8) // (UDP)
#define AT91C_UDP_PUON        ((unsigned int) 0x1 <<  9) // (UDP) Pull-up ON

// *****************************************************************************
//               REGISTER ADDRESS DEFINITION FOR AT91SAM7S256
// *****************************************************************************
// ========== Register definition for SYS peripheral ==========
// ========== Register definition for PDC_DBGU peripheral ==========
#define AT91C_DBGU_TCR  ((AT91_REG *) 	0xFFFFF30C) // (PDC_DBGU) Transmit Counter Register
#define AT91C_DBGU_RNPR ((AT91_REG *) 	0xFFFFF310) // (PDC_DBGU) Receive Next Pointer Register
#define AT91C_DBGU_TNPR ((AT91_REG *) 	0xFFFFF318) // (PDC_DBGU) Transmit Next Pointer Register
#define AT91C_DBGU_TPR  ((AT91_REG *) 	0xFFFFF308) // (PDC_DBGU) Transmit Pointer Register
#define AT91C_DBGU_RPR  ((AT91_REG *) 	0xFFFFF300) // (PDC_DBGU) Receive Pointer Register
#define AT91C_DBGU_RCR  ((AT91_REG *) 	0xFFFFF304) // (PDC_DBGU) Receive Counter Register
#define AT91C_DBGU_RNCR ((AT91_REG *) 	0xFFFFF314) // (PDC_DBGU) Receive Next Counter Register
#define AT91C_DBGU_PTCR ((AT91_REG *) 	0xFFFFF320) // (PDC_DBGU) PDC Transfer Control Register
#define AT91C_DBGU_PTSR ((AT91_REG *) 	0xFFFFF324) // (PDC_DBGU) PDC Transfer Status Register
#define AT91C_DBGU_TNCR ((AT91_REG *) 	0xFFFFF31C) // (PDC_DBGU) Transmit Next Counter Register
// ========== Register definition for DBGU peripheral ==========
#define AT91C_DBGU_EXID ((AT91_REG *) 	0xFFFFF244) // (DBGU) Chip ID Extension Register
#define AT91C_DBGU_BRGR ((AT91_REG *) 	0xFFFFF220) // (DBGU) Baud Rate Generator Register
#define AT91C_DBGU_IDR  ((AT91_REG *) 	0xFFFFF20C) // (DBGU) Interrupt Disable Register
#define AT91C_DBGU_CSR  ((AT91_REG *) 	0xFFFFF214) // (DBGU) Channel Status Register
#define AT91C_DBGU_CIDR ((AT91_REG *) 	0xFFFFF240) // (DBGU) Chip ID Register
#define AT91C_DBGU_MR   ((AT91_REG *) 	0xFFFFF204) // (DBGU) Mode Register
#define AT91C_DBGU_IMR  ((AT91_REG *) 	0xFFFFF210) // (DBGU) Interrupt Mask Register
#define AT91C_DBGU_CR   ((AT91_REG *) 	0xFFFFF200) // (DBGU) Control Register
#define AT91C_DBGU_FNTR ((AT91_REG *) 	0xFFFFF248) // (DBGU) Force NTRST Register
#define AT91C_DBGU_THR  ((AT91_REG *) 	0xFFFFF21C) // (DBGU) Transmitter Holding Register
#define AT91C_DBGU_RHR  ((AT91_REG *) 	0xFFFFF218) // (DBGU) Receiver Holding Register
#define AT91C_DBGU_IER  ((AT91_REG *) 	0xFFFFF208) // (DBGU) Interrupt Enable Register
// ========== Register definition for RSTC peripheral ==========
#define AT91C_RSTC_RCR  ((AT91_REG *) 	0xFFFFFD00) // (RSTC) Reset Control Register
#define AT91C_RSTC_RMR  ((AT91_REG *) 	0xFFFFFD08) // (RSTC) Reset Mode Register
#define AT91C_RSTC_RSR  ((AT91_REG *) 	0xFFFFFD04) // (RSTC) Reset Status Register
// ========== Register definition for RTTC peripheral ==========
#define AT91C_RTTC_RTSR ((AT91_REG *) 	0xFFFFFD2C) // (RTTC) Real-time Status Register
#define AT91C_RTTC_RTMR ((AT91_REG *) 	0xFFFFFD20) // (RTTC) Real-time Mode Register
#define AT91C_RTTC_RTVR ((AT91_REG *) 	0xFFFFFD28) // (RTTC) Real-time Value Register
#define AT91C_RTTC_RTAR ((AT91_REG *) 	0xFFFFFD24) // (RTTC) Real-time Alarm Register
// ========== Register definition for VREG peripheral ==========
#define AT91C_VREG_MR   ((AT91_REG *) 	0xFFFFFD60) // (VREG) Voltage Regulator Mode Register
// ========== Register definition for MC peripheral ==========
#define AT91C_MC_ASR    ((AT91_REG *) 	0xFFFFFF04) // (MC) MC Abort Status Register
#define AT91C_MC_RCR    ((AT91_REG *) 	0xFFFFFF00) // (MC) MC Remap Control Register
#define AT91C_MC_FCR    ((AT91_REG *) 	0xFFFFFF64) // (MC) MC Flash Command Register
#define AT91C_MC_AASR   ((AT91_REG *) 	0xFFFFFF08) // (MC) MC Abort Address Status Register
#define AT91C_MC_FSR    ((AT91_REG *) 	0xFFFFFF68) // (MC) MC Flash Status Register
#define AT91C_MC_FMR    ((AT91_REG *) 	0xFFFFFF60) // (MC) MC Flash Mode Register
// ========== Register definition for PDC_SPI peripheral ==========
#define AT91C_SPI_PTCR  ((AT91_REG *) 	0xFFFE0120) // (PDC_SPI) PDC Transfer Control Register
#define AT91C_SPI_TPR   ((AT91_REG *) 	0xFFFE0108) // (PDC_SPI) Transmit Pointer Register
#define AT91C_SPI_TCR   ((AT91_REG *) 	0xFFFE010C) // (PDC_SPI) Transmit Counter Register
#define AT91C_SPI_RCR   ((AT91_REG *) 	0xFFFE0104) // (PDC_SPI) Receive Counter Register
#define AT91C_SPI_PTSR  ((AT91_REG *) 	0xFFFE0124) // (PDC_SPI) PDC Transfer Status Register
#define AT91C_SPI_RNPR  ((AT91_REG *) 	0xFFFE0110) // (PDC_SPI) Receive Next Pointer Register
#define AT91C_SPI_RPR   ((AT91_REG *) 	0xFFFE0100) // (PDC_SPI) Receive Pointer Register
#define AT91C_SPI_TNCR  ((AT91_REG *) 	0xFFFE011C) // (PDC_SPI) Transmit Next Counter Register
#define AT91C_SPI_RNCR  ((AT91_REG *) 	0xFFFE0114) // (PDC_SPI) Receive Next Counter Register
#define AT91C_SPI_TNPR  ((AT91_REG *) 	0xFFFE0118) // (PDC_SPI) Transmit Next Pointer Register
// ========== Register definition for SPI peripheral ==========
#define AT91C_SPI_IER   ((AT91_REG *) 	0xFFFE0014) // (SPI) Interrupt Enable Register
#define AT91C_SPI_SR    ((AT91_REG *) 	0xFFFE0010) // (SPI) Status Register
#define AT91C_SPI_IDR   ((AT91_REG *) 	0xFFFE0018) // (SPI) Interrupt Disable Register
#define AT91C_SPI_CR    ((AT91_REG *) 	0xFFFE0000) // (SPI) Control Register
#define AT91C_SPI_MR    ((AT91_REG *) 	0xFFFE0004) // (SPI) Mode Register
#define AT91C_SPI_IMR   ((AT91_REG *) 	0xFFFE001C) // (SPI) Interrupt Mask Register
#define AT91C_SPI_TDR   ((AT91_REG *) 	0xFFFE000C) // (SPI) Transmit Data Register
#define AT91C_SPI_RDR   ((AT91_REG *) 	0xFFFE0008) // (SPI) Receive Data Register
#define AT91C_SPI_CSR   ((AT91_REG *) 	0xFFFE0030) // (SPI) Chip Select Register
// ========== Register definition for PDC_ADC peripheral ==========
#define AT91C_ADC_PTSR  ((AT91_REG *) 	0xFFFD8124) // (PDC_ADC) PDC Transfer Status Register
#define AT91C_ADC_PTCR  ((AT91_REG *) 	0xFFFD8120) // (PDC_ADC) PDC Transfer Control Register
#define AT91C_ADC_TNPR  ((AT91_REG *) 	0xFFFD8118) // (PDC_ADC) Transmit Next Pointer Register
#define AT91C_ADC_TNCR  ((AT91_REG *) 	0xFFFD811C) // (PDC_ADC) Transmit Next Counter Register
#define AT91C_ADC_RNPR  ((AT91_REG *) 	0xFFFD8110) // (PDC_ADC) Receive Next Pointer Register
#define AT91C_ADC_RNCR  ((AT91_REG *) 	0xFFFD8114) // (PDC_ADC) Receive Next Counter Register
#define AT91C_ADC_RPR   ((AT91_REG *) 	0xFFFD8100) // (PDC_ADC) Receive Pointer Register
#define AT91C_ADC_TCR   ((AT91_REG *) 	0xFFFD810C) // (PDC_ADC) Transmit Counter Register
#define AT91C_ADC_TPR   ((AT91_REG *) 	0xFFFD8108) // (PDC_ADC) Transmit Pointer Register
#define AT91C_ADC_RCR   ((AT91_REG *) 	0xFFFD8104) // (PDC_ADC) Receive Counter Register
// ========== Register definition for ADC peripheral ==========
#define AT91C_ADC_CDR2  ((AT91_REG *) 	0xFFFD8038) // (ADC) ADC Channel Data Register 2
#define AT91C_ADC_CDR3  ((AT91_REG *) 	0xFFFD803C) // (ADC) ADC Channel Data Register 3
#define AT91C_ADC_CDR0  ((AT91_REG *) 	0xFFFD8030) // (ADC) ADC Channel Data Register 0
#define AT91C_ADC_CDR5  ((AT91_REG *) 	0xFFFD8044) // (ADC) ADC Channel Data Register 5
#define AT91C_ADC_CHDR  ((AT91_REG *) 	0xFFFD8014) // (ADC) ADC Channel Disable Register
#define AT91C_ADC_SR    ((AT91_REG *) 	0xFFFD801C) // (ADC) ADC Status Register
#define AT91C_ADC_CDR4  ((AT91_REG *) 	0xFFFD8040) // (ADC) ADC Channel Data Register 4
#define AT91C_ADC_CDR1  ((AT91_REG *) 	0xFFFD8034) // (ADC) ADC Channel Data Register 1
#define AT91C_ADC_LCDR  ((AT91_REG *) 	0xFFFD8020) // (ADC) ADC Last Converted Data Register
#define AT91C_ADC_IDR   ((AT91_REG *) 	0xFFFD8028) // (ADC) ADC Interrupt Disable Register
#define AT91C_ADC_CR    ((AT91_REG *) 	0xFFFD8000) // (ADC) ADC Control Register
#define AT91C_ADC_CDR7  ((AT91_REG *) 	0xFFFD804C) // (ADC) ADC Channel Data Register 7
#define AT91C_ADC_CDR6  ((AT91_REG *) 	0xFFFD8048) // (ADC) ADC Channel Data Register 6
#define AT91C_ADC_IER   ((AT91_REG *) 	0xFFFD8024) // (ADC) ADC Interrupt Enable Register
#define AT91C_ADC_CHER  ((AT91_REG *) 	0xFFFD8010) // (ADC) ADC Channel Enable Register
#define AT91C_ADC_CHSR  ((AT91_REG *) 	0xFFFD8018) // (ADC) ADC Channel Status Register
#define AT91C_ADC_MR    ((AT91_REG *) 	0xFFFD8004) // (ADC) ADC Mode Register
#define AT91C_ADC_IMR   ((AT91_REG *) 	0xFFFD802C) // (ADC) ADC Interrupt Mask Register
// ========== Register definition for TWI peripheral ==========
#define AT91C_TWI_IER   ((AT91_REG *) 	0xFFFB8024) // (TWI) Interrupt Enable Register
#define AT91C_TWI_CR    ((AT91_REG *) 	0xFFFB8000) // (TWI) Control Register
#define AT91C_TWI_SR    ((AT91_REG *) 	0xFFFB8020) // (TWI) Status Register
#define AT91C_TWI_IMR   ((AT91_REG *) 	0xFFFB802C) // (TWI) Interrupt Mask Register
#define AT91C_TWI_THR   ((AT91_REG *) 	0xFFFB8034) // (TWI) Transmit Holding Register
#define AT91C_TWI_IDR   ((AT91_REG *) 	0xFFFB8028) // (TWI) Interrupt Disable Register
#define AT91C_TWI_IADR  ((AT91_REG *) 	0xFFFB800C) // (TWI) Internal Address Register
#define AT91C_TWI_MMR   ((AT91_REG *) 	0xFFFB8004) // (TWI) Master Mode Register
#define AT91C_TWI_CWGR  ((AT91_REG *) 	0xFFFB8010) // (TWI) Clock Waveform Generator Register
#define AT91C_TWI_RHR   ((AT91_REG *) 	0xFFFB8030) // (TWI) Receive Holding Register
// ========== Register definition for PWMC_CH3 peripheral ==========
#define AT91C_PWMC_CH3_CUPDR ((AT91_REG *) 	0xFFFCC270) // (PWMC_CH3) Channel Update Register
#define AT91C_PWMC_CH3_Reserved ((AT91_REG *) 	0xFFFCC274) // (PWMC_CH3) Reserved
#define AT91C_PWMC_CH3_CPRDR ((AT91_REG *) 	0xFFFCC268) // (PWMC_CH3) Channel Period Register
#define AT91C_PWMC_CH3_CDTYR ((AT91_REG *) 	0xFFFCC264) // (PWMC_CH3) Channel Duty Cycle Register
#define AT91C_PWMC_CH3_CCNTR ((AT91_REG *) 	0xFFFCC26C) // (PWMC_CH3) Channel Counter Register
#define AT91C_PWMC_CH3_CMR ((AT91_REG *) 	0xFFFCC260) // (PWMC_CH3) Channel Mode Register
// ========== Register definition for PWMC_CH2 peripheral ==========
#define AT91C_PWMC_CH2_Reserved ((AT91_REG *) 	0xFFFCC254) // (PWMC_CH2) Reserved
#define AT91C_PWMC_CH2_CMR ((AT91_REG *) 	0xFFFCC240) // (PWMC_CH2) Channel Mode Register
#define AT91C_PWMC_CH2_CCNTR ((AT91_REG *) 	0xFFFCC24C) // (PWMC_CH2) Channel Counter Register
#define AT91C_PWMC_CH2_CPRDR ((AT91_REG *) 	0xFFFCC248) // (PWMC_CH2) Channel Period Register
#define AT91C_PWMC_CH2_CUPDR ((AT91_REG *) 	0xFFFCC250) // (PWMC_CH2) Channel Update Register
#define AT91C_PWMC_CH2_CDTYR ((AT91_REG *) 	0xFFFCC244) // (PWMC_CH2) Channel Duty Cycle Register
// ========== Register definition for PWMC_CH1 peripheral ==========
#define AT91C_PWMC_CH1_Reserved ((AT91_REG *) 	0xFFFCC234) // (PWMC_CH1) Reserved
#define AT91C_PWMC_CH1_CUPDR ((AT91_REG *) 	0xFFFCC230) // (PWMC_CH1) Channel Update Register
#define AT91C_PWMC_CH1_CPRDR ((AT91_REG *) 	0xFFFCC228) // (PWMC_CH1) Channel Period Register
#define AT91C_PWMC_CH1_CCNTR ((AT91_REG *) 	0xFFFCC22C) // (PWMC_CH1) Channel Counter Register
#define AT91C_PWMC_CH1_CDTYR ((AT91_REG *) 	0xFFFCC224) // (PWMC_CH1) Channel Duty Cycle Register
#define AT91C_PWMC_CH1_CMR ((AT91_REG *) 	0xFFFCC220) // (PWMC_CH1) Channel Mode Register
// ========== Register definition for PWMC_CH0 peripheral ==========
#define AT91C_PWMC_CH0_Reserved ((AT91_REG *) 	0xFFFCC214) // (PWMC_CH0) Reserved
#define AT91C_PWMC_CH0_CPRDR ((AT91_REG *) 	0xFFFCC208) // (PWMC_CH0) Channel Period Register
#define AT91C_PWMC_CH0_CDTYR ((AT91_REG *) 	0xFFFCC204) // (PWMC_CH0) Channel Duty Cycle Register
#define AT91C_PWMC_CH0_CMR ((AT91_REG *) 	0xFFFCC200) // (PWMC_CH0) Channel Mode Register
#define AT91C_PWMC_CH0_CUPDR ((AT91_REG *) 	0xFFFCC210) // (PWMC_CH0) Channel Update Register
#define AT91C_PWMC_CH0_CCNTR ((AT91_REG *) 	0xFFFCC20C) // (PWMC_CH0) Channel Counter Register
// ========== Register definition for PWMC peripheral ==========
#define AT91C_PWMC_IDR  ((AT91_REG *) 	0xFFFCC014) // (PWMC) PWMC Interrupt Disable Register
#define AT91C_PWMC_DIS  ((AT91_REG *) 	0xFFFCC008) // (PWMC) PWMC Disable Register
#define AT91C_PWMC_IER  ((AT91_REG *) 	0xFFFCC010) // (PWMC) PWMC Interrupt Enable Register
#define AT91C_PWMC_VR   ((AT91_REG *) 	0xFFFCC0FC) // (PWMC) PWMC Version Register
#define AT91C_PWMC_ISR  ((AT91_REG *) 	0xFFFCC01C) // (PWMC) PWMC Interrupt Status Register
#define AT91C_PWMC_SR   ((AT91_REG *) 	0xFFFCC00C) // (PWMC) PWMC Status Register
#define AT91C_PWMC_IMR  ((AT91_REG *) 	0xFFFCC018) // (PWMC) PWMC Interrupt Mask Register
#define AT91C_PWMC_MR   ((AT91_REG *) 	0xFFFCC000) // (PWMC) PWMC Mode Register
#define AT91C_PWMC_ENA  ((AT91_REG *) 	0xFFFCC004) // (PWMC) PWMC Enable Register
// ========== Register definition for UDP peripheral ==========
#define AT91C_UDP_IMR   ((AT91_REG *) 	0xFFFB0018) // (UDP) Interrupt Mask Register
#define AT91C_UDP_FADDR ((AT91_REG *) 	0xFFFB0008) // (UDP) Function Address Register
#define AT91C_UDP_NUM   ((AT91_REG *) 	0xFFFB0000) // (UDP) Frame Number Register
#define AT91C_UDP_FDR   ((AT91_REG *) 	0xFFFB0050) // (UDP) Endpoint FIFO Data Register
#define AT91C_UDP_ISR   ((AT91_REG *) 	0xFFFB001C) // (UDP) Interrupt Status Register
#define AT91C_UDP_CSR   ((AT91_REG *) 	0xFFFB0030) // (UDP) Endpoint Control and Status Register
#define AT91C_UDP_IDR   ((AT91_REG *) 	0xFFFB0014) // (UDP) Interrupt Disable Register
#define AT91C_UDP_ICR   ((AT91_REG *) 	0xFFFB0020) // (UDP) Interrupt Clear Register
#define AT91C_UDP_RSTEP ((AT91_REG *) 	0xFFFB0028) // (UDP) Reset Endpoint Register
#define AT91C_UDP_TXVC  ((AT91_REG *) 	0xFFFB0074) // (UDP) Transceiver Control Register
#define AT91C_UDP_GLBSTATE ((AT91_REG *) 	0xFFFB0004) // (UDP) Global State Register
#define AT91C_UDP_IER   ((AT91_REG *) 	0xFFFB0010) // (UDP) Interrupt Enable Register

// *****************************************************************************
//               PERIPHERAL ID DEFINITIONS FOR AT91SAM7S256
// *****************************************************************************
#define AT91C_ID_FIQ    ((unsigned int)  0) // Advanced Interrupt Controller (FIQ)
#define AT91C_ID_SYS    ((unsigned int)  1) // System Peripheral
#define AT91C_ID_PIOA   ((unsigned int)  2) // Parallel IO Controller
#define AT91C_ID_3_Reserved ((unsigned int)  3) // Reserved
#define AT91C_ID_ADC    ((unsigned int)  4) // Analog-to-Digital Converter
#define AT91C_ID_SPI    ((unsigned int)  5) // Serial Peripheral Interface
#define AT91C_ID_US0    ((unsigned int)  6) // USART 0
#define AT91C_ID_US1    ((unsigned int)  7) // USART 1
#define AT91C_ID_SSC    ((unsigned int)  8) // Serial Synchronous Controller
#define AT91C_ID_TWI    ((unsigned int)  9) // Two-Wire Interface
#define AT91C_ID_PWMC   ((unsigned int) 10) // PWM Controller
#define AT91C_ID_UDP    ((unsigned int) 11) // USB Device Port
#define AT91C_ID_TC0    ((unsigned int) 12) // Timer Counter 0
#define AT91C_ID_TC1    ((unsigned int) 13) // Timer Counter 1
#define AT91C_ID_TC2    ((unsigned int) 14) // Timer Counter 2
#define AT91C_ID_15_Reserved ((unsigned int) 15) // Reserved
#define AT91C_ID_16_Reserved ((unsigned int) 16) // Reserved
#define AT91C_ID_17_Reserved ((unsigned int) 17) // Reserved
#define AT91C_ID_18_Reserved ((unsigned int) 18) // Reserved
#define AT91C_ID_19_Reserved ((unsigned int) 19) // Reserved
#define AT91C_ID_20_Reserved ((unsigned int) 20) // Reserved
#define AT91C_ID_21_Reserved ((unsigned int) 21) // Reserved
#define AT91C_ID_22_Reserved ((unsigned int) 22) // Reserved
#define AT91C_ID_23_Reserved ((unsigned int) 23) // Reserved
#define AT91C_ID_24_Reserved ((unsigned int) 24) // Reserved
#define AT91C_ID_25_Reserved ((unsigned int) 25) // Reserved
#define AT91C_ID_26_Reserved ((unsigned int) 26) // Reserved
#define AT91C_ID_27_Reserved ((unsigned int) 27) // Reserved
#define AT91C_ID_28_Reserved ((unsigned int) 28) // Reserved
#define AT91C_ID_29_Reserved ((unsigned int) 29) // Reserved
#define AT91C_ID_IRQ0   ((unsigned int) 30) // Advanced Interrupt Controller (IRQ0)
#define AT91C_ID_IRQ1   ((unsigned int) 31) // Advanced Interrupt Controller (IRQ1)

// *****************************************************************************
//               BASE ADDRESS DEFINITIONS FOR AT91SAM7S256
// *****************************************************************************
#define AT91C_BASE_SYS       ((AT91PS_SYS) 	0xFFFFF000) // (SYS) Base Address
#define AT91C_BASE_AIC       ((AT91PS_AIC) 	0xFFFFF000) // (AIC) Base Address
#define AT91C_BASE_PDC_DBGU  ((AT91PS_PDC) 	0xFFFFF300) // (PDC_DBGU) Base Address
#define AT91C_BASE_DBGU      ((AT91PS_DBGU) 	0xFFFFF200) // (DBGU) Base Address
#define AT91C_BASE_PIOA      ((AT91PS_PIO) 	0xFFFFF400) // (PIOA) Base Address
#define AT91C_BASE_CKGR      ((AT91PS_CKGR) 	0xFFFFFC20) // (CKGR) Base Address
#define AT91C_BASE_PMC       ((AT91PS_PMC) 	0xFFFFFC00) // (PMC) Base Address
#define AT91C_BASE_RSTC      ((AT91PS_RSTC) 	0xFFFFFD00) // (RSTC) Base Address
#define AT91C_BASE_RTTC      ((AT91PS_RTTC) 	0xFFFFFD20) // (RTTC) Base Address
#define AT91C_BASE_PITC      ((AT91PS_PITC) 	0xFFFFFD30) // (PITC) Base Address
#define AT91C_BASE_WDTC      ((AT91PS_WDTC) 	0xFFFFFD40) // (WDTC) Base Address
#define AT91C_BASE_VREG      ((AT91PS_VREG) 	0xFFFFFD60) // (VREG) Base Address
#define AT91C_BASE_MC        ((AT91PS_MC) 	0xFFFFFF00) // (MC) Base Address
#define AT91C_BASE_PDC_SPI   ((AT91PS_PDC) 	0xFFFE0100) // (PDC_SPI) Base Address
#define AT91C_BASE_SPI       ((AT91PS_SPI) 	0xFFFE0000) // (SPI) Base Address
#define AT91C_BASE_PDC_ADC   ((AT91PS_PDC) 	0xFFFD8100) // (PDC_ADC) Base Address
#define AT91C_BASE_ADC       ((AT91PS_ADC) 	0xFFFD8000) // (ADC) Base Address
#define AT91C_BASE_PDC_SSC   ((AT91PS_PDC) 	0xFFFD4100) // (PDC_SSC) Base Address
#define AT91C_BASE_SSC       ((AT91PS_SSC) 	0xFFFD4000) // (SSC) Base Address
#define AT91C_BASE_PDC_US1   ((AT91PS_PDC) 	0xFFFC4100) // (PDC_US1) Base Address
#define AT91C_BASE_US1       ((AT91PS_USART) 	0xFFFC4000) // (US1) Base Address
#define AT91C_BASE_PDC_US0   ((AT91PS_PDC) 	0xFFFC0100) // (PDC_US0) Base Address
#define AT91C_BASE_US0       ((AT91PS_USART) 	0xFFFC0000) // (US0) Base Address
#define AT91C_BASE_TWI       ((AT91PS_TWI) 	0xFFFB8000) // (TWI) Base Address
#define AT91C_BASE_TC0       ((AT91PS_TC) 	0xFFFA0000) // (TC0) Base Address
#define AT91C_BASE_TC1       ((AT91PS_TC) 	0xFFFA0040) // (TC1) Base Address
#define AT91C_BASE_TC2       ((AT91PS_TC) 	0xFFFA0080) // (TC2) Base Address
#define AT91C_BASE_TCB       ((AT91PS_TCB) 	0xFFFA0000) // (TCB) Base Address
#define AT91C_BASE_PWMC_CH3  ((AT91PS_PWMC_CH) 	0xFFFCC260) // (PWMC_CH3) Base Address
#define AT91C_BASE_PWMC_CH2  ((AT91PS_PWMC_CH) 	0xFFFCC240) // (PWMC_CH2) Base Address
#define AT91C_BASE_PWMC_CH1  ((AT91PS_PWMC_CH) 	0xFFFCC220) // (PWMC_CH1) Base Address
#define AT91C_BASE_PWMC_CH0  ((AT91PS_PWMC_CH) 	0xFFFCC200) // (PWMC_CH0) Base Address
#define AT91C_BASE_PWMC      ((AT91PS_PWMC) 	0xFFFCC000) // (PWMC) Base Address
#define AT91C_BASE_UDP       ((AT91PS_UDP) 	0xFFFB0000) // (UDP) Base Address

// *****************************************************************************
//               MEMORY MAPPING DEFINITIONS FOR AT91SAM7S256
// *****************************************************************************
#define AT91C_ISRAM	 ((char *) 	0x00200000) // Internal SRAM base address
#define AT91C_ISRAM_SIZE	 ((unsigned int) 0x00010000) // Internal SRAM size in byte (64 Kbyte)
#define AT91C_IFLASH	 ((char *) 	0x00100000) // Internal ROM base address
#define AT91C_IFLASH_SIZE	 ((unsigned int) 0x00040000) // Internal ROM size in byte (256 Kbyte)

#endif

#endif /* AT91SAM7S256_H_ */
