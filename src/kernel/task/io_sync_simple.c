/**
 * @file
 *
 * @brief
 *
 * @date 17.10.2012
 * @author Alexander Kalmuk
 */

#include <kernel/task/io_sync.h>

void io_op_unblock(struct idx_io_op_state *op) {
	softirq_lock();
	op->can_perform_op = 1;
	softirq_unlock();
}

void io_op_set_event(struct idx_io_op_state *op, struct event *e) {
	return; /* do nothing */
}
