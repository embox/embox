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
#include <util/async_ring_buff.h>
#include <util/dlist.h>
#include <kernel/task.h>
#include <kernel/task/idx.h>
#include "../kernel/task/common.h" //TODO this is very bad way to include headers
#include <framework/mod/options.h>
#include <kernel/task/io_sync.h>
#include <kernel/thread/event.h>

#define DEFAULT_PIPE_BUFFER_SIZE OPTION_GET(NUMBER, pipe_buffer_size)
#define MAX_PIPE_BUFFER_SIZE     OPTION_GET(NUMBER, max_pipe_buffer_size)

struct pipe {
	struct async_ring_buff *buff; /**< Buffer to store data */
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

#define writing_on(pipe)  io_op_unblock(&pipe->writing_end->data->write_state)
#define reading_on(pipe)  io_op_unblock(&pipe->reading_end->data->read_state)
#define writing_off(pipe) io_op_block(&pipe->writing_end->data->write_state)
#define reading_off(pipe) io_op_block(&pipe->reading_end->data->read_state)

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
	struct async_ring_buff *pipe_buff;
	void *storage;
	int res = 0;

	pipe = storage = NULL;
	pipe_buff = NULL;

	if (!(storage = malloc(DEFAULT_PIPE_BUFFER_SIZE))
			|| !(pipe = malloc(sizeof(struct pipe)))
			|| !(pipe_buff = malloc(sizeof(struct async_ring_buff)))) {
		res = ENOMEM;
		goto free_memory;
	}

	pipe->buff = pipe_buff;
	pipe->buf_size = DEFAULT_PIPE_BUFFER_SIZE;
	async_ring_buff_init(pipe_buff, 1, DEFAULT_PIPE_BUFFER_SIZE, storage);

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

	if (desc == pipe->reading_end) {
		pipe->reading_end = NULL;
	} else if (desc == pipe->writing_end) {
		pipe->writing_end = NULL;
	}

	/* Free memory if both of ends are closed. */
	if (NULL == pipe->reading_end && NULL == pipe->writing_end) {
		free(pipe->buff->buffer.storage);
		free(pipe->buff);
		free(pipe);
	}

	return 0;
}

static int pipe_read(struct idx_desc *data, void *buf, size_t nbyte) {
	int len;
	struct pipe *pipe;

	pipe = (struct pipe*) task_idx_desc_data(data);

	if (0 == nbyte) {
		return 0;
	}

	len = async_ring_buff_dequeue(pipe->buff, (void*)buf, nbyte);

	if (!(data->flags & O_NONBLOCK)) {
		while (!len) {
			event_wait_ms(&pipe->read_wait, SCHED_TIMEOUT_INFINITE);
			len = async_ring_buff_dequeue(pipe->buff, (void*)buf, nbyte);
		}
	}

	/* Block pipe on reading if pipe is empty. */
	if (async_ring_buff_get_cnt(pipe->buff) == 0) {
		reading_off(pipe);
	}

	/* Unblock pipe on writing if pipe is not full. */
	if (len > 0) {
		event_notify(&pipe->write_wait);
		writing_on(pipe);
	}

	return len;
}

static int pipe_write(struct idx_desc *data, const void *buf, size_t nbyte) {
	int len;
	struct pipe *pipe;

	pipe = (struct pipe*) task_idx_desc_data(data);

	if (NULL == pipe->reading_end) {
		SET_ERRNO(EPIPE);
		return -1;
	}

	if (0 == nbyte) {
		return 0;
	}

	len = async_ring_buff_enqueue(pipe->buff, (void*)buf, nbyte);

	if (!(data->flags & O_NONBLOCK)) {
		while (!len) {
			event_wait_ms(&pipe->write_wait, SCHED_TIMEOUT_INFINITE);
			len = async_ring_buff_enqueue(pipe->buff, (void*)buf, nbyte);
		}
	}

	/* Block pipe on writing if pipe is full. */
	if (async_ring_buff_get_space(pipe->buff) == 0) {
		writing_off(pipe);
	}

	/* Unblock pipe on reading if pipe is not empty. */
	if (len > 0) {
		event_notify(&pipe->read_wait);
		reading_on(pipe);
	}

	return len;
}

static int pipe_fcntl(struct idx_desc *data, int cmd, va_list args) {
	struct pipe *pipe;
	size_t size;

	pipe = (struct pipe*) task_idx_desc_data(data);

	switch (cmd) {
	case O_NONBLOCK:
		writing_on(pipe);
		reading_on(pipe);
	case F_GETPIPE_SZ:
		return pipe->buf_size;
	case F_SETPIPE_SZ:
		size = va_arg(args, size_t);
		pipe_set_buf_size(pipe, size);
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

	free(pipe->buff->buffer.storage);
	async_ring_buff_init(pipe->buff, 1, DEFAULT_PIPE_BUFFER_SIZE, storage);
}
