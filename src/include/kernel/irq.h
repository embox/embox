/**
 * @file irq.h
 *
 * @brief Common interrupting-related handling routines
 *
 * @author Eldar Abusalimov
 * @author Anton Bondarev
 * @author Alexey Fomin
 * @author Alexandr Batyukov
 */

#ifndef _IRQ_H_
#define _IRQ_H_
#include <autoconf.h>
#include <types.h>

#ifndef __ASSEMBLER__

typedef struct pt_regs {
	uint32_t pc;
} pt_regs_t;

typedef void (*IRQ_HANDLER)(int irq_num, void *dev_id, struct pt_regs *regs);

typedef struct {
	int irq_num;
	IRQ_HANDLER handler;
	void *dev_id;
	const char* dev_name;
	unsigned long flags;
	bool enabled;
} IRQ_INFO;

#define IRQ_TABLE_SIZE     0xFF

/*int irq_init_handlers(void);*/

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

#if CONFIG(SYSTEM_IRQ)
/*TODO this description from linux driver book*/
/**
 * The value returned from request_irq to the requesting function is either 0
 * to indicate success or a negative error code, as usual.
 * It's not uncommon for the function to return -EBUSY to signal
 * that another driver is already using the requested interrupt line.
 * The arguments to the functions are as follows:
 * @param irq This is the interrupt number being requested.
 * @param handler The pointer to the handling function being installed.
 *  We'll discuss the arguments to this function later in this chapter.
 * @param flags As you might expect, a bit mask of options (described later)
 *  related to interrupt management.
 * @param dev_name The string passed to request_irq is used in /proc/interrupts
 *  to show the owner of the interrupt (see the next section).
 * @param dev_id This pointer is used for shared interrupt lines.
 *  It is a unique identifier that is used when the interrupt line is freed
 *  and that may also be used by the driver to point to its own private data
 *  area (to identify which device is interrupting). When no sharing is in force,
 *  dev_id can be set to NULL, but it a good idea anyway to use this item to point
 *  to the device structure.
 *
 *
 */
int request_irq(unsigned int irq, IRQ_HANDLER handler, unsigned long flags,
		const char *dev_name, void *dev_id);
#else
#define request_irq(irq, handler, flags, dev_name, dev_id)
#endif

void free_irq(unsigned int irq, void *dev_id);

extern void irq_dispatch(uint32_t irq_number);

extern unsigned long local_irq_save(void);
extern void local_irq_restore(unsigned long old_psr);
extern void local_irq_enable(void);

/** When set, this indicates a "fast'' interrupt handler.
 * Fast handlers are executed with interrupts disabled
 */
#define SA_INTERRUPT 0x1

/** This bit signals that the interrupt can be shared between devices.
 *  The concept of sharing is outlined in "Interrupt Sharing", later in this chapter.
 */
#define SA_SHIRQ 0x2

/** flag for truly random devices*/
#define SA_SAMPLE_RANDOM 0x4

#endif /* __ASSEMBLER__ */
#endif /* _IRQ_H_ */
