/**
 * @file
 *
 * @date Jan 10, 2014
 * @author: Anton Bondarev
 */

#include <errno.h>
#include <sys/types.h>

pid_t kfork(void) {
	return -ENOSUPP;
}

