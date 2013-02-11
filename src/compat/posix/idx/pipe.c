/**
 * @file
 * @brief Pipe implementation.
 *
 * @date 12.09.2012
 * @author Alexander Kalmuk
 */

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>

#include "../kernel/task/common.h" //TODO this is very bad way to include headers
#include <kernel/task.h>
#include <kernel/task/idx.h>
#include <kernel/task/io_sync.h>
#include <kernel/thread/sched.h>
#include <kernel/thread/event.h>

#include <util/ring_buff.h>
#include <framework/mod/options.h>

#define DEFAULT_PIPE_BUFFER_SIZE OPTION_GET(NUMBER, pipe_buffer_size)
#define MAX_PIPE_BUFFER_SIZE     OPTION_GET(NUMBER, max_pipe_buffer_size)

struct pipe {
	struct ring_buff *buff;       /**< Buffer to store data */
	size_t buf_size;              /**< Size of buffer. May be changed by pipe_set_buf_size() */
	struct idx_desc *reading_end; /**< Reading end of pipe */
	struct idx_desc *writing_end; /**< Writing end of pipe */
	struct event read_wait;       /**< Event to activate reading when pipe was not empty */
	struct event write_wait;      /**< Event to activate writing when pipe was not full */
};

static int pipe_close(struct idx_desc *desc);
static int pipe_read(struct idx_desc *data, void *buf, size_t nbyte);
static int pipe_write(struct idx_desc *data, const void *buf, size_t nbyte);
static int pipe_fcntl(struct idx_desc *data, int cmd, va_list args);

/* Set size of pipe's buffer. */
static void pipe_set_buf_size(struct pipe *pipe, size_t size);

static inline void writing_enable(struct pipe *pipe) {
	if (pipe->writing_end)
		io_op_unblock(&pipe->writing_end->data->write_state);
}

static inline void reading_enable(struct pipe *pipe) {
	if (pipe->reading_end)
		io_op_unblock(&pipe->reading_end->data->read_state);
}

static inline void writing_disable(struct pipe *pipe) {
	if (pipe->writing_end)
		io_op_block(&pipe->writing_end->data->write_state);
}

static inline void reading_disable(struct pipe *pipe) {
	if (pipe->reading_end)
		io_op_block(&pipe->reading_end->data->read_state);
}

static const struct task_idx_ops read_ops = {
		.read = pipe_read,
		.close = pipe_close,
		.fcntl = pipe_fcntl,
		.type = TASK_RES_OPS_REGULAR
};

static const struct task_idx_ops write_ops = {
		.write = pipe_write,
		.close = pipe_close,
		.fcntl = pipe_fcntl,
		.type = TASK_RES_OPS_REGULAR
};

int pipe(int pipefd[2]) {
	struct pipe *pipe;
	struct ring_buff *pipe_buff;
	void *storage;
	int res = 0;

	pipe = storage = NULL;
	pipe_buff = NULL;

	if (!(storage = malloc(DEFAULT_PIPE_BUFFER_SIZE))
			|| !(pipe = malloc(sizeof(struct pipe)))
			|| !(pipe_buff = malloc(sizeof(struct ring_buff)))) {
		res = ENOMEM;
		goto free_memory;
	}

	pipe->buff = pipe_buff;
	pipe->buf_size = DEFAULT_PIPE_BUFFER_SIZE;
	ring_buff_init(pipe_buff, 1, DEFAULT_PIPE_BUFFER_SIZE, storage);

	pipefd[1] = task_self_idx_alloc(&write_ops, pipe);
	if (pipefd[1] < 0) {
		res = EMFILE;
		goto free_memory;
	}

	pipefd[0] = task_self_idx_alloc(&read_ops, pipe);
	if (pipefd[0] < 0) {
		task_self_idx_table_unbind(pipefd[1]);
		res = EMFILE;
		goto free_memory;
	}

	/* Init reader */
	event_init(&pipe->read_wait, "pipe_read_wait");
	pipe->reading_end = task_self_idx_get(pipefd[0]);

	/* Init writer */
	event_init(&pipe->write_wait, "pipe_write_wait");
	pipe->writing_end = task_self_idx_get(pipefd[1]);

	return 0;

free_memory:
	if (storage)   free(storage);
	if (pipe_buff) free(pipe_buff);
	if (pipe)      free(pipe);
	SET_ERRNO(res);
	return -1;
}

static int pipe_close(struct idx_desc *desc) {
	struct pipe *pipe;

	pipe = (struct pipe*) task_idx_desc_data(desc);

	sched_lock();
	{
		if (desc == pipe->reading_end) {
			pipe->reading_end = NULL;
			/* Wake up writing end if it is sleeping. */
			event_notify(&pipe->write_wait);
		} else if (desc == pipe->writing_end) {
			pipe->writing_end = NULL;
			/* Wake up reading end if it is sleeping. */
			event_notify(&pipe->read_wait);
		}

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

static int pipe_read(struct idx_desc *data, void *buf, size_t nbyte) {
	int len;
	struct pipe *pipe;

	pipe = (struct pipe*) task_idx_desc_data(data);

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

		if (!(data->flags & O_NONBLOCK)) {
			if (!len) {
				event_wait_ms(&pipe->read_wait, SCHED_TIMEOUT_INFINITE);
				len = ring_buff_dequeue(pipe->buff, (void*)buf, nbyte);
			}
		}

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

static int pipe_write(struct idx_desc *data, const void *buf, size_t nbyte) {
	int len;
	struct pipe *pipe;

	pipe = (struct pipe*) task_idx_desc_data(data);

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

		if (!(data->flags & O_NONBLOCK)) {
			if (!len) {
				event_wait_ms(&pipe->write_wait, SCHED_TIMEOUT_INFINITE);
				len = ring_buff_enqueue(pipe->buff, (void*)buf, nbyte);
			}
		}

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

static int pipe_fcntl(struct idx_desc *data, int cmd, va_list args) {
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

static void pipe_set_buf_size(struct pipe *pipe, size_t size) {
	void *storage;

	if (size > MAX_PIPE_BUFFER_SIZE )
		return;

	if (!(storage = malloc(size))) {
		return;
	}

	sched_lock();
	{
		free(pipe->buff->storage);
		ring_buff_init(pipe->buff, 1, DEFAULT_PIPE_BUFFER_SIZE, storage);
	}
	sched_unlock();
}
