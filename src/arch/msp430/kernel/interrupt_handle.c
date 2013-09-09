/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    30.08.2013
 */

#include <assert.h>
#include <kernel/irq.h>

void interrupt_handle(unsigned int source) {

	assert(!critical_inside(CRITICAL_IRQ_LOCK));

	critical_enter(CRITICAL_IRQ_HANDLER);

	irq_dispatch(source);

	critical_leave(CRITICAL_IRQ_HANDLER);
	critical_dispatch_pending();

}
