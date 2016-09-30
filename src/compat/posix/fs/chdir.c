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
#include <fs/hlpr_path.h>
#include <fs/fs_driver.h>
#include <fs/kfsop.h>
#include <fs/perm.h>

int chdir(const char *path) {
	struct path last;
	struct stat buff;
	char strbuf[PATH_MAX] = "";

	if ((path == NULL) || (*path == '\0')) {
		SET_ERRNO(ENOENT);
		return -1;
	}

	if (path[0] != '/') {
		strncpy(strbuf, getenv("PWD"), PATH_MAX - 1);
		strbuf[PATH_MAX - 1] = '\0';
		strcat(strbuf, "/");
	}
	strncat(strbuf, path, PATH_MAX - 1);
	strbuf[PATH_MAX - 1] = '\0';

	if (strlen(path) >= PATH_MAX - 1) {
		SET_ERRNO(ENAMETOOLONG);
		return -1;
	}

	/*check if such path exists in fs*/
	if(0 != fs_perm_lookup(path, NULL, &last)){
		SET_ERRNO(ENOENT);
		return -1;
	}

	/*check if it is a directory*/
	kfile_fill_stat(last.node, &buff);
	if(!S_ISDIR(buff.st_mode)){
		SET_ERRNO(ENOTDIR);
		return -1;
	}

	if (-1 == setenv("PWD", strbuf, 1)) {
		assert(errno == ENOMEM); /* it is the only possible error */
		SET_ERRNO(ENAMETOOLONG);
		return -1;
	}

	return 0;
}
