#ifndef LEON_134213_4535_53453
#define LEON_134213_4535_53453

#include <asm/cpu_conf.h>

#define UART_SCALER_VAL  ((((CORE_FREQ*10) / (8 * UART_BAUD_RATE))-5)/10)
/* Timing Calculation */
#define TIMER_SCALER_VAL (CORE_FREQ/1000000 - 1)

/*LEON control registers */
#define	PREGS	0x80000000
#define	MCFG1	0x00
#define	MCFG2	0x04
#define	MCFG3	0x08
#define	ECTRL	0x08
#define	FADDR	0x0c
#define	MSTAT	0x10
#define CCTRL	0x14
#define PWDOWN	0x18
#define WPROT1	0x1C
#define WPROT2	0x20
#define LCONF 	0x24
#define	TCNT0	0x40
#define	TRLD0	0x44
#define	TCTRL0	0x48
#define	TCNT1	0x50
#define	TRLD1	0x54
#define	TCTRL1	0x58
#define	SCNT  	0x60
#define	SRLD  	0x64
#define	UDATA0 	0x70
#define	USTAT0 	0x74
#define	UCTRL0 	0x78
#define	USCAL0 	0x7c
#define	UDATA1 	0x80
#define	USTAT1 	0x84
#define	UCTRL1 	0x88
#define	LVDS_CTRL 	0x88
#define	USCAL1 	0x8c
#define	IMASK	0x90
#define	IPEND	0x94
#define	IFORCE	0x98
#define	ICLEAR	0x9c
#define	IOREG	0xA0
#define	IODIR	0xA4
#define	IOICONF	0xA8
#define	IMASK2	0xB0
#define	IPEND2	0xB4
#define	ISTAT2  0xB8
#define	ICLEAR2	0xB8

#endif
