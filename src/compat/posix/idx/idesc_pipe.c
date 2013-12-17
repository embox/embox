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
#include <poll.h>
#include <unistd.h>

#include <util/ring_buff.h>

#include <framework/mod/options.h>
#include <kernel/thread/sync/mutex.h>
//#include <kernel/task/idx.h>
#include <kernel/task.h>
#include <kernel/task/idesc_table.h>
#include <fs/idesc.h>
#include <fs/idesc_event.h>

#include <kernel/sched.h>
#include <fs/flags.h>



#define DEFAULT_PIPE_BUFFER_SIZE OPTION_GET(NUMBER, pipe_buffer_size)
#define MAX_PIPE_BUFFER_SIZE     OPTION_GET(NUMBER, max_pipe_buffer_size)


#define idesc_to_pipe(desc) ((struct idesc_pipe *) desc)->pipe

struct ring_buff;
struct idesc;
struct pipe;

struct idesc_pipe {
	struct idesc idesc;
	struct pipe *pipe;
};

struct pipe {
	struct ring_buff *buff;         /**< Buffer to store data */
	size_t buf_size;                /**< Size of buffer. May be changed by pipe_set_buf_size() */
	struct mutex mutex;             /**< Global pipe mutex */

	struct idesc_pipe read_desc;    /**< Reading end of pipe */
	struct idesc_pipe write_desc;   /**< Writing end of pipe */
};

static int idesc_pipe_close(struct idesc_pipe *cur, struct idesc_pipe *other) {
	cur->idesc.idesc_amode = 0;
	if (other->idesc.idesc_amode) {
		idesc_notify(&other->idesc, POLLERR);
	} else {
		return 1;
	}

	return 0;
}

static const struct idesc_ops idesc_pipe_ops;
static int pipe_close(struct idesc *idesc) {
	struct pipe *pipe;
	struct idesc_pipe *cur;

	assert(idesc);
	assert(idesc->idesc_ops == &idesc_pipe_ops);

	cur = ((struct idesc_pipe *) idesc);

	pipe = idesc_to_pipe(idesc);

	mutex_lock(&pipe->mutex);

	if (cur == &pipe->read_desc) {
		if (idesc_pipe_close(cur, &pipe->write_desc)) {
			goto free_pipe;
		}
		goto out_unlock;
	}

	if (cur == &pipe->write_desc) {
		if (idesc_pipe_close(cur, &pipe->read_desc)) {
			goto free_pipe;
		}
		goto out_unlock;
	}

	mutex_unlock(&pipe->mutex);

	return -EBADF;

free_pipe:
	free(pipe->buff->storage);
	free(pipe->buff);
	free(pipe);

out_unlock:
	mutex_unlock(&pipe->mutex);

	return 0;
}

static int pipe_wait(struct idesc *idesc, struct pipe *pipe, int flags) {
	struct idesc_wait_link wl;
	int res;

	idesc_wait_prepare(idesc, &wl, flags);

	mutex_unlock(&pipe->mutex);

	res = idesc_wait(idesc, flags, SCHED_TIMEOUT_INFINITE);

	mutex_lock(&pipe->mutex);

	idesc_wait_cleanup(idesc, &wl);

	assert(res != -ETIMEDOUT);
	return res;
}

static int pipe_read(struct idesc *idesc, void *buf, size_t nbyte) {
	struct pipe *pipe;
	int res;

	assert(buf);
	assert(idesc);
	assert(idesc->idesc_ops == &idesc_pipe_ops);
	assert(idesc->idesc_amode == FS_MAY_READ);

	pipe = idesc_to_pipe(idesc);
	mutex_lock(&pipe->mutex);

	if (!nbyte) {
		res = 0;
		goto out_unlock;
	}

	do {
		res = ring_buff_dequeue(pipe->buff, buf, nbyte);

		if (0 == pipe->write_desc.idesc.idesc_amode) {
			/* Nothing to do, what's read, that's read */
			break;
		}

		if (res > 0) {
			/* Smth read, notify write end (can't be closed,
 			 * checked already) */
			idesc_notify(&pipe->write_desc.idesc, POLLOUT);
			break;
		}

		res = pipe_wait(idesc, pipe, POLLIN | POLLERR);
	} while (res == 0);

out_unlock:
	mutex_unlock(&pipe->mutex);
	return res;
}

