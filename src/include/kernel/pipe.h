/**
 * @file
 * @brief Defines pipe structure and methods associated with it.
 *
 *
 * @date 21.11.2010
 * @author Vladimir Muhin
 */

//#include <kernel/thread.h>

const int sync_size = 1;

struct pipe {
	int thread_num; //number of thread needs to synchronize
	int sync_data[thread_num * sync_size]; //data to synchronize
};
/*
 * In our pipe each thread have 1 'int' elements to write itself data
 * */

struct pipe *pipe_create(int number_of_threads);

static int synchronize(int threads_num, int data_to_sync, int thread_ID);

int pipe_read(int thread_ID);

int pipe_write(int thread_ID, int data_to_write);

void pipe_flush(void);
