/**
 * @file
 * @brief
 *
 * @date 14.01.13
 * @author Alexander Kalmuk
 */

#include <javacall_events.h>
#include <sys/select.h>
#include <time.h>
#include <unistd.h>
#include <kernel/thread/sync/mutex.h>

int java_pipe_read = 0, java_pipe_write = 0;

static struct mutex java_global_mutex;

javacall_result javacall_event_send(unsigned char* binaryBuffer,
                                    int binaryBufferLen) {
	mutex_lock(&java_global_mutex);
	{
		/* blocking writing */
		write(java_pipe_write, binaryBuffer, binaryBufferLen);
		write(java_pipe_write, &binaryBufferLen, 4);
	}
	mutex_unlock(&java_global_mutex);
	return JAVACALL_OK;
}

javacall_result javacall_event_receive(
                            long                    timeToWaitInMillisec, /* XXX unused*/
                            /*OUT*/ unsigned char*  binaryBuffer,
                            int                     binaryBufferMaxLen,
                            /*OUT*/ int*            outEventLen) {
	size_t actual_size;
	struct timeval timeout;
	fd_set readfds;
	javacall_result res = JAVACALL_OK;
	int fd_cnt = 0;

	mutex_lock(&java_global_mutex);
	{
		FD_ZERO(&readfds);
		FD_SET(java_pipe_read, &readfds);

		timeout.tv_sec = timeToWaitInMillisec / 1000;
		timeout.tv_usec = (timeToWaitInMillisec % 1000) * 1000;

		fd_cnt = select(java_pipe_read + 1, &readfds, NULL, NULL, &timeout);

		if (fd_cnt != 1) {
			res = JAVACALL_FAIL;
			goto out;
		}

		read(java_pipe_read, &actual_size, 4);
		if (actual_size > binaryBufferMaxLen) {
			res = JAVACALL_OUT_OF_MEMORY;
			actual_size = binaryBufferMaxLen;
		}
		*outEventLen = read(java_pipe_read, binaryBuffer, actual_size);
	}

out:
	mutex_unlock(&java_global_mutex);
	return res;
}

javacall_result javacall_events_init(void) {
	mutex_init(&java_global_mutex);
	return JAVACALL_OK;
}
