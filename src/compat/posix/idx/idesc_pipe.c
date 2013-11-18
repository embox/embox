/**
 * @file
 *
 * @brief
 *
 * @date 17.11.2013
 * @author Alexander Kalmuk
 */
#include <stddef.h>
#include <stdlib.h>
#include <errno.h>

#include <util/ring_buff.h>
#include <util/ring.h>

#include <kernel/task/io_sync.h>
#include <kernel/sched.h>
#include <kernel/event.h>

#include <kernel/task.h>
#include <kernel/task/idx.h>

#include <kernel/task/idesc_table.h>
#include <fs/idesc.h>
#include <fcntl.h>

#include <fs/pipe.h>


static inline void writing_enable(struct pipe *pipe) {
	if (pipe->writing_end)
		io_sync_enable(pipe->writing_end->ios, IO_SYNC_WRITING);
}

static inline void reading_enable(struct pipe *pipe) {
	if (pipe->reading_end)
		io_sync_enable(pipe->reading_end->ios, IO_SYNC_READING);
}

static inline void writing_disable(struct pipe *pipe) {
	if (pipe->writing_end)
		io_sync_disable(pipe->writing_end->ios, IO_SYNC_WRITING);
}

static inline void reading_disable(struct pipe *pipe) {
	if (pipe->reading_end)
		io_sync_disable(pipe->reading_end->ios, IO_SYNC_READING);
}


static inline int pipe_rx_open(void) {
	return 0;
}

int pipe_close(struct idesc *desc) {
	struct pipe *pipe;

	pipe = NULL;//(struct pipe*) task_idx_desc_data(desc);

	sched_lock();
	{
#ifndef IDESC_TABLE_USE
		if (task_idx_indata(desc) == pipe->reading_end) {
			pipe->reading_end = NULL;
			/* Wake up writing end if it is sleeping. */
			event_notify(&pipe->write_wait);
		} else if (task_idx_indata(desc) == pipe->writing_end) {
			pipe->writing_end = NULL;
			/* Wake up reading end if it is sleeping. */
			event_notify(&pipe->read_wait);
		}
#else
#endif
		/* Free memory if both of ends are closed. */
		if (NULL == pipe->reading_end && NULL == pipe->writing_end) {
			free(pipe->buff->storage);
			free(pipe->buff);
			free(pipe);
		}

	}
	sched_unlock();

	return 0;
}

int pipe_read(struct idesc *data, void *buf, size_t nbyte) {
	int len;
	struct pipe *pipe;

	pipe = NULL;//(struct pipe*) task_idx_desc_data(data);

	if (0 == nbyte) {
		return 0;
	}

	sched_lock();
	{
		len = ring_buff_dequeue(pipe->buff, (void*)buf, nbyte);

		/* If writing end is closed that means it was last data in pipe. */
		if (NULL == pipe->writing_end) {
			sched_unlock();
			return len;
		}
#if 0
		if (!(data->flags & O_NONBLOCK)) {
			if (!len) {
				EVENT_WAIT(&pipe->read_wait, 0, SCHED_TIMEOUT_INFINITE); /* TODO: event condition */
				len = ring_buff_dequeue(pipe->buff, (void*)buf, nbyte);
			}
		}
#endif
		/* Block pipe on reading if pipe is empty. */
		if (ring_buff_get_cnt(pipe->buff) == 0) {
			reading_disable(pipe);
		}

		/* Unblock pipe on writing if pipe is not full. */
		if (len > 0) {
			event_notify(&pipe->write_wait);
			writing_enable(pipe);
		}
	}
	sched_unlock();

	return len;
}

int pipe_write(struct idesc *data, const void *buf, size_t nbyte) {
	int len;
	struct pipe *pipe;

	pipe = NULL;//(struct pipe*) task_idx_desc_data(data);

	sched_lock();
	{
		/* If reading end is closed that means it is not reason for further writing. */
		if (NULL == pipe->reading_end) {
			SET_ERRNO(EPIPE);
			sched_unlock();
			return -1;
		}

		if (0 == nbyte) {
			sched_unlock();
			return 0;
		}

		len = ring_buff_enqueue(pipe->buff, (void*)buf, nbyte);
#if 0
		if (!(data->flags & O_NONBLOCK)) {
			if (!len) {
				EVENT_WAIT(&pipe->write_wait, 0, SCHED_TIMEOUT_INFINITE); /* TODO: event condition */
				len = ring_buff_enqueue(pipe->buff, (void*)buf, nbyte);
			}
		}
#endif
		/* Block pipe on writing if pipe is full. */
		if (ring_buff_get_space(pipe->buff) == 0) {
			writing_disable(pipe);
		}

		/* Unblock pipe on reading if pipe is not empty. */
		if (len > 0) {
			event_notify(&pipe->read_wait);
			reading_enable(pipe);
		}
	}
	sched_unlock();

	return len;
}
#if 0
static inline int pipe_fcntl(struct idesc *data, int cmd, va_list args) {
	struct pipe *pipe;
	size_t size;

	pipe = (struct pipe*) task_idx_desc_data(data);

	switch (cmd) {
	case F_GETPIPE_SZ:
		return pipe->buf_size;
	case F_SETPIPE_SZ:
		size = va_arg(args, size_t);
		pipe_set_buf_size(pipe, size);
		break;
	default:
		break;
	}

	return 0;
}
#endif


static const struct task_idx_ops read_ops = {
		.read = pipe_read,
		.close = pipe_close,
		//.fcntl = pipe_fcntl,
};

static const struct task_idx_ops write_ops = {
		.write = pipe_write,
		.close = pipe_close,
};
