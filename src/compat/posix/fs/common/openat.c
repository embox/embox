/**
 * @brief
 *
 * @date 03.12.25
 * @author Aleksey Zhmulin
 */

#include <fcntl.h>

int openat(int fildes, const char *path, int __oflag, ...) {
	return open(path, __oflag);
}
