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
#include <util/dlist.h>
#include <kernel/task.h>
#include <kernel/task/idx.h>
#include "../kernel/task/common.h"
#include <framework/mod/options.h>

#define DEFAULT_PIPE_BUFFER_SIZE OPTION_GET(NUMBER, pipe_buffer_size)
#define MAX_PIPE_BUFFER_SIZE     OPTION_GET(NUMBER, max_pipe_buffer_size)

struct pipe;
struct pipe_end_state;

static int pipe_close(struct idx_desc *desc);
static int pipe_read(struct idx_desc *data, void *buf, size_t nbyte);
static int pipe_write(struct idx_desc *data, const void *buf, size_t nbyte);
static int pipe_ioctl(struct idx_desc *data, int request, va_list args);
static int pipe_fcntl(struct idx_desc *data, int cmd, va_list args);

/* Functions to operate with state of pipe ends */
static void pipe_ends_activate(struct dlist_head *ends);
static void pipe_ends_deactivate(struct dlist_head *ends);
static struct pipe_end_state *get_pipe_end_state_by_op(struct dlist_head *ends, struct idx_io_op_state *op);

/* Set size of pipe's buffer. */
static void pipe_set_buf_size(struct pipe *pipe, size_t size);

struct pipe_end_state {
	struct dlist_head link;
	struct idx_io_op_state *state;
};

struct pipe {
	struct async_ring_buff *buff;
	struct dlist_head readers;
	struct dlist_head writers;
	struct event read_wait;
	struct event write_wait;
	size_t buf_size;
};

static const struct task_idx_ops read_ops = {
		.read = pipe_read,
		.close = pipe_close,
		.ioctl = pipe_ioctl,
		.fcntl = pipe_fcntl,
		.type = TASK_RES_OPS_REGULAR
};

static const struct task_idx_ops write_ops = {
		.write = pipe_write,
		.close = pipe_close,
		.ioctl = pipe_ioctl,
		.fcntl = pipe_fcntl,
		.type = TASK_RES_OPS_REGULAR
};

int pipe(int pipefd[2]) {
	struct pipe *pipe;
	struct async_ring_buff *pipe_buff;
	void *storage;
	struct idx_desc *desc;
	struct pipe_end_state *r, *w;
	int res = 0;

	if (!(storage = malloc(DEFAULT_PIPE_BUFFER_SIZE))
			|| !(pipe_buff = malloc(sizeof(struct async_ring_buff)))
			|| !(pipe = malloc(sizeof(struct pipe)))
			|| !(r = malloc(sizeof(struct pipe_end_state)))
			|| !(w = malloc(sizeof(struct pipe_end_state)))) {
		res = ENOMEM; /* not sure */
		goto buffree;
	}

	dlist_init(&pipe->readers);
	dlist_init(&pipe->writers);

	dlist_head_init(&r->link);
	dlist_add_prev(&r->link, &pipe->readers);

	dlist_head_init(&w->link);
	dlist_add_prev(&w->link, &pipe->writers);

	pipe->buff = pipe_buff;
	pipe->buf_size = DEFAULT_PIPE_BUFFER_SIZE;
	async_ring_buff_init(pipe_buff, 1, DEFAULT_PIPE_BUFFER_SIZE, storage);

	pipefd[1] = task_self_idx_alloc(&write_ops, pipe);
	if (pipefd[1] < 0) {
		res = EMFILE;
		goto buffree;
	}

	pipefd[0] = task_self_idx_alloc(&read_ops, pipe);
	if (pipefd[0] < 0) {
		task_self_idx_table_unbind(pipefd[1]);
		res = EMFILE;
		goto buffree;
	}

	/* Init reader */
	event_init(&pipe->read_wait, "pipe_read_wait");
	desc = task_self_idx_get(pipefd[0]);
	r->state = &desc->data->read_state;

	/* Init writer */
	event_init(&pipe->write_wait, "pipe_write_wait");
	desc = task_self_idx_get(pipefd[1]);
	w->state = &desc->data->write_state;

	return 0;

buffree:
	free(r);
	free(w);
	free(storage);
	free(pipe);
	free(pipe_buff);
	SET_ERRNO(res);
	return -1;
}

static int pipe_close(struct idx_desc *desc) {
	struct pipe *pipe = (struct pipe*) task_idx_desc_data(desc);
	struct pipe_end_state *st = get_pipe_end_state_by_op(&pipe->readers, &desc->data->read_state);

	if (!st) {
		st = get_pipe_end_state_by_op(&pipe->writers, &desc->data->write_state);
	}

	if (st) {
		dlist_del(&st->link);
		free(st);
	}

	if (dlist_empty(&pipe->readers) && dlist_empty(&pipe->writers)) {
		free(pipe->buff->buffer.storage);
		free(pipe->buff);
		free(pipe);
		return 0;
	}

	return 0;
}

