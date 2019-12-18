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
#include <sys/stat.h>

int chdir(const char *path) {
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
		return SET_ERRNO(ENAMETOOLONG);
	}

	if (stat(path, &buff)) {
		return SET_ERRNO(ENOENT);
	}

	/*check if it is a directory*/
	if(!S_ISDIR(buff.st_mode)){
		return SET_ERRNO(ENOTDIR);
	}

	if (-1 == setenv("PWD", strbuf, 1)) {
		assert(errno == ENOMEM); /* it is the only possible error */
		return SET_ERRNO(ENAMETOOLONG);
	}

	return 0;
}
