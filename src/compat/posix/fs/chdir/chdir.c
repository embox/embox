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
		SET_ERRNO(ENAMETOOLONG);
		return -1;
	}

	if (stat(path, &buff)) {
		/* SET_ERRNO(ENOENT); already set by stat()*/
		return -1;
	}

	/*check if it is a directory*/
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
