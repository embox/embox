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

static int pipe_close(struct idx_desc *data);
static int pipe_read(struct idx_desc *data, void *buf, size_t nbyte);
static int pipe_write(struct idx_desc *data, const void *buf, size_t nbyte);
static int pipe_ioctl(struct idx_desc *data, int request, va_list args);

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
	int pipefd[2];   /* ends of pipe */
	int ends_count; /* current number of ends: 0, 1 or 2 */
};

int pipe(int pipefd[2]) {
	struct pipe *pipe;
	struct async_ring_buff *pipe_buff;
	void *storage;
	struct event_set *set_read, *set_write;
	struct idx_desc *desc;
	int res = 0;

	if (!(storage = malloc(PIPE_BUFFER_SIZE))) {
		res = ENOMEM; /* not sure */
		goto out;
	}

	if (!(pipe = malloc(sizeof(struct pipe)))) {
		free(storage);
		res = ENOMEM; /* not sure */
		goto out;
	}

	pipe_buff = &pipe->buff;

	async_ring_buff_init(pipe_buff, 1, PIPE_BUFFER_SIZE, storage);

	pipe->ends_count = 2;

	pipefd[1] = task_self_idx_alloc(&write_ops, pipe_buff);
	if (pipefd[1] < 0) {
		res = EMFILE;
		goto buffree;
	}
	pipe->pipefd[1] = pipefd[1];

	pipefd[0] = task_self_idx_alloc(&read_ops, pipe_buff);
	if (pipefd[0] < 0) {
		task_self_idx_table_unbind(pipefd[1]);
		res = EMFILE;
		goto buffree;
	}
	pipe->pipefd[0] = pipefd[0];

	/* Init read event */
	if (!(set_read = event_set_create())) {
		res = ENOMEM;
		goto buffree_and_unbind;
	}
	desc = task_self_idx_get(pipefd[0]);
	event_set_add(set_read, &desc->data->read_state.activate);

	/* Init write event */
	if (!(set_write = event_set_create())) {
		event_set_clear(set_read);
		res = ENOMEM;
		goto buffree_and_unbind;
	}
	desc = task_self_idx_get(pipefd[1]);
	event_set_add(set_write, &desc->data->write_state.activate);

	return 0;

buffree_and_unbind:
	task_self_idx_table_unbind(pipefd[0]);
	task_self_idx_table_unbind(pipefd[1]);
buffree:
	free(storage);
	free(pipe);
out:
	SET_ERRNO(res);
	return -1;
}

static void update_ends_cnt(struct pipe *pipe) {
	struct idx_desc *reader = task_self_idx_get(pipe->pipefd[0]);
	struct idx_desc *writer = task_self_idx_get(pipe->pipefd[1]);

	pipe->ends_count = reader->data->link_count + writer->data->link_count;
}

/* XXX */
static int pipe_close(struct idx_desc *data) {
	struct pipe *pipe = (struct pipe*) task_idx_desc_data(data);
//	struct event_set *e_read = data->data->read_state.activate.set;
//	struct event_set *e_write = data->data->write_state.activate.set;
//
//	if (e_read) {
//		event_set_clear(e_read);
//	}
//
//	if (e_write) {
//		event_set_clear(e_write);
//	}
//
//	update_pipe_info(pipe);

	if (!(--pipe->ends_count)) {
		free(pipe->buff.buffer.storage);
		free(pipe);
	}

	return 0;
}

static int pipe_read(struct idx_desc *data, void *buf, size_t nbyte) {
	int len;
	struct pipe *pipe= (struct pipe*) task_idx_desc_data(data);
	struct idx_desc *writer;

	update_ends_cnt(pipe);

	if (!nbyte || pipe->ends_count == 1) {
		return 0;
	}

	writer = task_self_idx_get(pipe->pipefd[1]);

	len = async_ring_buff_dequeue(&pipe->buff, (void*)buf, nbyte);

	if (!(data->flags & O_NONBLOCK)) {
		while (!len) {
			event_wait(data->data->read_state.activate.set, SCHED_TIMEOUT_INFINITE);
			len = async_ring_buff_dequeue(&pipe->buff, (void*)buf, nbyte);
		}
	}

	if (len > 0) {
		task_idx_io_activate(&writer->data->write_state);
	}

	if (async_ring_buff_get_cnt(&pipe->buff) == 0) {
		task_idx_io_deactivate(&data->data->read_state);
	}

	return len;
}

static int pipe_write(struct idx_desc *data, const void *buf, size_t nbyte) {
	int len;
	struct pipe *pipe= (struct pipe*) task_idx_desc_data(data);
	struct idx_desc *reader;

	update_ends_cnt(pipe);

	if (pipe->ends_count == 1) {
		SET_ERRNO(EPIPE);
		return -1;
	}

	reader = task_self_idx_get(pipe->pipefd[0]);

	if (!nbyte) {
		return nbyte;
	}

	len = async_ring_buff_enqueue(&pipe->buff, (void*)buf, nbyte);

	if (!(data->flags & O_NONBLOCK)) {
		while (!len) {
			event_wait(data->data->write_state.activate.set, SCHED_TIMEOUT_INFINITE);
			len = async_ring_buff_enqueue(&pipe->buff, (void*)buf, nbyte);
		}
	}

	if (len > 0) {
		task_idx_io_activate(&reader->data->read_state);
	}

	if (async_ring_buff_get_space(&pipe->buff) == 0) {
		task_idx_io_deactivate(&data->data->write_state);
	}

	return len;
}

static int pipe_ioctl(struct idx_desc *data, int request, va_list args) {
	return 0;
}
