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
#include <sys/stat.h>
#include <sys/uio.h>
#include <fcntl.h>

#include <lib/libds/ring_buff.h>

#include <framework/mod/options.h>
#include <kernel/thread/sync/mutex.h>
#include <kernel/task.h>
#include <kernel/task/resource/idesc_table.h>
#include <kernel/task/resource/idesc.h>
#include <kernel/task/resource/idesc_event.h>

#include <kernel/thread/thread_sched_wait.h>

#include <kernel/sched.h>
#include <mem/sysmalloc.h>



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
    int write_close_event;          /* When write end of pipe closed set to 1 */
};

static const struct idesc_ops idesc_pipe_ops;

static inline int idesc_pipe_isclosed(struct idesc_pipe *ipipe) {
	return (ipipe->idesc.idesc_flags & O_ACCESS_MASK) == O_ACCESS_MASK;
}

static int idesc_pipe_close(struct idesc_pipe *cur, struct idesc_pipe *other) {

	cur->idesc.idesc_flags |= O_ACCESS_MASK;

	if (!idesc_pipe_isclosed(other)) {
		idesc_notify(&other->idesc, POLLERR);
	} else {
		return 1;
	}

	return 0;
}

static void pipe_close(struct idesc *idesc) {
	struct pipe *pipe;
	struct idesc_pipe *cur, *other;
	int ret;

	assert(idesc);
	assert(idesc->idesc_ops == &idesc_pipe_ops);

	cur = ((struct idesc_pipe *) idesc);
	pipe = idesc_to_pipe(idesc);
	mutex_lock(&pipe->mutex);

	if (cur == &pipe->read_desc) {
		other = &pipe->write_desc;
	} else {
		assert(cur == &pipe->write_desc);
        pipe->write_close_event = 1; /*set this for read end notification*/
		other = &pipe->read_desc;
	}

	ret = idesc_pipe_close(cur, other);
	mutex_unlock(&pipe->mutex);
	if (ret) {
		sysfree(pipe->buff->storage);
		sysfree(pipe->buff);
		sysfree(pipe);
	}
}

static int pipe_wait(struct idesc *idesc, struct pipe *pipe, int flags) {
	struct idesc_wait_link wl;

	return IDESC_WAIT_LOCKED(
		mutex_unlock(&pipe->mutex),
		idesc, &wl, flags, SCHED_TIMEOUT_INFINITE,
		mutex_lock(&pipe->mutex));
}

static ssize_t pipe_read(struct idesc *idesc, const struct iovec *iov, int cnt) {
	struct pipe *pipe;
	ssize_t res;
	void *buf;
	size_t nbyte;

	assert(iov);
	buf = iov->iov_base;
	assert(cnt == 1);
	nbyte = iov->iov_len;
	assert(buf);
	assert(idesc);
	assert(idesc->idesc_ops == &idesc_pipe_ops);
	assert((idesc->idesc_flags & O_ACCESS_MASK) != O_WRONLY);

	if (!nbyte) {
		return 0;
	}

	pipe = idesc_to_pipe(idesc);
	mutex_lock(&pipe->mutex);
	do {
		res = ring_buff_dequeue(pipe->buff, buf, nbyte);

		if (idesc_pipe_isclosed(&pipe->write_desc)) {
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
	mutex_unlock(&pipe->mutex);

	return res;
}

static ssize_t pipe_write(struct idesc *idesc, const struct iovec *iov, int cnt) {
	size_t nbyte;
	struct pipe *pipe;
	const void *cbuf;
	int len;
	ssize_t res;

	assert(iov);
	assert(iov->iov_base);
	assert(cnt == 1);
	assert(idesc);
	assert(idesc->idesc_ops == &idesc_pipe_ops);
	assert((idesc->idesc_flags & O_ACCESS_MASK) != O_RDONLY);

	cbuf = iov->iov_base;
	nbyte = iov->iov_len;
	/* nbyte == 0 is ok to passthrough */

	pipe = idesc_to_pipe(idesc);
	mutex_lock(&pipe->mutex);
	do {
		/* No data can be readed at all */
		if (idesc_pipe_isclosed(&pipe->read_desc)) {
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
			res = cbuf - iov->iov_base;
			break;
		}

		res = pipe_wait(idesc, pipe, POLLOUT | POLLERR);
	} while (res == 0);
	mutex_unlock(&pipe->mutex);

	return res;
}

static int pipe_fcntl(struct idesc *data, int cmd, void * args) {
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

	res = 0;
	mutex_lock(&pipe->mutex);
	switch (mask) {
	case POLLIN:
		/* how many we can read */
		res = ring_buff_get_cnt(pipe->buff);
        if(!res) {
            res = pipe->write_close_event; /*after write-end is closed, read-end
                                            will always be ready to read*/
        }
		goto out;
	case POLLOUT:
		/* how many we can write */
		res = pipe->buf_size - ring_buff_get_cnt(pipe->buff);
		goto out;
	case POLLERR:
		/* is there any exeptions */
		res = 0; //TODO Where is errors counter
		goto out;
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

out:
	mutex_unlock(&pipe->mutex);

	return res;
}

static const struct idesc_ops idesc_pipe_ops = {
		.id_readv = pipe_read,
		.id_writev = pipe_write,
		.close = pipe_close,
		.ioctl = pipe_fcntl,
		.status = idesc_pipe_status
		/*.fcntl = pipe_fcntl,*/
};

static int idesc_pipe_init(struct idesc_pipe *pdesc, struct pipe *pipe,
		mode_t amode) {

	idesc_init(&pdesc->idesc, &idesc_pipe_ops, amode);

	pdesc->pipe = pipe;


	return 0;
}

static struct pipe *pipe_alloc(void) {
	struct pipe *pipe;
	struct ring_buff *pipe_buff;
	void *storage;

	storage = sysmalloc(DEFAULT_PIPE_BUFFER_SIZE);
	if (!storage) {
		return NULL;
	}
	pipe = sysmalloc(sizeof(struct pipe));
	if (!pipe) {
		sysfree(storage);
		return NULL;
	}
	pipe_buff = sysmalloc(sizeof(struct ring_buff));
	if (!pipe_buff) {
		sysfree(storage);
		sysfree(pipe);
		return NULL;
	}

	pipe->buff = pipe_buff;
	pipe->buf_size = DEFAULT_PIPE_BUFFER_SIZE - 1;
	ring_buff_init(pipe_buff, 1, DEFAULT_PIPE_BUFFER_SIZE, storage);
    pipe->write_close_event = 0;

	mutex_init(&pipe->mutex);

	return pipe;
}

static void pipe_free(struct pipe *pipe) {
	sysfree(pipe->buff->storage);
	sysfree(pipe->buff);
	sysfree(pipe);
}

int pipe(int pipefd[2]) {
	return pipe2(pipefd, 0);
}

int pipe2(int pipefd[2], int flags) {
	struct idesc_table *it;
	struct pipe *pipe;
	int res = 0;

	it = task_resource_idesc_table(task_self());
	assert(it);


	pipe = pipe_alloc();
	if (!pipe) {
		res = ENOMEM;
		goto out_err;
	}


	idesc_pipe_init(&pipe->read_desc, pipe, O_RDONLY);
	idesc_pipe_init(&pipe->write_desc, pipe, O_WRONLY);


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
