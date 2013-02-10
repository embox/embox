/**
 * @file
 *
 * @brief
 *
 * @date 17.10.2012
 * @author Alexander Kalmuk
 */

#include <kernel/task/io_sync.h>
#include <kernel/thread/event.h>

void io_op_unblock(struct idx_io_op_state *op) {
	softirq_lock();
	{
		op->can_perform_op = 1;
		if (op->unblock) {
			event_notify(op->unblock);
		}
	}
	softirq_unlock();
}

void io_op_set_event(struct idx_io_op_state *op, struct event *e) {
	softirq_lock();
	op->unblock = e;
	softirq_unlock();
}
