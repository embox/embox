/**
 * @file
 *
 * @brief
 *
 * @date 17.10.2012
 * @author Alexander Kalmuk
 */

#include <io_sync.h>

void io_op_unblock(struct idx_io_op_state *op) {
	/*irq_lock();*/
	op->can_perform_op = 1;
	/*irq_unlock();*/
}

void io_op_set_event(struct idx_io_op_state *op, struct event *e) {
	return; /* do nothing */
}

