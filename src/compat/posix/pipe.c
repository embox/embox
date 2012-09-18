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

int pipe(int pipefd[2]) {
	int res;
	struct async_ring_buff *pipe_buff = malloc(sizeof(struct async_ring_buff));
	void *storage = malloc(PIPE_BUFFER_SIZE);

	async_ring_buff_init(pipe_buff, 1, PIPE_BUFFER_SIZE, storage);

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
	struct async_ring_buff *pipe_buff = (struct async_ring_buff*) data;

	free(pipe_buff->buffer.storage);
	free(pipe_buff);

	return 0;
}

static int pipe_read(void *data, void *buf, size_t nbyte) {
	if (!buf) {
		return -1;
	}

	return async_ring_buff_deque((struct async_ring_buff*)data, buf, nbyte);
}

static int pipe_write(void *data, const void *buf, size_t nbyte) {
	if (!buf) {
		return -1;
	}

	return async_ring_buff_enque((struct async_ring_buff*)data, (void*)buf, nbyte);
}

static int pipe_ioctl(void *data, int request, va_list args) {
	return 0; /* do nothing */
}

static int pipe_fseek(void *data, long int offset, int origin) {
	return 0; /* do nothing */
}
