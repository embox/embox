/**
 * @file
 * @brief
 *
 * @date 3 Apr 2015
 * @author Denis Deryugin
 */

#include <errno.h>
#include <unistd.h>

#include <fs/dvfs.h>

int rmdir(const char *pathname) {
	int res = dvfs_rmdir(pathname);

	if (res != 0) {
		return SET_ERRNO(-res);
	}

	return 0;
}
