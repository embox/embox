/**
 * @file
 * @brief Pipe manager structure.
 *
 *
 * @date 25.11.10
 * @author Vladimir Muhin
 */

#define MAX_PIPE_SIZE 0x10

struct n_pipe{
	char sync_data[MAX_PIPE_SIZE];
	unsigned int ready_to_read,
				 ready_to_write,
				 on,
				 is_full;
	int read_index, write_index;
};

int pipe_create(void);

int pipe_flush(int pipe);

int free_pipe_status(void);

int pipe_write(int pipe, char data);

char pipe_read(int pipe);

