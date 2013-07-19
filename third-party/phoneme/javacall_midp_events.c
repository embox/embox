/**
 * @file
 * @brief
 *
 * @date 14.01.13
 * @author Alexander Kalmuk
 */

#include <javacall_events.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#include <kernel/thread/sync/mutex.h>

/**
 * Note:
 * We made send/recv operations throw pipe between kernel task and java task.
 * That means any another tasks could not send/recv events.
 * */

static int java_pipe[2];

static struct mutex java_global_mutex;

javacall_result javacall_event_send(unsigned char* binaryBuffer,
                                    int binaryBufferLen) {
	mutex_lock(&java_global_mutex);
	{
		/* blocking writing */
		write(java_pipe[1], &binaryBufferLen, 4);
		write(java_pipe[1], binaryBuffer, binaryBufferLen);
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

	FD_ZERO(&readfds);
	FD_SET(java_pipe[0], &readfds);

	timeout.tv_sec = timeToWaitInMillisec / 1000;
	timeout.tv_usec = (timeToWaitInMillisec % 1000) * 1000;

	fd_cnt = select(java_pipe[0] + 1, &readfds, NULL, NULL, &timeout);

	if (fd_cnt != 1) {
		res = JAVACALL_FAIL;
		goto out;
	}

	read(java_pipe[0], &actual_size, 4);
	if (actual_size > binaryBufferMaxLen) {
		res = JAVACALL_OUT_OF_MEMORY;
		actual_size = binaryBufferMaxLen;
	}
	*outEventLen = read(java_pipe[0], binaryBuffer, actual_size);

out:
	return res;
}

javacall_result javacall_create_event_queue_lock(void) {
	return JAVACALL_OK;
}

javacall_result javacall_wait_and_lock_event_queue(void) {
	mutex_lock(&java_global_mutex);
	return JAVACALL_OK;
}

javacall_result javacall_unlock_event_queue(void) {
	mutex_unlock(&java_global_mutex);
	return JAVACALL_OK;
}

javacall_result javacall_destroy_event_queue_lock(void) {
	return JAVACALL_OK;
}

javacall_result javacall_events_init(void) {
	if (-1 == pipe(java_pipe)) {
		return JAVACALL_FAIL;
	}
	mutex_init(&java_global_mutex);
	return JAVACALL_OK;
}
