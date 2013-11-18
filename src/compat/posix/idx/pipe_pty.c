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
#include <fcntl.h>
#include <string.h>
#include <termios.h>

#include <util/ring_buff.h>
#include <util/ring.h>

#include <drivers/termios_ops.h>
#include <kernel/task.h>
#include <kernel/task/idx.h>
#include <fs/pipe.h>

#include <kernel/task/idesc_table.h>


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

static int inject_ops(int fd, const struct task_idx_ops *ops, const struct termios *tio,
		const struct task_idx_ops **backup) {
#ifndef IDESC_TABLE_USE
	struct idesc *idx_desc = task_self_idx_get(fd);
	struct pipe *pipe = (struct pipe*) task_idx_desc_data(idx_desc);

	if (!idx_desc) {
		return -EBADF;
	}

	if (backup) {
		*backup = task_idx_indata(idx_desc)->res_ops;
	}

	task_idx_indata(idx_desc)->res_ops = ops;
#else
	//struct idesc *idesc = idesc_common_get(fd);
	struct pipe *pipe = NULL;
#endif

	if (tio) {
		memcpy(&pipe->tio, tio, sizeof(struct termios));
		pipe->has_tio = 1;
	} else {
		pipe->has_tio = 0;
	}

	return 0;
}



static int pipe_pty_ioctl(struct idesc *desc, int request, void *data) {
	struct pipe *pipe;
	size_t size;

	pipe = NULL;//(struct pipe*) task_idx_desc_data(data);

	switch (request) {
	case F_GETPIPE_SZ:
		return pipe->buf_size;
	case F_SETPIPE_SZ:
		size = (size_t)data;
		pipe_set_buf_size(pipe, size);
		break;
	default:
		break;
	}


	return 0;
}

static int pipe_pty_fstat(struct idesc *data, void *buff) {
	struct stat *st = buff;

	st->st_mode = S_IFCHR;

	return 0;

}
extern int pipe_write(struct idesc *data, const void *buf, size_t nbyte);

static int pipe_pty_write(struct idesc *desc, const void *buf, size_t nbyte) {
	char tbuf[4];
	struct ring r;
	struct pipe *pipe = NULL;//(struct pipe*) task_idx_desc_data(desc);
	const char *c = buf;
	int i;

	if (pipe->has_tio == 0) {
		return pipe_write(desc, buf, nbyte);
	}

	ring_init(&r);

	for (i = 0; i < nbyte; i++) {

		termios_putc((struct termios *) &pipe->tio, c[i], &r, tbuf, 4);

		while (!ring_empty(&r)) {
			pipe_write(desc, &tbuf[r.tail], 1);
			ring_just_read(&r, 4, 1);
		}
	}

	return nbyte;
}
extern int pipe_close(struct idesc *desc);
extern int pipe_read(struct idesc *data, void *buf, size_t nbyte);

static const struct task_idx_ops pipe_pty_ops = {
		.write = pipe_pty_write,
		.read  = pipe_read,
		.close = pipe_close,
		//.fcntl = pipe_fcntl,
		.ioctl = pipe_pty_ioctl,
		.fstat = pipe_pty_fstat,
};

int pipe_pty(int pipe[2], const struct termios *tio) {
	int res;
	const struct task_idx_ops *backup1, *backup2;

	if (0 > (res = inject_ops(pipe[0], &pipe_pty_ops, tio, &backup1))) {
		return res;
	}

	if (0 > (res = inject_ops(pipe[1], &pipe_pty_ops, tio, &backup2))) {
		inject_ops(pipe[0], backup1, NULL, NULL);

		return res;
	}

	return 0;
}

