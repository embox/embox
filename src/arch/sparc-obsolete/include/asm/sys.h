/**
 * @file
 * @brief System calls and other system function
 *
 * @date 19.11.2009
 * @author Anton Bondarev
 */

#ifndef SPARC_SYS_H_
#define SPARC_SYS_H_


/*function for kernel spin_locks*/
/**
 * Disable irq and return irq status
 * this function set PIL in value 15 it means that all irq disable except IRQ level 15( non mask irq)
 */


/**Enable irq set PIL in level 0*/
void local_irq_enable(void);

/**
 * Restore irq status after local_irq_save function
 */
void local_irq_restore(unsigned long old_psr);

#endif /* SPARC_SYS_H_ */
