/**
 * @file
 *
 * @data 01.02.2016
 * @author: Anton Bondarev
 */

#include <errno.h>
#include <unistd.h>

int chown(const char *path, uid_t owner, gid_t group) {
	(void) path;
	(void) owner;
	(void) group;
	return SET_ERRNO(ENOSYS);
}
