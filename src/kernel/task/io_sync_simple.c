/**
 * @file
 * @brief
 *
 * @date 17.10.12
 * @author Alexander Kalmuk
 */

#include <kernel/task/io_sync.h>

void idx_io_enable(struct idx_desc_data *ddata, int op) {
	softirq_lock();
	assert(ddata);
	ddata->io_state.io_ready |= op;
	softirq_unlock();
}

void idx_io_set_event(struct idx_desc_data *ddata, struct event *event) {
	return; /* do nothing */
}
