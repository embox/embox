/**
 * @file
 * @brief
 *
 * @date 16.09.2011
 * @author Anton Bondarev
 */

#include <sys/stat.h>
#include <sys/types.h>

#include <fs/kfsop.h>

extern mode_t umask_modify(mode_t newmode);

int mkdir(const char *pathname, mode_t mode) {
	int rc;

	rc = kmkdir(pathname, umask_modify(mode));

	return rc;
}