static int pipe_read(struct idx_desc *data, void *buf, size_t nbyte) {
	int len;
	struct pipe *pipe = (struct pipe*) task_idx_desc_data(data);

	if (!nbyte || dlist_empty(&pipe->writers)) {
		return 0;
	}

	len = async_ring_buff_dequeue(pipe->buff, (void*)buf, nbyte);

	if (!(data->flags & O_NONBLOCK)) {
		while (!len) {
			event_wait(&pipe->read_wait, SCHED_TIMEOUT_INFINITE);
			len = async_ring_buff_dequeue(pipe->buff, (void*)buf, nbyte);
		}
	}

	if (async_ring_buff_get_cnt(pipe->buff) == 0) {
		pipe_ends_deactivate(&pipe->readers);
	}

	if (len > 0) {
		event_notify(&pipe->write_wait);
		pipe_ends_activate(&pipe->writers);
	}

	return len;
}

static int pipe_write(struct idx_desc *data, const void *buf, size_t nbyte) {
	int len;
	struct pipe *pipe = (struct pipe*) task_idx_desc_data(data);

	if (dlist_empty(&pipe->readers)) {
		SET_ERRNO(EPIPE);
		return -1;
	}

	if (!nbyte) {
		return nbyte;
	}

	len = async_ring_buff_enqueue(pipe->buff, (void*)buf, nbyte);

	if (!(data->flags & O_NONBLOCK)) {
		while (!len) {
			event_wait(&pipe->write_wait, SCHED_TIMEOUT_INFINITE);
			len = async_ring_buff_enqueue(pipe->buff, (void*)buf, nbyte);
		}
	}

	if (async_ring_buff_get_space(pipe->buff) == 0) {
		pipe_ends_deactivate(&pipe->writers);
	}

	if (len > 0) {
		event_notify(&pipe->read_wait);
		pipe_ends_activate(&pipe->readers);
	}

	return len;
}

static int pipe_ioctl(struct idx_desc *data, int request, va_list args) {
	return 0;
}

static int pipe_fcntl(struct idx_desc *data, int cmd, va_list args) {
	struct pipe *pipe = (struct pipe*) task_idx_desc_data(data);
	size_t size;

	switch (cmd) {
	case O_NONBLOCK:
		pipe_ends_activate(&pipe->writers);
		pipe_ends_activate(&pipe->readers);
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

static void pipe_ends_activate(struct dlist_head *ends) {
	struct pipe_end_state *cur, *nxt;
	dlist_foreach_entry(cur, nxt, ends, link) {
		task_idx_io_activate(cur->state);
	}
}

static void pipe_ends_deactivate(struct dlist_head *ends) {
	struct pipe_end_state *cur, *nxt;
	dlist_foreach_entry(cur, nxt, ends, link) {
		task_idx_io_deactivate(cur->state);
	}
}

static struct pipe_end_state *get_pipe_end_state_by_op(struct dlist_head *ends, struct idx_io_op_state *op) {
	struct pipe_end_state *cur, *nxt;

	dlist_foreach_entry(cur, nxt, ends, link) {
		if (cur->state == op) {
			return cur;
		}
	}

	return NULL;
}

/* Called when idx descriptors are inherit. */
static void inherit_if_pipe(struct idx_desc *d) {
	struct pipe *pipe;
	struct pipe_end_state *state;

	if (d->data->res_ops != &read_ops && d->data->res_ops != &write_ops) { /* check if it is pipe */
		return;
	}
	pipe = (struct pipe*) task_idx_desc_data(d);
	state = malloc(sizeof(struct pipe_end_state));

	dlist_head_init(&state->link);

	if (d->data->res_ops == &read_ops) {
		state->state = &d->data->read_state;
		dlist_add_prev(&state->link, &pipe->readers);
	} else {
		state->state = &d->data->write_state;
		dlist_add_prev(&state->link, &pipe->writers);
	}
}

static void pipe_inherit(struct task *task, struct task *parent) {
	struct task_idx_table *idx_table = task_idx_table(task);

	for (int i = 0; i < TASKS_RES_QUANTITY; i++) {
		if (task_idx_table_is_binded(idx_table, i)) {
			inherit_if_pipe(task_self_idx_get(i));
		}
	}
}

static void pipe_init(struct task *task, void *a) {}
static void pipe_deinit(struct task *task) {}

static const struct task_resource_desc pipe_resource = {
	.init = pipe_init,
	.inherit = pipe_inherit,
	.deinit = pipe_deinit,
	.resource_size = 0
};

TASK_RESOURCE_DESC(&pipe_resource);
