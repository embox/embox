/**
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 03.12.25
 */

#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

/* Embox does not support symbolic links. */

ssize_t readlink(const char *path, char *buf, size_t bufsize) {
	errno = ENOSYS;
	return -1;
}
