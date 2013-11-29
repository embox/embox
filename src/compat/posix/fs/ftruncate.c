/**
 * @file
 *
 * @date Nov 15, 2013
 * @author: Anton Bondarev
 */

#include <unistd.h>
#include <errno.h>

#include <fs/index_descriptor.h>

int ftruncate(int fd, off_t length) {
	//ktruncate()
	assert(0);
	return 0;
}
