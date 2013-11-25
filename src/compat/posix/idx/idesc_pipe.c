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

#include <framework/mod/options.h>
#include <kernel/thread/sync/mutex.h>
#include <kernel/task/idx.h>
#include <kernel/task/idesc_table.h>
#include <fs/idesc.h>
#include <fs/idesc_event.h>
#include <fs/flags.h>

#include <unistd.h>

#define DEFAULT_PIPE_BUFFER_SIZE OPTION_GET(NUMBER, pipe_buffer_size)
#define MAX_PIPE_BUFFER_SIZE     OPTION_GET(NUMBER, max_pipe_buffer_size)

struct ring_buff;
struct idesc;

struct pipe {
	struct ring_buff *buff;         /**< Buffer to store data */
	size_t buf_size;                /**< Size of buffer. May be changed by pipe_set_buf_size() */

	struct mutex mutex;             /**< Global pipe mutex */

	struct idesc *reading_end;      /**< Reading end of pipe */
	struct idesc *writing_end;      /**< Writing end of pipe */

};

struct idesc_pipe {
	struct idesc idesc;
	struct pipe *pipe;
};

int pipe_close(struct idesc *idesc) {
	struct pipe *pipe;
	struct idesc *other_end;

	pipe = ((struct idesc_pipe *) idesc)->pipe;
	mutex_lock(&pipe->mutex);

	if (idesc == pipe->reading_end) {
		pipe->reading_end = NULL;
		other_end = pipe->writing_end;
	} else if (idesc == pipe->writing_end) {
		pipe->writing_end = NULL;
		other_end = pipe->reading_end;
	}

	if (NULL == other_end) {
		free(pipe->buff->storage);
		free(pipe->buff);
		free(pipe);
	} else {
		idesc_notify_all(other_end, IDESC_EVENT_ERROR);
	}

	mutex_unlock(&pipe->mutex);
	return 0;
}

static int pipe_last_read(struct pipe *pipe, void *buf, size_t nbyte) {
	void *cbuf = buf;
	int len;

	do {
		len = ring_buff_dequeue(pipe->buff, buf, nbyte);
		if (len <= 0) {
			break;
		}

		nbyte -= len;
	} while (nbyte > 0);

	return cbuf - buf;
}

int pipe_read(struct idesc *idesc, void *buf, size_t nbyte) {
	struct pipe *pipe;
	void *cbuf;
	int res;

	if (0 == nbyte) {
		return 0;
	}

	pipe = ((struct idesc_pipe *) idesc)->pipe;
	mutex_lock(&pipe->mutex);

	assert(idesc == pipe->reading_end);

	if (NULL == pipe->writing_end) {
		res = pipe_last_read(pipe, buf, nbyte);
		goto out_unlock;
	}

	cbuf = buf;

	do {
		res = ring_buff_dequeue(pipe->buff, cbuf, nbyte);
		if (res > 0) {

			assert(res <= nbyte);

			cbuf += res;
			nbyte -= res;
		} else {
			struct idesc_wait_link wl;

			idesc_wait_prepare(idesc, &wl, IDESC_EVENT_READ | IDESC_EVENT_ERROR);

			mutex_unlock(&pipe->mutex);
			res = idesc_wait(&wl, SCHED_TIMEOUT_INFINITE);
			mutex_lock(&pipe->mutex);

			idesc_wait_cleanup(&wl);

			assert(res != -ETIMEDOUT);
			if (res < 0) {
				break;
			}

			/* is the error event occured? Let's check another end of pipe */
			if (NULL == pipe->writing_end) {
				res = (cbuf - buf) + pipe_last_read(pipe, cbuf, nbyte);
				goto out_unlock;
			}
		}
	} while (nbyte > 0);

	if (cbuf != buf) {
		idesc_notify_all(pipe->writing_end, IDESC_EVENT_WRITE);
		res = cbuf - buf;
	}

out_unlock:
	mutex_unlock(&pipe->mutex);
	return res;
}

int pipe_write(struct idesc *idesc, const void *buf, size_t nbyte) {
	struct pipe *pipe;
	const void *cbuf;
	int res;

	if (0 == nbyte) {
		return 0;
	}

	pipe = ((struct idesc_pipe *) idesc)->pipe;
	mutex_lock(&pipe->mutex);

	assert(idesc == pipe->writing_end);

	/* If writing end is closed that means it was last data in pipe. */
	if (NULL == pipe->reading_end) {
		res = -EPIPE;
		goto out_unlock;
	}

	cbuf = buf;

	do {
		res = ring_buff_enqueue(pipe->buff, (void *) cbuf, nbyte);
		if (res > 0) {

			assert(res <= nbyte);

			cbuf += res;
			nbyte -= res;
		} else {
			struct idesc_wait_link wl;

			idesc_wait_prepare(idesc, &wl, IDESC_EVENT_WRITE | IDESC_EVENT_ERROR);

			mutex_unlock(&pipe->mutex);
			res = idesc_wait(&wl, SCHED_TIMEOUT_INFINITE);
			mutex_lock(&pipe->mutex);

			idesc_wait_cleanup(&wl);

			assert(res != -ETIMEDOUT);
			if (res < 0) {
				break;
			}

			/* is the error event occured? Let's check another end of pipe */
			if (NULL == pipe->reading_end) {
				res = cbuf - buf;
				goto out_unlock;
			}
		}
	} while (nbyte > 0);

	if (cbuf != buf) {
		idesc_notify_all(pipe->reading_end, IDESC_EVENT_READ);
		res = cbuf - buf;
	}

out_unlock:
	mutex_unlock(&pipe->mutex);
	return res;
}

#if 0
static int pipe_fcntl(struct idesc *data, int cmd, va_list args) {
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

	mutex_init(&pipe->mutex);

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

//	io_sync_enable(&pipe->writing_end->idesc_event.io_sync, IO_SYNC_WRITING);

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
