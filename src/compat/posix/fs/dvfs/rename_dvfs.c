/**
 * @file
 * @brief Implementation of POSIX function "rename" for new VFS
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-11-19
 */

#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>

#include <fs/dvfs.h>

#define FS_BUFFER_SZ 512

/**
 * @brief Change the name of the file. Actually it moves file from one location
 * to another
 *
 * @param src_name Old pathname of the file
 * @param dst_name New pathname of the file
 *
 * @return 0 if succeed and -1 if failed, errno is set
 */
int rename(const char *src_name, const char *dst_name) {
	int err;

	if (!src_name || !dst_name) {
		return SET_ERRNO(EFAULT);
	}

	err = dvfs_rename(src_name, dst_name);
	if (err) {
		return SET_ERRNO(-err);
	}

	return 0;
}
