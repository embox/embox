#ifndef _LEON_H_
#define _LEON_H_
#if 0
#ifndef __ASSEMBLER__

#ifdef LEON2
typedef struct _LEON_REGS
{
	volatile unsigned int memcfg1;		/* 0x00 */
	volatile unsigned int memcfg2;
	volatile unsigned int ectrl;
	volatile unsigned int failaddr;
	volatile unsigned int memstatus;		/* 0x10 */
	volatile unsigned int cachectrl;
	volatile unsigned int powerdown;
	volatile unsigned int writeprot1;
	volatile unsigned int writeprot2;	/* 0x20 */
	volatile unsigned int leonconf;
	volatile unsigned int dummy2;
	volatile unsigned int dummy3;
	volatile unsigned int dummy4;		// 0x30 */
	volatile unsigned int dummy5;
	volatile unsigned int dummy6;
	volatile unsigned int dummy7;
	volatile unsigned int _timercnt1;		// 0x40
	volatile unsigned int _timerload1;
	volatile unsigned int _timerctrl1;
	volatile unsigned int _wdog;
	volatile unsigned int _timercnt2;		//0x50
	volatile unsigned int _timerload2;
	volatile unsigned int _timerctrl2;
	volatile unsigned int _dummy8;
	volatile unsigned int _scalercnt;		//0x60
	volatile unsigned int _scalerload;
	volatile unsigned int dummy9;
	volatile unsigned int dummy10;
	volatile unsigned int _uartdata1;		//0x70
	volatile unsigned int _uartstatus1;
	volatile unsigned int _uartctrl1;
	volatile unsigned int _uartscaler1;
	volatile unsigned int _uartdata2;
	volatile unsigned int _uartstatus2;
	volatile unsigned int _uartctrl2;
	volatile unsigned int _uartscaler2;
	volatile unsigned int _irqmask;
	volatile unsigned int _irqpend;
	volatile unsigned int _irqforce;
	volatile unsigned int _irqclear;
	volatile unsigned int piodata;
	volatile unsigned int piodir;
	volatile unsigned int pioirq;
	volatile unsigned int dummy11;
	volatile unsigned int imask2;
	volatile unsigned int ipend2;
	volatile unsigned int istat2;
	volatile unsigned int dummy12;
	volatile unsigned int dcomdata;
	volatile unsigned int dcomstatus;
	volatile unsigned int dcomctrl;
	volatile unsigned int dcomscaler;
}LEON_REGS;
#endif

#ifdef LEON3
typedef struct _LEON_REGS
{
	volatile unsigned int memcfg1;		/* 0x00 */
	volatile unsigned int memcfg2;
	volatile unsigned int ectrl;
	volatile unsigned int failaddr;
	volatile unsigned int memstatus;		/* 0x10 */
	volatile unsigned int cachectrl;
	volatile unsigned int powerdown;
	volatile unsigned int writeprot1;
	volatile unsigned int writeprot2;	/* 0x20 */
	volatile unsigned int leonconf;
	volatile unsigned int dummy2;
	volatile unsigned int dummy3;
	volatile unsigned int dummy4;		// 0x30 */
	volatile unsigned int dummy5;
	volatile unsigned int dummy6;
	volatile unsigned int dummy7;
	volatile unsigned int timercnt1;		// 0x40
	volatile unsigned int timerload1;
	volatile unsigned int timerctrl1;
	volatile unsigned int wdog;
	volatile unsigned int timercnt2;		//0x50
	volatile unsigned int timerload2;
	volatile unsigned int timerctrl2;
	volatile unsigned int dummy8;
	volatile unsigned int scalercnt;		//0x60
	volatile unsigned int scalerload;
	volatile unsigned int dummy9;
	volatile unsigned int dummy10;
	volatile unsigned int uartdata1;		//0x70
	volatile unsigned int uartstatus1;
	volatile unsigned int uartctrl1;
	volatile unsigned int uartscaler1;
	volatile unsigned int uartdata2;
	volatile unsigned int uartstatus2;
	volatile unsigned int uartctrl2;
	volatile unsigned int uartscaler2;
	volatile unsigned int irqmask;
	volatile unsigned int irqpend;
	volatile unsigned int irqforce;
	volatile unsigned int irqclear;
	volatile unsigned int piodata;
	volatile unsigned int piodir;
	volatile unsigned int pioirq;
	volatile unsigned int dummy11;
	volatile unsigned int imask2;
	volatile unsigned int ipend2;
	volatile unsigned int istat2;
	volatile unsigned int dummy12;
	volatile unsigned int dcomdata;
	volatile unsigned int dcomstatus;
	volatile unsigned int dcomctrl;
	volatile unsigned int dcomscaler;
}LEON_REGS;
#endif

extern LEON_REGS *const l_regs;

#endif // ifndef __ASSEMBLER__
#endif
#endif // _LEON_H_
