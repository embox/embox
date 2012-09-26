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
#include <util/async_ring_buff.h>
#include <kernel/task.h>
#include <kernel/task/idx.h>
#include <framework/mod/options.h>

#define PIPE_BUFFER_SIZE OPTION_GET(NUMBER, pipe_buffer_size)

static int pipe_close(struct idx_desc_data *data);
static int pipe_read(struct idx_desc_data *data, void *buf, size_t nbyte);
static int pipe_write(struct idx_desc_data *data, const void *buf, size_t nbyte);
static int pipe_ioctl(struct idx_desc_data *data, int request, va_list args);

static const struct task_idx_ops read_ops = {
		.read = pipe_read,
		.close = pipe_close,
		.ioctl = pipe_ioctl,
		.type = TASK_RES_OPS_REGULAR
};

static const struct task_idx_ops write_ops = {
		.write = pipe_write,
		.close = pipe_close,
		.ioctl = pipe_ioctl,
		.type = TASK_RES_OPS_REGULAR
};

struct pipe {
	struct async_ring_buff buff;
	struct event nonfull; /* activates the writer */
	struct event nonempty; /* activates the reader */
	int ends_count : 2; /* 0, 1 or 2 */
};

int pipe(int pipefd[2]) {
	struct pipe *pipe = malloc(sizeof(struct pipe));
	struct async_ring_buff *pipe_buff = &pipe->buff;
	void *storage = malloc(PIPE_BUFFER_SIZE);

	async_ring_buff_init(pipe_buff, 1, PIPE_BUFFER_SIZE, storage);

	pipe->ends_count = 2;
	event_init(&pipe->nonfull, "pipe_is_nonfull");
	event_init(&pipe->nonempty, "pipe_is_nonempty");

	pipefd[0] = task_self_idx_alloc(&write_ops, pipe_buff);
	if (pipefd[0] < 0) {
		free(storage);
		free(pipe);
		SET_ERRNO(EMFILE);
		return -1;
	}

	pipefd[1] = task_self_idx_alloc(&read_ops, pipe_buff);
	if (pipefd[1] < 0) {
		task_self_idx_table_unbind(pipefd[0]);
		free(storage);
		free(pipe);
		SET_ERRNO(EMFILE);
		return -1;
	}

	return 0;
}

static int pipe_close(struct idx_desc_data *data) {
	struct pipe *pipe= (struct pipe*) data->fd_struct;

	if (!(--pipe->ends_count)) {
		free(pipe->buff.buffer.storage);
		free(pipe);
	}

	return 0;
}

static int pipe_read(struct idx_desc_data *data, void *buf, size_t nbyte) {
	int len;
	struct pipe *pipe = (struct pipe*)data->fd_struct;

	if (!nbyte || pipe->ends_count == 1) {
		return 0;
	}

	if (data->flags & O_NONBLOCK) {
		len = async_ring_buff_dequeue(&pipe->buff, (void*)buf, nbyte);
	} else {
		while (!(len = async_ring_buff_dequeue(&pipe->buff, (void*)buf, nbyte))) {
			event_wait(&pipe->nonempty, SCHED_TIMEOUT_INFINITE);
		}
	}

	if (len > 0) {
		event_notify(&pipe->nonfull);
	}

	return len;
}

static int pipe_write(struct idx_desc_data *data, const void *buf, size_t nbyte) {
	int len;
	struct pipe *pipe = (struct pipe*)data->fd_struct;

	if (pipe->ends_count == 1) {
		SET_ERRNO(EPIPE);
		return -1;
	}

	if (!nbyte) {
		return nbyte;
	}

	if (data->flags & O_NONBLOCK) {
		len = async_ring_buff_enqueue(&pipe->buff, (void*)buf, nbyte);
	} else {
		while (!(len = async_ring_buff_enqueue(&pipe->buff, (void*)buf, nbyte))) {
			event_wait(&pipe->nonfull, SCHED_TIMEOUT_INFINITE);
		}
	}

	if (len > 0) {
		event_notify(&pipe->nonempty);
	}

	return len;
}

static int pipe_ioctl(struct idx_desc_data *data, int request, va_list args) {
	return 0;
}
