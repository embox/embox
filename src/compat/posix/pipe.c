/**
 * @file
 * @brief Pipe implementation.
 *
 * @date 12.09.2012
 * @author Alexander Kalmuk
 */

#include <unistd.h>
#include <stdlib.h>
#include <util/async_ring_buff.h>
#include <kernel/task.h>
#include <kernel/task/idx.h>
#include <fs/ioctl.h> /* it must be sys/ioctl.h --Alexander */
#include <framework/mod/options.h>

#define PIPE_BUFFER_SIZE OPTION_GET(NUMBER, pipe_buffer_size)

static int pipe_close(void *data);
static int pipe_read(void *data, void *buf, size_t nbyte);
static int pipe_write(void *data, const void *buf, size_t nbyte);
static int pipe_ioctl(void *data, int request, va_list args);
static int pipe_fseek(void *data, long int offset, int origin);

static const struct task_idx_ops pipe_ops = {
		.read = pipe_read,
		.write = pipe_write,
		.close = pipe_close,
		.ioctl = pipe_ioctl,
		.fseek = pipe_fseek,
		.type = TASK_RES_OPS_REGULAR /* it is so? */
};

struct pipe {
	struct async_ring_buff buff;
	struct event nonfull; /* activates the writer */
	struct event nonempty; /* activates the reader */
	int nonblock;
};

int pipe(int pipefd[2]) {
	int res;
	struct pipe *pipe = malloc(sizeof(struct pipe));
	struct async_ring_buff *pipe_buff = &pipe->buff;
	void *storage = malloc(PIPE_BUFFER_SIZE);

	async_ring_buff_init(pipe_buff, 1, PIPE_BUFFER_SIZE, storage);

	pipe->nonblock = 1; /* pipe is non block by default */

	pipefd[0] = task_self_idx_alloc(&pipe_ops, pipe_buff);
	if (pipefd[0] < 0) {
		pipe_close(pipe_buff);
		return -1;
	/* SET_ERRNO(-1); */
	}

	pipefd[1] = task_self_idx_first_unbinded();
	if (pipefd[1] < 0) {
		task_self_idx_table_unbind(pipefd[0]);
		return -1;
	}

	res = dup2(pipefd[0], pipefd[1]);
	if (res < 0) {
		task_self_idx_table_unbind(pipefd[0]);
		return -1;
	}

	return 0;
}

static int pipe_close(void *data) {
	struct pipe *pipe= (struct pipe*) data;

	free(pipe->buff.buffer.storage);
	free(pipe);

	return 0;
}

static int pipe_read(void *data, void *buf, size_t nbyte) {
	int len;
	struct pipe *pipe = (struct pipe*)data;

	if (!buf) {
		return -1;
	}

	if (!pipe->nonblock) {
		while ((len = async_ring_buff_pop(&pipe->buff, (void*)buf, nbyte)) <= 0) {
			event_wait(&pipe->nonempty, SCHED_TIMEOUT_INFINITE);
		}

		if (len > 0)
			event_notify(&pipe->nonfull);
	} else {
		return async_ring_buff_pop(&pipe->buff, (void*)buf, nbyte);
	}

	return len;
}

static int pipe_write(void *data, const void *buf, size_t nbyte) {
	int len;
	struct pipe *pipe = (struct pipe*)data;

	if (!buf) {
		return -1;
	}

	if (!pipe->nonblock) {
		while ((len = async_ring_buff_push(&pipe->buff, (void*)buf, nbyte)) <= 0) {
			event_wait(&pipe->nonfull, SCHED_TIMEOUT_INFINITE);
		}

		if (len > 0)
			event_notify(&pipe->nonempty);
	} else {
		return async_ring_buff_push(&pipe->buff, (void*)buf, nbyte);
	}

	return len;
}

static int pipe_ioctl(void *data, int request, va_list args) {
	struct pipe *pipe = (struct pipe*)data;

	switch (request) {
	case O_NONBLOCK:
		pipe->nonblock = va_arg(args, int);
		if (!pipe->nonblock) {
			event_init(&pipe->nonfull, "pipe_is_nonfull");
			event_init(&pipe->nonempty, "pipe_is_nonempty");
		}
		break;
	default:
		/* SET_ERRNO(EINVAL) */
		return -1; /* no such request value */
	}

	return 0;
}

static int pipe_fseek(void *data, long int offset, int origin) {
	return 0; /* do nothing */
}
