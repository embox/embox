/**
 * @file
 * @brief
 *
 * @date 17.10.12
 * @author Alexander Kalmuk
 */

#include <kernel/task/io_sync.h>
#include <kernel/event.h>

void idx_io_enable(struct idx_desc_data *ddata, int op) {
	struct idx_io_state *io_state;

	assert(ddata);

	io_state = &ddata->io_state;

	softirq_lock();
	{
		io_state->io_ready |= op;
		if (io_state->io_enable && (io_state->io_monitoring & op)) {
			event_notify(io_state->io_enable);
		}
	}
	softirq_unlock();
}

void idx_io_set_event(struct idx_desc_data *ddata, struct event *event) {
	softirq_lock();
	ddata->io_state.io_enable = event;
	softirq_unlock();
}
