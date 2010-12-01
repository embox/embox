/**
 * @file
 * @brief Pipe manager implementation.
 *
 * @date 27.11.2010
 * @author Vladimir Muhin
 */
#include <kernel/pipe_manager.h>

#define MAX_PIPES_COUNT 0x10
#define true 1
#define false 0

struct n_pipe pipe_pool[MAX_PIPES_COUNT];

void pool_init(void) {
	int i;
	for (i = 0; i < MAX_PIPES_COUNT; i++) {
		pipe_pool[i].on = false;
		pipe_pool[i].ready_to_read = false;
		pipe_pool[i].ready_to_write = false;
	}
	for (i = 0; i < MAX_PIPE_SIZE; i++) {
		pipe_pool[i].sync_data[i] = 0;
	}
}

struct n_pipe *pipe_create(void) {
	struct n_pipe *created_pipe;
	int i;
	pool_init();
	for (i = 0; i < MAX_PIPES_COUNT; i++) {
		if (!pipe_pool[i].on) {
			pipe_pool[i].on = true;
			pipe_pool[i].ready_to_read = true;
			pipe_pool[i].ready_to_write = true;
			break;
		}
	}
	created_pipe = &pipe_pool[i];
	return created_pipe;
}

int pipe_section_read(int pipe, int section) {
	int data;
	pipe_pool[pipe].ready_to_write = false;
	data = pipe_pool[pipe].sync_data[section];
	pipe_pool[pipe].ready_to_write = true;
	return data;
}

void pipe_section_write(int pipe, int section, int data) {
	pipe_pool[pipe].ready_to_write = false;
	pipe_pool[pipe].ready_to_read = false;
	pipe_pool[pipe].sync_data[section] = data;
	pipe_pool[pipe].ready_to_write = true;
	pipe_pool[pipe].ready_to_read = true;
}

