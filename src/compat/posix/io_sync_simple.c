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
	ipl_t ipl = ipl_save();
	op->op_is_nonblocking = 1;
	ipl_restore(ipl);
}

void io_op_set_event(struct idx_io_op_state *op, struct event *e) {
	return; /* do nothing */
}

