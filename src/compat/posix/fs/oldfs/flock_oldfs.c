/**
 * @file
 * @brief
 *
 * @date 16.09.2011
 * @author Anton Bondarev
 */

#include <sys/file.h>

#include <fs/kfsop.h>

int flock(int fd, int operation) {
	return kflock(fd, operation);
}
