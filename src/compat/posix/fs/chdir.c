/**
 * @file
 * @brief
 *
 * @author Ilia Vaprol
 * @date 31.03.13
 */

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>

#include <fs/vfs.h>
#include <fs/path.h>
#include <fs/fs_driver.h>
#include <fs/kfsop.h>
#include <fs/perm.h>

int chdir(const char *path) {
	struct node *last;
	struct stat buff;

	if ((path == NULL) || (*path == '\0')
			|| (*path != '/')) {
		SET_ERRNO(ENOENT);
		return -1;
	}

	if (strlen(path) >= PATH_MAX - 1) {
		SET_ERRNO(ENAMETOOLONG);
		return -1;
	}

	/*check if such path exists in fs*/
	if(0 != fs_perm_lookup(vfs_get_root(), path, NULL, &last)){
		SET_ERRNO(ENOENT);
		return -1;
	}

	/*check if it is a directory*/
	kfile_fill_stat(last, &buff);
	if(!S_ISDIR(buff.st_mode)){
		SET_ERRNO(ENOTDIR);
		return -1;
	}

	if (-1 == setenv("PWD", path, 1)) {
		assert(errno == ENOMEM); /* it is the only possible error */
		SET_ERRNO(ENAMETOOLONG);
		return -1;
	}

	return 0;
}
