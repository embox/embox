/**
 * @file
 * @brief
 *
 * @date 3 Apr 2015
 * @author Denis Deryugin
 */

#include <errno.h>
#include <stdio.h>

#include <fs/dvfs.h>

int remove(const char *pathname) {
	int ret = dvfs_remove(pathname);
	if (ret != 0) {
		return SET_ERRNO(-ret);
	}

	return 0;
}
