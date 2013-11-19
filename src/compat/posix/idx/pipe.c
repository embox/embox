/**
 * @file
 * @brief Pipe implementation.
 *
 * @date 12.09.2012
 * @author Alexander Kalmuk
 */

#if 0
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

//#include "../kernel/task/common.h" //TODO this is very bad way to include headers
#include <kernel/task.h>
#include <kernel/task/idx.h>
#include <kernel/task/io_sync.h>
#include <kernel/sched.h>
#include <kernel/event.h>

#include <util/ring_buff.h>
#include <util/ring.h>
#include <framework/mod/options.h>

#include <termios.h>
#include <drivers/termios_ops.h>
#include <fs/pipe.h>


//static int pipe_close(struct idx_desc *desc);
//static int pipe_read(struct idx_desc *data, void *buf, size_t nbyte);
//static int pipe_write(struct idx_desc *data, const void *buf, size_t nbyte);
//static int pipe_fcntl(struct idx_desc *data, int cmd, va_list args);

/* Set size of pipe's buffer. */
//static void pipe_set_buf_size(struct pipe *pipe, size_t size);

int pipe(int pipefd[2]) {
	struct pipe *pipe;
	struct ring_buff *pipe_buff;
	void *storage;
	int res = 0;

	pipe = storage = NULL;
	pipe_buff = NULL;

	if (!(storage = malloc(DEFAULT_PIPE_BUFFER_SIZE))
			|| !(pipe = malloc(sizeof(struct pipe)))
			|| !(pipe_buff = malloc(sizeof(struct ring_buff)))) {
		res = ENOMEM;
		goto free_memory;
	}

	pipe->buff = pipe_buff;
	pipe->buf_size = DEFAULT_PIPE_BUFFER_SIZE;
	ring_buff_init(pipe_buff, 1, DEFAULT_PIPE_BUFFER_SIZE, storage);
	io_sync_init(&pipe->ios_read, 0, 0);
	io_sync_init(&pipe->ios_write, 0, 0);

#if 0
	pipefd[1] = task_self_idx_alloc(&write_ops, pipe,
			&pipe->ios_write);
	if (pipefd[1] < 0) {
		res = EMFILE;
		goto free_memory;
	}

	pipefd[0] = task_self_idx_alloc(&read_ops, pipe,
			&pipe->ios_read);
	if (pipefd[0] < 0) {
		task_self_idx_table_unbind(pipefd[1]);
		res = EMFILE;
		goto free_memory;
	}

	/* Init reader */
	event_init(&pipe->read_wait, "pipe_read_wait");
	pipe->reading_end = task_idx_indata(task_self_idx_get(pipefd[0]));

	/* Init writer */
	event_init(&pipe->write_wait, "pipe_write_wait");
	pipe->writing_end = task_idx_indata(task_self_idx_get(pipefd[1]));

	/* And enable writing in pipe */
	io_sync_enable(pipe->writing_end->ios, IO_SYNC_WRITING);
#endif
	return 0;

free_memory:
	if (storage)   free(storage);
	if (pipe_buff) free(pipe_buff);
	if (pipe)      free(pipe);
	SET_ERRNO(res);
	return -1;
}
#endif

