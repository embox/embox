/**
 * @file
 *
 * @brief
 *
 * @date 17.10.2012
 * @author Alexander Kalmuk
 */

#include <kernel/task/io_sync.h>
#include <kernel/thread/sched_lock.h>
#include <kernel/thread/event.h>
#include <kernel/irq_lock.h>

void io_op_unblock(struct idx_io_op_state *op) {
	irq_lock();
	{
		op->can_perform_op = 1;
		if (op->unblock) {
			event_notify(op->unblock);
		}
	}
	irq_unlock();
}

void io_op_set_event(struct idx_io_op_state *op, struct event *e) {
	irq_lock();
	op->unblock = e;
	irq_unlock();
}
