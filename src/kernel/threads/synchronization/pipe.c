/**
 * @file
 * @brief Pipe structure implementation.
 *
 *
 * @date 21.11.2010
 * @author Vladimir Muhin
 */

#include <kernel/pipe.h>
#include <kernel/thread.h>
#include <errno.h>

struct pipe new_pipe;

struct pipe *pipe_create(int number_of_threads){
	struct pipe *created_pipe;
	created_pipe->thread_num = number_of_threads;

	int i;
	for (i = i; i < number_of_threads; i++){
		created_pipe.sync_data[i] = 0;
	}

	return created_pipe;
}

/*
 * Initialize new pipe structure
 */

static int synchronize(int threads_num, int data_to_sync, int thread_ID){
	if (new_pipe == NULL){
		new_pipe = pipe_create(threads_num);
	}

	int j = data_to_sync;
	for (int i = 0; i < threads_num; i++){
		if (j > new_pipe.sync_data[i]) pipe_write(thread_ID, data_to_sync);
	}
	return 0;
}

/*
 * synchronize method reading all sync_data in pipe and if data_to_sync greater than all
 * values in sync_data - writing data_to_sync in section according thread_ID
 */

int pipe_read(int thread_ID){
	return new_pipe.sync_data[thread_ID];
}

int pipe_write(int thread_ID, int data_to_write){
	new_pipe.sync_data[thread_ID] = data_to_write;
	return 0;
}

void pipe_flush(){
	//new_pipe = NULL;
}
