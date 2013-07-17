/**
 * @file
 * @brief
 *
 * @date 17.10.12
 * @author Alexander Kalmuk
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <kernel/event.h>
#include <kernel/manual_event.h>
#include <kernel/task/io_sync.h>
#include <stddef.h>

void io_sync_init(struct io_sync *ios, int r_set, int w_set) {
	assert(ios != NULL);

	manual_event_init(&ios->can_read, r_set);
	manual_event_init(&ios->can_write, w_set);
	ios->on_reading = NULL;
	ios->on_writing = NULL;
}

void io_sync_enable(struct io_sync *ios, enum io_sync_op op) {
	assert(ios != NULL);
	assert((op == IO_SYNC_READING) || (op == IO_SYNC_WRITING));

	switch (op) {
	case IO_SYNC_READING:
		manual_event_notify(&ios->can_read);
		if (ios->on_reading != NULL) {
			event_notify(ios->on_reading);
		}
		break;
	case IO_SYNC_WRITING:
		manual_event_notify(&ios->can_write);
		if (ios->on_writing != NULL) {
			event_notify(ios->on_writing);
		}
		break;
	}
}

void io_sync_disable(struct io_sync *ios, enum io_sync_op op) {
	assert(ios != NULL);
	assert((op == IO_SYNC_READING) || (op == IO_SYNC_WRITING));

	switch (op) {
	case IO_SYNC_READING:
		manual_event_reset(&ios->can_read);
		break;
	case IO_SYNC_WRITING:
		manual_event_reset(&ios->can_write);
		break;
	}
}

void io_sync_error(struct io_sync *ios) {
	assert(ios != NULL);

	manual_event_notify(&ios->can_read);
	manual_event_notify(&ios->can_write);
	if (ios->on_error != NULL) {
		event_notify(ios->on_error);
	}
}

int io_sync_ready(struct io_sync *ios, enum io_sync_op op) {
	assert(ios != NULL);
	assert((op == IO_SYNC_READING) || (op == IO_SYNC_WRITING));

	switch (op) {
	case IO_SYNC_READING:
		return manual_event_is_set(&ios->can_read);
	case IO_SYNC_WRITING:
		return manual_event_is_set(&ios->can_write);
	}

	return 0;
}

void io_sync_notify(struct io_sync *ios, enum io_sync_op op,
		struct event *on_op) {
	assert(ios != NULL);
	assert((op == IO_SYNC_READING) || (op == IO_SYNC_WRITING));

	switch (op) {
	case IO_SYNC_READING:
		ios->on_reading = on_op;
		break;
	case IO_SYNC_WRITING:
		ios->on_writing = on_op;
		break;
	}
}

int io_sync_wait(struct io_sync *ios, enum io_sync_op op,
		unsigned long timeout) {
	assert(ios != NULL);
	assert((op == IO_SYNC_READING) || (op == IO_SYNC_WRITING));

	switch (op) {
	case IO_SYNC_READING:
		return manual_event_wait(&ios->can_read, timeout);
	case IO_SYNC_WRITING:
		return manual_event_wait(&ios->can_write, timeout);
	}

	return 0;
}
