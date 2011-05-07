/**
 * @file
 * @brief Pipe manager implementation.
 *
 * @date 27.11.10
 * @author Vladimir Muhin
 */

#include <kernel/thread/sync/pipe_manager.h>
#include <errno.h>
#include <embox/unit.h>

#define MAX_PIPES_COUNT 0x10
#define true 1
#define false 0

#define UNREADABLE_PIPE 'E'

EMBOX_UNIT_INIT(pool_init);

static struct n_pipe pipe_pool[MAX_PIPES_COUNT];

static int pool_init(void) { /* initializing all pipes in pool */
	size_t i;
	for (i = 0; i < MAX_PIPES_COUNT; i++) {
		pipe_pool[i].on = false;
		pipe_pool[i].ready_to_read = false;
		pipe_pool[i].ready_to_write = false;
		pipe_pool[i].is_full = false;
		pipe_pool[i].read_index = 0;
		pipe_pool[i].write_index = 0;
	}
	return 0;
}

int free_pipe_status(void) { /* returns free pipes number. ENOMEM if pool is full */
	size_t i;
	for (i = 0; i < MAX_PIPES_COUNT; i++) {
		if (!pipe_pool[i].on)
			return MAX_PIPES_COUNT - (i++);
	}
	return ENOMEM;
}

int pipe_flush(int pipe) { /* reinitialize pipe */
	struct n_pipe *tmp_pipe = &pipe_pool[pipe];
	tmp_pipe->is_full = false;
	tmp_pipe->read_index = 0;
	tmp_pipe->write_index = 0;
	tmp_pipe->ready_to_read = false;
	tmp_pipe->ready_to_write = true;
	tmp_pipe->on = true;
	tmp_pipe->is_full = false;
	return 0;
}

int pipe_create(void) { /* if pipe pool is full - returns ENOMEM */
	size_t i;
	for (i = 0; i < MAX_PIPES_COUNT; i++) {
		if (!pipe_pool[i].on) {
			pipe_pool[i].on = true;
			pipe_pool[i].ready_to_read = false;
				/* pipe not ready to be read after initialize */
			pipe_pool[i].ready_to_write = true;
			pipe_pool[i].read_index = 0;
			pipe_pool[i].write_index = 0;
			return 0;
		}
	}
	return ENOMEM;
}

char pipe_read(int pipe) {
	size_t i;
	if (pipe_pool[pipe].read_index == pipe_pool[pipe].write_index) {
		pipe_pool[pipe].ready_to_read = false;
		return UNREADABLE_PIPE;
	}

	i = pipe_pool[pipe].read_index;

	if (pipe_pool[pipe].read_index < MAX_PIPE_SIZE - 1) {
		pipe_pool[pipe].read_index++;
	} else{
		pipe_pool[pipe].read_index = 0;
	}
	pipe_pool[pipe].ready_to_write = true;  //Therefore 1 element of pipe was reading
						//pipe opening for writing

	return pipe_pool[pipe].sync_data[i];
}

int pipe_write(int pipe, char data) {
	size_t i, j;

	if (pipe_pool[pipe].ready_to_write) {
		i = pipe_pool[pipe].read_index;
		j = pipe_pool[pipe].write_index;
		pipe_pool[pipe].sync_data[j] = data;

		if (j < MAX_PIPE_SIZE - 1) {
			pipe_pool[pipe].write_index++;
		} else{
			pipe_pool[pipe].write_index = 0;
		}
		if (i == pipe_pool[pipe].write_index) {
			pipe_pool[pipe].ready_to_write = false;
			return 0;
		}
	} else {
		return ENOMEM;
	}
	return ENOERR; /* FIXME if need or delete remark. */
}
