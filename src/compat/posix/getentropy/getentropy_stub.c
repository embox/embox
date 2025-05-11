/**
 * @file
 * @brief getentropy function stub.
 *
 * @see unistd.h
 *
 * @date 11.05.25
 * @author Dmitry Pilyuk
 */

#include <errno.h>
#include <stddef.h>

int getentropy(void *buffer, size_t length) {
	SET_ERRNO(ENOSYS);
	return -1;
}