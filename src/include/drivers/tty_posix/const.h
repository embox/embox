/*
 * const.h
 *
 *  Created on: 20.08.2011
 *      Author: Gerald
 */

#ifndef CONST_H_
#define CONST_H_

#define EXTERN        extern	/* used in *.h files */
#define PRIVATE       static	/* PRIVATE x limits the scope of x */
#define PUBLIC					/* PUBLIC is the opposite of PRIVATE */
#define FORWARD       static	/* some compilers require this to be 'static'*/
#define PORT_B          0x61	/* I/O port for 8255 port B (kbd, beeper...) */
#define LINEWRAP           1	/* console.c - wrap lines at column 80 */
#define	TRUE	true
#define FALSE	false
#define OK 1
/* Type definitions. */
typedef unsigned int vir_clicks; /* virtual  addresses and lengths in clicks */
typedef unsigned long phys_bytes;/* physical addresses and lengths in bytes */
typedef unsigned int phys_clicks;/* physical addresses and lengths in clicks */
typedef unsigned long vir_bytes;	/* virtual addresses and lengths in bytes */

/* Keep everything for ANSI prototypes. */
#define	_PROTOTYPE(function, params)	function params

EXTERN clock_t tty_timeout;	/* time to wake up the TTY task */

/* Disable/Enable hardware interrupts. */
#define lock		irq_lock
#define unlock		irq_unlock

#endif /* CONST_H_ */
