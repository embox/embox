/**
 * @file
 *
 * @brief
 *
 * @date 17.10.2012
 * @author Alexander Kalmuk
 */

#include <io_sync.h>
#include <kernel/thread/sched_lock.h>
#include <kernel/thread/event.h>

void io_op_unblock(struct idx_io_op_state *op) {
	ipl_t ipl = ipl_save();

	op->op_is_nonblocking = 1;
	if (op->unblock) {
		event_notify(op->unblock);
	}

	ipl_restore(ipl);
}

void io_op_set_event(struct idx_io_op_state *op, struct event *e) {
	sched_lock();
	op->unblock = e;
	sched_unlock();
}
