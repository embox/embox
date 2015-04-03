/**
 * @file
 * @brief
 *
 * @date 5 Apr 2015
 * @author Denis Deryugin
 */

#include <dirent.h>
#include <defines/null.h>

DIR *opendir(const char *path) {
	return NULL;
}

int closedir(DIR *dir) {
	return 0;
}

struct dirent *readdir(DIR *dir) {
	return NULL;
}
