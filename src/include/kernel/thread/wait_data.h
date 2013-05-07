/**
 * @file
 * @brief
 *
 * @date 06.05.2013
 * @author Anton Bulychev
 */

#ifndef KERNEL_THREAD_WAIT_DATA_H_
#define KERNEL_THREAD_WAIT_DATA_H_

#include <errno.h>
#include <assert.h>

#include <hal/ipl.h>

#include <kernel/work.h>

#define WAIT_DATA_STATUS_NONE        0
#define WAIT_DATA_STATUS_WAITING     1
#define WAIT_DATA_STATUS_NOTIFIED    2

typedef void (*notify_handler)(struct thread *, void *data);

struct wait_data {
	struct work work;

	int status;
	int result;

	notify_handler on_notified;
	void *data;
};

static inline void wait_data_init(struct wait_data *wait_data) {
	wait_data->status = WAIT_DATA_STATUS_NONE;
}

static inline void wait_data_prepare(struct wait_data *wait_data, work_handler handler) {
	ipl_t ipl;

	assert(wait_data->status == WAIT_DATA_STATUS_NONE);

	ipl = ipl_save();
	{
		work_init(&wait_data->work, handler, 0);
		wait_data->status = WAIT_DATA_STATUS_WAITING;
		wait_data->result = ENOERR;
	}
	ipl_restore(ipl);
}

static inline void wait_data_cleanup(struct wait_data *wait_data) {
	assert(wait_data->status != WAIT_DATA_STATUS_NONE);

	wait_data->status = WAIT_DATA_STATUS_NONE;
	work_disable(&wait_data->work);
}

#endif /* KERNEL_THREAD_WAIT_DATA_H_ */