static int pipe_write(struct idesc *idesc, const void *buf, size_t nbyte) {
	struct pipe *pipe;
	const void *cbuf;
	int len, res;

	assert(buf);
	assert(idesc);
	assert(idesc->idesc_ops == &idesc_pipe_ops);
	assert(idesc->idesc_amode == FS_MAY_WRITE);

	pipe = idesc_to_pipe(idesc);
	mutex_lock(&pipe->mutex);

	cbuf = buf;
	/* nbyte == 0 is ok to passthrough */

	do {
		/* No data can be readed at all */
		if (0 == pipe->read_desc.idesc.idesc_amode) {
			res = -EPIPE;
			break;
		}

		/* Try to write some data */
		len = ring_buff_enqueue(pipe->buff, (void *) cbuf, nbyte);
		if (len > 0) {
			/* Notzero was written, adjust pointers and notify
 			 * (read end can't be closed) */
			cbuf += len;
			nbyte -= len;

			idesc_notify(&pipe->read_desc.idesc, POLLIN);
		}

		/* Have nothing to write, exit*/
		if (!nbyte) {
			res = cbuf - buf;
			break;
		}

		res = pipe_wait(idesc, pipe, POLLOUT | POLLERR);
	} while (res == 0);

	mutex_unlock(&pipe->mutex);
	return res;
}

static int pipe_fcntl(struct idesc *data, int cmd, void * args) {
#if 0
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
#endif
	return 0;
}

static int idesc_pipe_status(struct idesc *idesc, int mask) {
	struct pipe *pipe;
	int res;

	assert(idesc);

	if (!mask) {
		return 0;
	}
	pipe = idesc_to_pipe(idesc);
	assert(pipe);

	switch (mask) {
	case POLLIN:
		/* how many we can read */
		return ring_buff_get_cnt(pipe->buff);
	case POLLOUT:
		/* how many we can write */
		return pipe->buf_size - ring_buff_get_cnt(pipe->buff);
	case POLLERR:
		/* is there any exeptions */
		return 0;//TODO Where is errors counter
	default:
		res = 0;
		break;
	}

	if (mask & POLLIN) {
		/* how many we can read */
		res += ring_buff_get_cnt(pipe->buff);
	}

	if (mask & POLLOUT) {
		/* how many we can write */
		res += pipe->buf_size - ring_buff_get_cnt(pipe->buff);
	}

	if (mask & POLLERR) {
		/* is there any exeptions */
		res += 0; //TODO Where is errors counter
	}

	return res;
}


static const struct idesc_ops idesc_pipe_ops = {
		.read = pipe_read,
		.write = pipe_write,
		.close = pipe_close,
		.ioctl = pipe_fcntl,
		.status = idesc_pipe_status
		/*.fcntl = pipe_fcntl,*/
};

static int idesc_pipe_init(struct idesc_pipe *pdesc, struct pipe *pipe,
		idesc_access_mode_t amode) {

	idesc_init(&pdesc->idesc, &idesc_pipe_ops, amode);

	pdesc->pipe = pipe;


	return 0;
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
	int res = 0;

	it = task_get_idesc_table(task_self());
	assert(it);


	pipe = pipe_alloc();
	if (!pipe) {
		res = ENOMEM;
		goto out_err;
	}


	idesc_pipe_init(&pipe->read_desc, pipe, FS_MAY_READ);
	idesc_pipe_init(&pipe->write_desc, pipe, FS_MAY_WRITE);


	pipefd[0] = idesc_table_add(it, &pipe->read_desc.idesc, flags);
	pipefd[1] = idesc_table_add(it, &pipe->write_desc.idesc, flags);

	if (pipefd[0] < 0 || pipefd[1] < 0) {
		res = ENOMEM;
		goto out_err;
	}

	return 0;

out_err:
	if (pipefd[1] >= 0) {
		idesc_table_del(it, pipefd[1]);
	}
	if (pipefd[0] >= 0) {
		idesc_table_del(it, pipefd[0]);
	}
	if (pipe) {
		pipe_free(pipe);
	}

	SET_ERRNO(res);
	return -1;
}
