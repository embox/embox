/**
 * @file
 *
 * @brief
 *
 * @date 17.10.2012
 * @author Alexander Kalmuk
 */

#ifndef KERNEL_TASK_IO_SYNC_H_
#define KERNEL_TASK_IO_SYNC_H_

#include <kernel/task.h>
#include <kernel/task/idx.h>
#include <kernel/softirq_lock.h>
#include <assert.h>

struct event;
struct idx_desc;

/* I/O operations */
#define IDX_IO_READING 0x01
#define IDX_IO_WRITING 0x10

static inline void idx_io_set_monitor(struct idx_desc_data *ddata, int op) {
	assert(ddata);
	ddata->io_state.io_monitoring |= op;
}

static inline void idx_io_unset_monitor(struct idx_desc_data *ddata, int op) {
	assert(ddata);
	ddata->io_state.io_monitoring &= ~op;
}

static inline void idx_io_disable(struct idx_desc_data *ddata, int op) {
	softirq_lock();
	assert(ddata);
	ddata->io_state.io_ready &= ~op;
	softirq_unlock();
}

extern void idx_io_enable(struct idx_desc_data *ddata, int op);

extern void idx_io_set_event(struct idx_desc_data *ddata, struct event *event);

#endif /* KERNEL_TASK_IO_SYNC_H_ */
