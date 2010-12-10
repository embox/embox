/**
 * @file
 * @brief Pipe manager implementation.
 *
 * @date 27.11.2010
 * @author Vladimir Muhin
 */
#include <kernel/pipe_manager.h>
#include <errno.h>
#include <embox/unit.h>

#define MAX_PIPES_COUNT 0x10
#define true 1
#define false 0
#define FULL_POOL_ERROR -1
#define EMPTY_PIPE -11
#define SYNC_DATA_INIT_SYMB '_'

EMBOX_UNIT_INIT(pool_init);

static struct n_pipe pipe_pool[MAX_PIPES_COUNT];

static int pool_init(void) {
	int i, j;
	for (i = 0; i < MAX_PIPES_COUNT; i++) {
		pipe_pool[i].on = false;
		pipe_pool[i].ready_to_read = false;
		pipe_pool[i].ready_to_write = false;
		pipe_pool[i].is_full = false;
		pipe_pool[i].read_index = 0;
		pipe_pool[i].write_index = 0;

		for (j = 0; j < MAX_PIPE_SIZE; j++) {
			pipe_pool[i].sync_data[j] = SYNC_DATA_INIT_SYMB;
		}
	}
	return 0;
}

int free_pipe_status(void) { /* returns free pipes number */
	int i;
	for (i = 0; i < MAX_PIPES_COUNT; i++){
		if (!pipe_pool[i].on) return MAX_PIPES_COUNT - (i++);
	}
	return FULL_POOL_ERROR;
}

int pipe_flush(int pipe){
	struct n_pipe *tmp_pipe = &pipe_pool[pipe];
	tmp_pipe->is_full = false;
	tmp_pipe->read_index = 0;
	tmp_pipe->write_index = 0;
	int i;
	for (int i = 0; i < MAX_PIPE_SIZE; i++){
		tmp_pipe->sync_data[i] = SYNC_DATA_INIT_SYMB;
	}
	return 0;
}

int pipe_create(void) {
	int i;
	for (i = 0; i < MAX_PIPES_COUNT; i++) {
		if (!pipe_pool[i].on) {
			pipe_pool[i].on = true;
			pipe_pool[i].ready_to_read = true;
			pipe_pool[i].ready_to_write = true;
			pipe_pool[i].read_index = 0;
			pipe_pool[i].write_index = 0;
			return 0;
		}
	}
	return FULL_POOL_ERROR;
}

char pipe_read(int pipe) {
	struct n_pipe *tmp_pipe = &pipe_pool[pipe];

	if ((tmp_pipe->read_index = tmp_pipe->write_index) &&
			tmp_pipe->is_full) return EMPTY_PIPE;

	int i = tmp_pipe->read_index;
	if (tmp_pipe->read_index < MAX_PIPE_SIZE - 1) tmp_pipe->read_index++;
		else tmp_pipe->read_index = 0;

	if (tmp_pipe->read_index = tmp_pipe->write_index)
		tmp_pipe->is_full = true;
	else tmp_pipe->is_full = false;

	return tmp_pipe->sync_data[i];
}

int pipe_write(int pipe, char data) {
	struct n_pipe *tmp_pipe = &pipe_pool[pipe];
	int i = tmp_pipe->read_index,
		j = tmp_pipe->write_index;

	if ((i = j) && (tmp_pipe->sync_data[j++] = SYNC_DATA_INIT_SYMB)) {
		tmp_pipe->sync_data[j] = &data;
		tmp_pipe->write_index++;
		return 0;
	}

	if (i != j) {
    	tmp_pipe->sync_data[j] = &data;
    	if (tmp_pipe->write_index < MAX_PIPE_SIZE - 1) tmp_pipe->write_index++;
			else tmp_pipe->write_index = 0;
    	return 0;
    }

	if ((i = j) && (tmp_pipe->sync_data[j++] != SYNC_DATA_INIT_SYMB)) {
		tmp_pipe->is_full = true;
		return ENOMEM;
	}
}

