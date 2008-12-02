#ifndef _IRQ_H_
#define _IRQ_H_
#include "types.h"
#ifndef __ASSEMBLER__
#ifdef LEON2
typedef struct _IRQ_REGS
{
	volatile unsigned short level;
	volatile unsigned short mask;
	volatile unsigned int pend;
	volatile unsigned int force;
	volatile unsigned int clear;
}IRQ_REGS;
#endif

#ifdef LEON3
typedef struct _IRQ_REGS
{
    volatile unsigned int level;		/* 0x00 */
    volatile unsigned int pend;		/* 0x04 */
    volatile unsigned int force;		/* 0x08 */
    volatile unsigned int clear;		/* 0x0C */
    volatile unsigned int mpstatus;		/* 0x10 */
    volatile unsigned int dummy[11];		/* 0x14 - 0x3C */
    volatile unsigned int mask;		/* 0x40 */
}IRQ_REGS;
#endif
extern IRQ_REGS *const irq_regs;

#endif//assembler

/* Main Interrupt Controller */
#define IRQ_Halt		0xF		//
#define IRQ_PCI 		0xE		// (PCI)
#define IRQ_Hang		0xD		// Зависание при обращении в область Memory-Bus I/O
#define IRQ_EthMAC		0xC		// Прерывание от LXT971 (Eth Phy)
#define IRQ_DSU			0xB		// DSU trace buffer

#define IRQ_Timer2		0x9		// Timer 2
#define IRQ_Timer1		0x8		// Timer 1


#define IRQ_UART1		0x3		//


#define IRQ_AHB_error	0x1		//


#define IRQ_TRAP_TYPE(nirq)   (0x10 + (nirq))
#define IRQ_NO_TRAP           0x00

// Interrupt buffer size
#define IRQ_BUFFER_SIZE   64

// Not for assembler
#ifndef __ASSEMBLER__

// Interrupt handlers table size
#define IRQ_HTBL_SIZE     0x20

// type of irq handler function
typedef void (*IRQ_HANDLER)();

// description of irq handler
typedef struct _IRQ_HANDLER_DESCR
{
	BYTE tt;              // trap type
	IRQ_HANDLER pfunc;    // irq handler
} IRQ_HANDLER_DESCR;


extern int irq_current;
extern IRQ_HANDLER_DESCR irq_handlers_table[IRQ_HTBL_SIZE];


/***
 * Functions
 ***/

// set handler & enable interrupt
BOOL irq_set_handler(BYTE nirq, IRQ_HANDLER pfunc);
// set trap handler
BOOL irq_set_trap_handler(BYTE tt, IRQ_HANDLER pfunc);
// remove interrupt handler
void irq_remove_handler(BYTE nirq);


void halt_trap();
#endif // ifndef __ASSEMBLER__

#endif  // ifndef _IRQ_H_
