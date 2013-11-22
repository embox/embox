/**
 * @file
 *
 * @brief
 *
 * @date 17.11.2013
 * @author Alexander Kalmuk
 * @author Anton Kozlov
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
#include <fs/flags.h>

#include <unistd.h>

struct idesc_pipe {
	struct idesc idesc;
	struct pipe *pipe;
};

static inline void writing_enable(struct pipe *pipe) {
//	if (pipe->writing_end)
//		io_sync_enable(&pipe->writing_end->idesc_event.io_sync, IO_SYNC_WRITING);
}

static inline void reading_enable(struct pipe *pipe) {
//	if (pipe->reading_end)
//		io_sync_enable(&pipe->reading_end->idesc_event.io_sync, IO_SYNC_READING);
}

static inline void writing_disable(struct pipe *pipe) {
//	if (pipe->writing_end)
//		io_sync_disable(&pipe->writing_end->idesc_event.io_sync, IO_SYNC_WRITING);
}

static inline void reading_disable(struct pipe *pipe) {
//	if (pipe->reading_end)
//		io_sync_disable(&pipe->reading_end->idesc_event.io_sync, IO_SYNC_READING);
}

int pipe_close(struct idesc *idesc) {
	struct idesc_pipe *ip;
	struct pipe *pipe;

	ip = (struct idesc_pipe *) idesc;
	pipe = ip->pipe;

	sched_lock();
	{
		if (idesc == pipe->reading_end) {
			pipe->reading_end = NULL;
			/* Wake up writing end if it is sleeping. */
			event_notify(&pipe->write_wait);
		} else if (idesc == pipe->writing_end) {
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

int pipe_read(struct idesc *idesc, void *buf, size_t nbyte) {
	int len;
	struct idesc_pipe *ip;
	struct pipe *pipe;

	ip = (struct idesc_pipe *) idesc;
	pipe = ip->pipe;

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
		if (!(idesc->idesc_flags & O_NONBLOCK)) {
			if (!len) {
				EVENT_WAIT(&pipe->read_wait, 0, SCHED_TIMEOUT_INFINITE); /* TODO: event condition */
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

int pipe_write(struct idesc *idesc, const void *buf, size_t nbyte) {
	int len;
	struct pipe *pipe;
	struct idesc_pipe *ip;

	ip = (struct idesc_pipe *) idesc;
	pipe = ip->pipe;

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
		if (!(idesc->idesc_flags & O_NONBLOCK)) {
			if (!len) {
				EVENT_WAIT(&pipe->write_wait, 0, SCHED_TIMEOUT_INFINITE); /* TODO: event condition */
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
		/*.fcntl = pipe_fcntl,*/
};

static const struct task_idx_ops write_ops = {
		.write = pipe_write,
		.close = pipe_close,
		/*.fcntl = pipe_fcntl,*/
};

static struct idesc_pipe *idesc_pipe_alloc(struct pipe *pipe,
		const struct task_idx_ops *ops, idesc_access_mode_t amode,
		struct idesc **id) {
	struct idesc_pipe *idp;

	idp = malloc(sizeof(struct idesc_pipe));

	if (idp) {
		idesc_init(&idp->idesc, ops, amode);

		idp->pipe = pipe;
		*id = &idp->idesc;

		//io_sync_init(&idp->idesc.idesc_event.io_sync, 0, 0);
	}

	return idp;
}

static struct idesc_pipe *idesc_pipe_alloc_reading(struct pipe *pipe) {

	return idesc_pipe_alloc(pipe, &read_ops, FS_MAY_READ, &pipe->reading_end);
}

static struct idesc_pipe *idesc_pipe_alloc_writing(struct pipe *pipe) {

	return idesc_pipe_alloc(pipe, &write_ops, FS_MAY_WRITE, &pipe->writing_end);
}

static void idesc_pipe_free(struct idesc_pipe *idp) {

	free(idp);
}

static struct pipe *pipe_alloc(void) {
	struct pipe *pipe;
	struct ring_buff *pipe_buff;
        void *storage;
        int res = 0;

        pipe = storage = NULL;
        pipe_buff = NULL;

        if (!(storage = malloc(DEFAULT_PIPE_BUFFER_SIZE))
                        || !(pipe = malloc(sizeof(struct pipe)))
                        || !(pipe_buff = malloc(sizeof(struct ring_buff)))) {
                goto free_memory;
        }

        pipe->buff = pipe_buff;
        pipe->buf_size = DEFAULT_PIPE_BUFFER_SIZE;
        ring_buff_init(pipe_buff, 1, DEFAULT_PIPE_BUFFER_SIZE, storage);
	event_init(&pipe->read_wait, "pipe_read_wait");
	event_init(&pipe->write_wait, "pipe_write_wait");

	return pipe;

free_memory:
        if (storage)   free(storage);
        if (pipe_buff) free(pipe_buff);
        if (pipe)      free(pipe);
        SET_ERRNO(res);
        return NULL;

}

static void pipe_free(struct pipe *pipe) {

	free(pipe->buff->storage);
	free(pipe->buff);
	free(pipe);
}

int pipe(int pipefd[2]) {

	return pipe2(pipefd, 0);
}

int pipe2(int pipefd[2], int flags) {
	struct idesc_table *it;
	struct pipe *pipe;
	struct idesc_pipe *idpr, *idpw;
	int res = 0;

	it = idesc_table_get_table(task_self());

	pipe = NULL;
	idpr = idpw = NULL;

	pipe = pipe_alloc();
	if (!pipe) {
		res = ENOMEM;
		goto out_err;
	}

	idpr = idesc_pipe_alloc_reading(pipe);
	idpw = idesc_pipe_alloc_writing(pipe);

	if (!idpr || !idpw) {
		res = ENOMEM;
		goto out_err;
	}

	pipefd[0] = idesc_table_add(it, &idpr->idesc, 0);
	pipefd[1] = idesc_table_add(it, &idpw->idesc, 0);

	if (pipefd[0] < 0 || pipefd[1] < 0) {
		res = ENOMEM;
		goto out_err;
	}

	io_sync_enable(&pipe->writing_end->idesc_event.io_sync, IO_SYNC_WRITING);

	return 0;

out_err:
	if (pipefd[1] >= 0) {
		idesc_table_del(it, pipefd[1]);
	}
	if (pipefd[0] >= 0) {
		idesc_table_del(it, pipefd[0]);
	}
	if (idpw) {
		idesc_pipe_free(idpw);
	}
	if (idpr) {
		idesc_pipe_free(idpr);
	}
	if (pipe) {
		pipe_free(pipe);
	}
	SET_ERRNO(res);
	return -1;
}
