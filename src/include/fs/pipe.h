/**
 * @file
 *
 * @brief
 *
 * @date 17.11.2013
 * @author Anton Bondarev
 */

#ifndef PIPE_H_
#define PIPE_H_

#include <kernel/event.h>
#include <kernel/task/io_sync.h>

#include <termios.h>
#include <framework/mod/options.h>

#define DEFAULT_PIPE_BUFFER_SIZE OPTION_GET(NUMBER, pipe_buffer_size)
#define MAX_PIPE_BUFFER_SIZE     OPTION_GET(NUMBER, max_pipe_buffer_size)

struct ring_buff;
struct idx_desc_data;

struct pipe {
	struct ring_buff *buff;       /**< Buffer to store data */
	size_t buf_size;              /**< Size of buffer. May be changed by pipe_set_buf_size() */
	struct idx_desc_data *reading_end; /**< Reading end of pipe */
	struct idx_desc_data *writing_end; /**< Writing end of pipe */
	struct event read_wait;       /**< Event to activate reading when pipe was not empty */
	struct event write_wait;      /**< Event to activate writing when pipe was not full */
	struct io_sync ios_read;
	struct io_sync ios_write;

	/* FIXME used by pipe io, will follow pipe_pty when thrown  */
	char has_tio;
	struct termios tio;
};


#endif /* PIPE_H_ */
