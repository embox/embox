/**
 * @file
 *
 * @brief
 *
 * @date 17.10.2012
 * @author Alexander Kalmuk
 */

#ifndef IO_SYNC_H_
#define IO_SYNC_H_

#include <kernel/task.h>
#include <kernel/task/idx.h>
//#include <kernel/irq_lock.h>

struct event;

extern void io_op_unblock(struct idx_io_op_state *op);
extern void io_op_set_event(struct idx_io_op_state *op, struct event *e);

static inline void io_op_block(struct idx_io_op_state *op) {
	//irq_lock();
	op->can_perform_op = 0; /* it must be test_and_set */
	//irq_unlock();
}

static inline int io_op_is_block(struct idx_io_op_state *op) {
	return op->can_perform_op;
}

#endif /* IO_SYNC_H_ */
