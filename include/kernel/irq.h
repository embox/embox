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

typedef struct {
	int irq_num;
	IRQ_HANDLER handler;
	bool enabled;
} IRQ_INFO;

// traps handlers table size
#define IRQ_TABLE_SIZE     0xFF

int irq_init_handlers();



/**
 * Sets and enables a new callback for the specified IRQ number
 * (removing and disabling an old one if any).
 *
 * @param IRQ_INFO new IRQ info to be set
 * 		changes it's value to old value of specified irq info
 * @return true if completed successively
 */
bool irq_set_info(IRQ_INFO *irq_info);

void irq_set_handler(uint8_t irq_number, IRQ_HANDLER pfunc);

/**
 * Gets a handler associated with the specified IRQ number (if any).
 *
 * @param nirq IRQ number to check
 * @return the handler function if it has been set or NULL otherwise
 */
IRQ_HANDLER irq_get_handler(uint8_t nirq);

#endif // __ASSEMBLER__
#endif  // _IRQ_H_
