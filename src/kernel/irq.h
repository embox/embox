/**
 * \file irq.h
 *
 * \brief Common interrupting-related handling routines
 *
 * \author Eldar Abusalimov
 * \author Anton Bondarev
 * \author Alexey Fomin
 * \author Alexandr Batyukov
 */

#ifndef _IRQ_H_
#define _IRQ_H_
#include "types.h"

// Not for assembler
#ifndef __ASSEMBLER__

// type of irq handler function
typedef void (*IRQ_HANDLER)();

// traps handlers table size
#define IRQ_TABLE_SIZE     0xFF

void irq_init_handlers();

// TODO m.b. move these ones to arch or delete them at all?
// set trap handler
BOOL irq_set_trap_handler(BYTE tt, IRQ_HANDLER pfunc);
// remove trap handler
BOOL irq_remove_trap_handler(BYTE tt);

/**
 * Sets and enables a new callback for the specified IRQ number
 * (removing and disabling an old one if any).
 *
 * @param nirq IRQ number to set handler for
 * @param pfunc the new handler function to set
 *  	or NULL to disable interrupts for the specified IRQ number
 */
void irq_set_handler(BYTE nirq, IRQ_HANDLER pfunc);

/**
 * Gets a handler associated with the specified IRQ number (if any).
 *
 * @param nirq IRQ number to check
 * @return the handler function if it has been set or NULL otherwise
 */
IRQ_HANDLER irq_get_handler(BYTE nirq);

#endif // __ASSEMBLER__
#endif  // _IRQ_H_
