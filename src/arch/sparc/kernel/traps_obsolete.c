/**
 * @arch/sparc/sparc_traps.c
 * @brief function for working with traps in SPARC architecture
 *
 * @date 24.11.2009
 * @author Anton Bondarev
 */

#include <types.h>

//TODO this defines must be in config header
#define MAX_IRQ_NUMBER 0x10

#define IRQ_TRAP_NUMBER 0x10
#define IRQ_TRAP_TYPE(nirq)   (IRQ_TRAP_NUMBER + (nirq))
#define IRQ_GET_NUMBER(nirq)   ((nirq) - IRQ_TRAP_NUMBER)
#define CHECK_IRQ_TRAP(tt)      ((tt > IRQ_TRAP_NUMBER) && (tt < (IRQ_TRAP_NUMBER + MAX_IRQ_NUMBER)))

/**
 * Runs user defined handler (if one has been enabled).
 * @param tt - trap type (number of current trap) it's took from "tbr" register
 * @param sp - stack pointer (TODO why is stack pointer place here)
 */
void dispatch_trap(uint8_t tt, uint32_t *sp) {
	if (CHECK_IRQ_TRAP(tt)){
		irq_dispatch(IRQ_GET_NUMBER(tt));
		return ;
	}
}
