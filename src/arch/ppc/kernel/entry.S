/**
 * @file
 * @brief Low-level interrupt handler
 *
 * @date 12.11.12
 * @author Ilia Vaprol
 */

#include <asm/regs.h>

.globl fi_tmr_hnd
fi_tmr_hnd:
#if 0
	mftsr r3   /* clear TSR[FIS] */
	lis   r4, TSR_FIS@h
	ori   r4, r4, TSR_FIS@l
	andc  r3, r3, r4
	mttsr r3
#endif
	rfi        /* return from interrupt */
