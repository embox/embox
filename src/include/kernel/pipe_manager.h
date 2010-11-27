/**
 * @file
 * @brief Pipe manager structure.
 *
 *
 * @date 25.11.2010
 * @author Vladimir Muhin
 */

#define MAX_PIPE_SIZE 0x10

struct n_pipe{
	int sync_data[MAX_PIPE_SIZE];
	_Bool ready_to_read, ready_to_write, on;
};


struct n_pipe *pipe_create(void);

void pipe_section_write(int pipe,
		int section, int data);

int pipe_section_read(int pipe, int section);

