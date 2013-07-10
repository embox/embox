/**
 * @file
 * @brief
 *
 * @date 17.10.12
 * @author Alexander Kalmuk
 * @author Ilia Vaprol
 */

#ifndef KERNEL_TASK_IO_SYNC_H_
#define KERNEL_TASK_IO_SYNC_H_

#include <kernel/event.h>
#include <kernel/manual_event.h>

struct io_sync {
	struct manual_event reading;
	struct manual_event writing;
    struct event *on_reading;
    struct event *on_writing;
};

enum io_sync_op {
	IO_SYNC_READING = 1,
	IO_SYNC_WRITING = 2
};

extern void io_sync_init(struct io_sync *ios, int r_set,
		int w_set);
extern void io_sync_enable(struct io_sync *ios,
        enum io_sync_op op);
extern void io_sync_disable(struct io_sync *ios,
        enum io_sync_op op);
extern int io_sync_ready(struct io_sync *ios,
		enum io_sync_op op);
extern void io_sync_notify(struct io_sync *ios,
		enum io_sync_op op, struct event *on_op);
extern int io_sync_wait(struct io_sync *ios, enum io_sync_op op,
		unsigned long timeout);

#endif /* KERNEL_TASK_IO_SYNC_H_ */
